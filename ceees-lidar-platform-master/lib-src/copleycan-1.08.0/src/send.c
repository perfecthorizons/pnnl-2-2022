/* send.c */

#include "send.h"
#include "usb.h"
#include "utils.h"
#include "xmitbuff.h"
#include "os/debug.h"

// local data
static int fdLen[] = { 12, 16, 20, 24, 32, 48, 64 };

// local functions
static uint32_t PackBytes( uint8_t *data, int bigEndian );
static int PCI_SendCAN( PortInfo *pPort, CANFW_CANMSG *msg, int timeout );
static int DecodeLength( CANCARD_MSG *can );

/**************************************************************
 * Add a CAN message to the transmit queue.  This function 
 * returns an error of there is no space in the transmit queue.
 *************************************************************/
int SendCAN( PortInfo *pPort, CANCARD_MSG *can )
{
   CardInfo *pCard = pPort->pCard;

   // Convert the CAN message into the format
   // required by the card
   int len = DecodeLength( can );
   if( len < 0 ) return EICAN_ERR_CAN_DATA_LENGTH;

   // Make sure card supports CAN-FD if this was an FD message
   if( (can->flags & EICAN_CANFLG_FD) && !pCard->supportFD )
      return EICAN_ERR_CAN_DATA_LENGTH;

   CANFW_CANMSG msg;
   msg.time  = 0;
   msg.flags = 0x0000FFFF & can->flags;
   msg.msgID = can->id;

   len = (len+3)/4;
   for( int i=0; i<len; i++ )
      msg.data[i] = PackBytes( &can->data[4*i], pCard->bigEndianData );

   // If any files attached to this port require transmit
   // acks, then assign a non-zero ID to this message.
   if( AtomicGet( &pPort->xmitAckCt ) )
   {
      uint32_t id = 0;
      for( int i=0; i<10 && !id; i++ )
	 id = 0x0000FFFF & AtomicInc( &pPort->nextXmitID );

      msg.flags |= id << 16;
   }

   int err;
   if( pCard->hwPtr ) 
      err = PCI_SendCAN( pPort, &msg, can->timeout );
   else
      err = USB_SendCAN( pPort, &msg, can->timeout, len+2 );

   if( err ) return err;

   // Add this to my transmit buffer
   if( AtomicGet( &pPort->xmitAckCt ) )
      XmitBuffAdd( pPort, &msg );

   return 0;
}

// TODO - PCI doesn't support CAN-FD yet
static int PCI_SendCAN( PortInfo *pPort, CANFW_CANMSG *msg, int timeout )
{
   // Clear any pending transmit interrupts.
   // The transmit interrupt is set by the card every
   // time it reads a message from the transmit queue
   QueueInfo *queue = &pPort->send;
   EICAN_INTR_CTRL *intr = (EICAN_INTR_CTRL *)(pPort->pCard->hwPtr + EICAN_INTCTL_BASE);
   writel( queue->mask, &intr->flags );

   uint32_t head, tail, size, newHead;
   while( 1 )
   {
      // Grab the queue mutex
      int err;
      MutexGet( queue->mutex, -1, err );
      if( err ) return err;

      // See if there is space on the queue
      ReadTxqHead( pPort, &head, &tail, &size );

      // Some quick sanity checks.  These should never fail
      if( head >= size )
      {
	 errmsg( "Read invalid command queue head: 0x%08x, tail: 0x%08x, size: 0x%08x\n", head, tail, size );
	 debug( "Reading again for debug: head: 0x%08x, tail: 0x%08x, head: 0x%08x, tail: 0x%08x\n",
		readl( &queue->ctrl->head ), readl( &queue->ctrl->head ), 
		readl( &queue->ctrl->tail ), readl( &queue->ctrl->tail ) );
	 MutexPut( queue->mutex );
	 return EICAN_ERR_QUEUECTRL;
      }

      if( tail >= size )
      {
	 errmsg( "Read invalid command queue head: 0x%08x, tail: 0x%08x, size: 0x%08x\n", head, tail, size );
	 debug( "Reading again for debug: head: 0x%08x, tail: 0x%08x, head: 0x%08x, tail: 0x%08x\n",
		readl( &queue->ctrl->head ), readl( &queue->ctrl->head ), 
		readl( &queue->ctrl->tail ), readl( &queue->ctrl->tail ) );
	 MutexPut( queue->mutex );
	 return EICAN_ERR_QUEUECTRL;
      }

      // See if there is room in the queue
      newHead = head+1;
      if( newHead == size ) newHead = 0;
      if( newHead != tail )
	 break;

      // The transmit queue is full.  I need to wait for 
      // space to become available.
      EventClear( &queue->event );
      MutexPut( queue->mutex );

      // Enable the interrupt for this queue
      IntMaskUpdt( pPort->pCard, queue->mask, 1 );

      // Wait on my event
      err = EventWait( &queue->event, &timeout );
      if( err )
      {
	 debug( "Error %d waiting on transmit event.  Timeout was %d\n", err, timeout );
	 return err;
      }
   }

   // Add my message to the queue
   memcpy_toio( &(queue->ctrl->data[head*EICAN_QUEUE_ENTRY_LEN]), msg, EICAN_QUEUE_ENTRY_LEN*4 );

   writel( newHead, &queue->ctrl->head );

   // Signal an interrupt to the card's micro
   writel( queue->mask, &intr->remote );

   MutexPut( queue->mutex );

   return 0;
}

/**************************************************************
 * Pack 4 bytes into a 32-bit integer
 *************************************************************/
static uint32_t PackBytes( uint8_t *data, int bigEndian )
{
   if( bigEndian )
      return (((uint32_t)data[0])<<24) | (((uint32_t)data[1])<<16) |
             (((uint32_t)data[2])<< 8) | (((uint32_t)data[3]));
   else
      return (((uint32_t)data[3])<<24) | (((uint32_t)data[2])<<16) |
             (((uint32_t)data[1])<< 8) | (((uint32_t)data[0]));
}

// Return the message length in bytes, or -1 if invalid
static int DecodeLength( CANCARD_MSG *can )
{
   int len = can->flags & EICAN_CANFLG_LENGTH;
   if( len <= 8 ) return len;

   if( !(can->flags & EICAN_CANFLG_FD) )
      return -1;

   return fdLen[len-9];
}
