/* usb.c */

#include <linux/usb.h>
#include "usb.h"
#include "utils.h"
#include "xmitbuff.h"
#include "os/debug.h"

// local functions
static void USB_RxInt( struct urb *urb );
static void USB_TxInt( struct urb *urb );

// Enable the USB card.
// This is called when the card is first connected
int USB_CardEnable( CardInfo *pCard )
{
   void *rxBuff = 0;
   void *txBuff[MAX_PORTS] = {0};

   if( pCard->cardRemoved )
   {
      info( "USB_CardEnable called on card that's been removed\n" );
      return -ENODEV;
   }

   struct usb_device *dev = interface_to_usbdev( pCard->os.usbintf );

   // Find the endpoint sizes
   struct usb_host_interface *alt = pCard->os.usbintf->cur_altsetting;
   for( int i=0; i<alt->desc.bNumEndpoints; i++ )
   {
      struct usb_host_endpoint *ep = &alt->endpoint[i];
      if( !ep )
         continue;

      // The endpoint address in the descriptor defines the type of endpoint:
      //   xxxxxxxx
      //   ....\\\\---- Endpoint number
      //   .\\\-------- reserved
      //   \----------- Set for input, clear for output
      if( ep->desc.bEndpointAddress & 0x80 )
         pCard->usbRxBuffLen = ep->desc.wMaxPacketSize;
      else
         pCard->usbTxBuffLen = ep->desc.wMaxPacketSize;
   }

   if( !pCard->usbRxBuffLen )
   {
      info( "Error initializing USB device, couldn't find receive endpoint!\n" );
      goto error;
   }

   if( !pCard->usbTxBuffLen )
   {
      info( "Error initializing USB device, couldn't find receive endpoint!\n" );
      goto error;
   }

   pCard->usbcmd   = usb_alloc_urb( 0, GFP_KERNEL ); 
   pCard->usbintR  = usb_alloc_urb( 0, GFP_KERNEL );
   rxBuff = kmalloc( pCard->usbRxBuffLen, GFP_KERNEL );

   if( !pCard->usbcmd || !pCard->usbintR || !rxBuff )
      goto error;

   for( int i=0; i<pCard->portCt; i++ )
   {
      PortInfo *pPort = &pCard->port[i];
      pPort->usbintX = usb_alloc_urb( 0, GFP_KERNEL );
      pCard->port[i].usbTxBuff = (uint32_t*)kmalloc( pCard->usbTxBuffLen, GFP_KERNEL );
      txBuff[i] = (uint32_t*)kmalloc( pCard->usbTxBuffLen, GFP_KERNEL );

      if( !pPort->usbintX || !pCard->port[i].usbTxBuff || !txBuff[i] )
         goto error;

      usb_fill_int_urb( pPort->usbintX, dev, usb_sndintpipe(dev,2+i), 
                        txBuff[i], 0, USB_TxInt, pPort, 1 );

      EventInit( &pPort->usbTxEvent, 0 );
   }

   usb_fill_int_urb( pCard->usbintR, dev, usb_rcvintpipe(dev,1),
                     rxBuff, pCard->usbRxBuffLen, USB_RxInt, pCard, 1 );

   if( usb_submit_urb( pCard->usbintR, GFP_KERNEL ) ) 
      goto error;

   return 0;

error:
   if( pCard->usbcmd  ) usb_free_urb( pCard->usbcmd  );
   if( pCard->usbintR ) usb_free_urb( pCard->usbintR );
   if( rxBuff ) kfree( rxBuff );

   for( int i=0; i<pCard->portCt; i++ )
   {
      PortInfo *pPort = &pCard->port[i];
      if( pPort->usbintX ) usb_free_urb( pPort->usbintX );
      if( pCard->port[i].usbTxBuff ) kfree( pCard->port[i].usbTxBuff );
      if( txBuff[i] ) kfree( txBuff[i] );
   }

   return -ENOMEM;
}

// Completion handler for receive interrupt URBs
static void USB_RxInt( struct urb *urb )
{
   if( urb->status == -EPROTO )
   {
      errmsg( "USB_RxInit, card not responding.  Not resubmitting URB\n" );
      return;
   }

   CardInfo *pCard = (CardInfo *)urb->context;
   int wct = urb->actual_length/4;

   // Grab the spinlock.
   SPINLOCK_STATE state;
   SpinLockGet( &pCard->spin, &state );

   uint32_t *rxBuff = urb->transfer_buffer;

   while( wct >= 3 )
   {
      CANFW_CANMSG msg;
      msg.time  = *rxBuff++;
      msg.msgID = *rxBuff++;
      msg.flags = *rxBuff++;
      wct -= 3;

      int j, ct = DecodeMsgLen( msg.flags & EICAN_CANFLG_LENGTH );

      ct = (ct+3)/4;

      if( ct > wct ) ct = wct;
      wct -= ct;

      for( j=0; j<ct; j++ )
         msg.data[j] = *rxBuff++;
      for( ; j<16; j++ )
         msg.data[j] = 0;

      int p = (msg.flags & EICAN_CANFLG_PORT)>>12;
      if( p >= pCard->portCt ) continue;

      PortInfo *pPort = &pCard->port[p];

      // If this was a send notify, then only the timestamp and flags
      // are valid.  I have a local copy of the sent message.
      if( msg.flags & EICAN_CANFLG_SENDNOTIFY )
      {
	 if( !XmitBuffGet( pPort, &msg ) )
         {
            debug( "Cound't find xmit message\n" );
	    continue;
         }
      }

      // Add this message to the receive queue of each open file.
      for( int i=0; i<MAX_FILES; i++ )
      {
	 FileInfo *pFile = pPort->files[i];
	 if( !pFile ) break;

	 if( (msg.flags & EICAN_CANFLG_SENDNOTIFY) && !pFile->needXmit )
	    continue;

	 MsgQueuePut( pFile->recvMsgQueue, &msg );
      }
   }

   SpinLockPut( &pCard->spin, &state );

   int err = usb_submit_urb( urb, GFP_ATOMIC );
   if( err ) warn( "USB_RxInt, submit error %d\n", err );
}

static void USB_TxInt( struct urb *urb )
{
   PortInfo *pPort = (PortInfo *)urb->context;
   CardInfo *pCard = pPort->pCard;

   SPINLOCK_STATE state;
   SpinLockGet( &pCard->spin, &state );

   // Swap transmit buffers
   void *tmp = pPort->usbintX->transfer_buffer;
   pPort->usbintX->transfer_buffer = pPort->usbTxBuff;
   pPort->usbTxBuff = tmp;

   // Update the transmit length with the number of bytes 
   // of data available
   pPort->usbintX->transfer_buffer_length = 4*pPort->usbTxLen;
   pPort->usbTxLen = 0;

   if( urb->status == -ENOENT )
      warn( "%d: tx URB killed, not resubmitting\n", MINOR(pPort->os.num) );
   else if( urb->status )
      warn( "%d: tx URB status %d, not resubmitting\n", MINOR(pPort->os.num), pPort->usbintX->status );
   else
   {
      // Submit the transmit URB, even if there's no data.
      // On some systems I start getting errors if I don't always
      // submit a transmit interrupt URB
      int err = usb_submit_urb( pPort->usbintX, GFP_ATOMIC );
      if( err )
         warn( "%d: Error %d submitting transmit URB %d\n", MINOR(pPort->os.num), err, pPort->usbintX->status );
   }

   EventSignal( &pPort->usbTxEvent );
   SpinLockPut( &pCard->spin, &state );
}

int USB_SendCAN( PortInfo *pPort, CANFW_CANMSG *msg, int timeout, int wct )
{
   CardInfo *pCard = pPort->pCard;

   if( pCard->cardRemoved )
   {
      info( "USB_SendCAN called on card that's been removed\n" );
      return -ENODEV;
   }

   // Wait for enough space in the transmit buffer to store this message
   SPINLOCK_STATE state;
   SpinLockGet( &pCard->spin, &state );
   while( 1 )
   {
      if( (pCard->usbTxBuffLen/4 - pPort->usbTxLen) >= wct )
         break;

      EventClear( &pPort->usbTxEvent );
      SpinLockPut( &pCard->spin, &state );

      int err = EventWait( &pPort->usbTxEvent, &timeout );
      if( err )
      {
	 debug( "%d: Error %d waiting on transmit event.  Timeout was %d\n", MINOR(pPort->os.num), err, timeout );
	 return err;
      }

      SpinLockGet( &pCard->spin, &state );
   }

   // Copy my message into the transmit buffer
   uint32_t *ptr = &pPort->usbTxBuff[ pPort->usbTxLen ];
   *ptr++ = msg->msgID;
   *ptr++ = msg->flags;
   for( int i=0; i<wct-2; i++ )
      *ptr++ = msg->data[i];

   pPort->usbTxLen += wct;

   SpinLockPut( &pCard->spin, &state );

   return 0;
}

// Send a command to the USB card and return it's response
int USB_SendCmd( CardInfo *pCard, uint32_t *cmd, uint32_t xCt, uint32_t *rCt )
{
   if( pCard->cardRemoved )
   {
      info( "USB_SendCmd called on card that's been removed\n" );
      return -ENODEV;
   }

   struct usb_device *dev = interface_to_usbdev( pCard->os.usbintf );

   uint8_t op   = (cmd[0]>>16);
   uint8_t port = (cmd[0]>>8);

   int err = 0;

   dma_addr_t dmaAddr;
   uint32_t *buf = (uint32_t *)usb_alloc_coherent( dev, EICAN_CMD_LEN*sizeof(uint32_t), GFP_KERNEL, &dmaAddr );
   if( !buf ) {
      err = -ENOMEM;
      goto fail;
   }

//info( "Sending cmd: 0x%08x (0x%08x), xct: %d, rCt: %d\n", cmd[0], cmd[1], xCt, *rCt );

   if( xCt > EICAN_CMD_LEN )
      xCt = EICAN_CMD_LEN;

   for( int i=1; i<xCt; i++ )
      buf[i-1] = cmd[i];

   int ct = usb_control_msg( dev, usb_sndctrlpipe(dev,0), 0, 0x40, op, port, buf, 4*(xCt-1), HZ );
   if( ct < 0 )
   {
      info( "cmd returned %d\n", ct );
      err = EICAN_ERR_INTERNAL;
      goto fail;
   }

   // Typcal command timeout is 200ms.
   // Flash commands can take longer
   uint32_t timeout;
   switch( op )
   {
      // Theoretical worst case erase time.
      // Normally takes about 15 seconds
      case EICAN_CMD_FIRMWARE:
         timeout = 200*HZ; 
         break;

      // Writing to flash
      case EICAN_CMD_FWDATA:
      case EICAN_CMD_SETPARAM:
         timeout = HZ; 
         break;

      // Normal commands
      default:
         timeout = HZ/5;
         break;
   }
//info( "Waiting on response with timeout %d (%d msec)\n", timeout, 1000*timeout / HZ );

   uint32_t startTime = jiffies;
   int done = 0;
   int max = (*rCt)*4;
   while( (uint32_t)(jiffies-startTime) < timeout )
   {
      ct = usb_control_msg( dev, usb_rcvctrlpipe(dev,0), 1, 0xC0, op, port, buf, max, HZ/10 );
      if( ct < 0 )
      {
         info( "rsp returned %d\n", ct );
         err = EICAN_ERR_INTERNAL;
         goto fail;
      }

//info( "Response 0x%08x, ct: %d\n", buf[0], ct );

      if( (ct>=4) && (((buf[0]>>16)&0xff)==EICAN_ERR_CARD_BUSY) )
      {
         if( op == EICAN_CMD_FIRMWARE )
            msleep_interruptible( 10 );
         else
            schedule();
         continue;
      }

      if( (ct&3) || (ct>EICAN_CMD_LEN*sizeof(uint32_t)) )
      {
         errmsg( "usb_control_msg returned an invalid count %d\n", ct );
         err = EICAN_ERR_INTERNAL;
         goto fail;
      }

      ct >>= 2;
      if( ct > EICAN_CMD_LEN ) ct = EICAN_CMD_LEN;

      done = 1;
      break;
   }

   if( !done )
   {
      errmsg( "Timeout waiting for command response\n" );
      err = EICAN_ERR_TIMEOUT;
      goto fail;
   }

   for( int i=0; i<ct; i++ )
      cmd[i] = buf[i];

   if( (ct != 1 + (cmd[0] & 0x003F)) && (ct != max) )
      warn( "count returned from usb_control_msg (%d), doesn't match lower bits of first word (0x%08x)\n", ct, cmd[0] );

   *rCt = ct;
   err = 0;

   // If the port was just closed, kill the transmit URB
   // to prevent stale messages from being sent next time
   // it's open
   if( op == EICAN_CMD_CLOSEPORT )
   {
      PortInfo *pPort = &pCard->port[port];
      pPort->usbTxLen = 0;
      usb_kill_urb( pPort->usbintX );
   }

   // When a port is opened, I need to re-submit the transmit URB
   if( op == EICAN_CMD_OPENPORT )
   {
      PortInfo *pPort = &pCard->port[port];
      pPort->usbTxLen = 0;
      int err = usb_submit_urb( pPort->usbintX, GFP_ATOMIC );
      if( err ) warn( "%d: Error %d submitting tx URB on open\n", MINOR(pPort->os.num), err );
   }

fail:
   if( buf ) 
      usb_free_coherent( dev, 256, buf, dmaAddr );
   return err;
}

