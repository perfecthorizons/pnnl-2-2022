/***************************************************
 * OS generic card interface to CAN card
 **************************************************/

#include <linux/delay.h>
#include "cardintf.h"
#include "command.h"
#include "main.h"
#include "recv.h"
#include "test.h"
#include "usb.h"
#include "utils.h"
#include "os/debug.h"

// local functions 
static int PCI_SendCmd( CardInfo *pCard, uint32_t *cmd, uint32_t xCt, uint32_t *rCt );
static int ResetCard( CardInfo *pCard, uint32_t *cmd );

/**************************************************************
 * Low level command handler.  This does the dirty work for all
 * basic commands to the card.
 *
 * @param card The card information structure
 * @param cmd  The command data structure
 * @param xCt  The number of 32-bit words to be sent (1 to 64)
 * @param rCt  The maximum number of response words on entry.
 *             On return the actual number of response words 
 *             will be passed here.
 *
 * @return zero if the command was completed (possibly with an error from the card).
 *         non-zero if some problem prevented us from communicating with the card.
 *************************************************************/
int SendCmd( FileInfo *pFile, uint32_t *cmd, uint32_t xCt, uint32_t *rCt )
{
   CardInfo *pCard = pFile->pPort->pCard;

   if( xCt < 1 )
   {
      errmsg( "Command sent to card with zero words of data\n" );
      return EICAN_ERR_BAD_PARAM;
   }

   if( xCt > EICAN_CMD_LEN )
      xCt = EICAN_CMD_LEN;

   // Identify the port to which this command applies
   cmd[0] |= pFile->pPort->mask;

   // Check for special commands handled by the driver
   switch( cmd[0] >> 16 )
   {
      case EICAN_CMD_OPENPORT:
	 if( pFile->open )
	 {
	    debug( "Open command received for a file that already has the port open\n" );
	    cmd[0] = (EICAN_ERR_PORT_OPEN<<16);
	    *rCt = 1;
	    return 0;
	 }

//debug( "Open command received port %d, Initial count was %d\n", (pFile->pPort->mask != 0), AtomicGet(&pFile->pPort->openCt) ); 
	 pFile->open = 1;
	 if( AtomicInc( &pFile->pPort->openCt ) > 1 )
	 {
	    cmd[0] = 0;
	    *rCt = 1;
	    return 0;
	 }

	 // Flush the hardware receive queue before actually opening the port
	 FlushRecvQueue( pFile->pPort );
	 break;

      case EICAN_CMD_CLOSEPORT:

	 // If this file was requesting transmit acks, then
	 // reduce the number of requesters
	 if( pFile->needXmit )
	 {
	    AtomicDec( &pFile->pPort->xmitAckCt );
	    pFile->needXmit = 0;
	 }

	 if( !pFile->open )
	 {
	    cmd[0] = (EICAN_ERR_PORT_CLOSED<<16);
	    *rCt = 1;
	    return 0;
	 }

	 MsgQueueFlush( pFile->recvMsgQueue ); 
	 pFile->open = 0;

//debug( "Close command received port %d, Initial count was %d\n", (pFile->pPort->mask !=0), AtomicGet(&pFile->pPort->openCt) ); 
	 if( AtomicDec( &pFile->pPort->openCt ) > 0 )
	 {
	    cmd[0] = 0;
	    *rCt = 1;
	    return 0;
	 }
	 break;

      case EICAN_CMD_RECVXMIT:
//info( "rx_xmit commnad, xCt: %d, dat: 0x%08x\n", xCt, cmd[1] );
	 if( xCt < 2 )
	    return EICAN_ERR_CMD_TOO_SMALL;

	 if( pFile->needXmit && !cmd[1] )
	    AtomicDec( &pFile->pPort->xmitAckCt );

	 else if( !pFile->needXmit && cmd[1] )
	    AtomicInc( &pFile->pPort->xmitAckCt );

//info( "ack: %d\n", AtomicGet( &pFile->pPort->xmitAckCt ) );

	 pFile->needXmit = cmd[1];
	 cmd[0] = 0;
	 *rCt = 1;
	 return 0;

      case EICAN_CMD_PCIPEEK:
      {
         if( !pCard->hwPtr )
            return EICAN_ERR_INTERNAL;

	 uint32_t *addr = (uint32_t *)(pCard->hwPtr + cmd[1]);
	 int ct = cmd[2] & 0x3F;

	 cmd[0] = ct;
	 for( int i=0; i<ct; i++ )
	    cmd[i+1] = readl( addr++ );

	 *rCt = ct+1;
	 return 0;
      }

      case EICAN_CMD_PCIPOKE:
      {
         if( !pCard->hwPtr ) 
            return EICAN_ERR_INTERNAL;

	 if( xCt < 2 )
	    return EICAN_ERR_CMD_TOO_SMALL;

	 uint32_t *addr = (uint32_t *)(pCard->hwPtr + cmd[1]);
	 for( int i=2; i<xCt; i++ )
	    writel( cmd[i], addr++ );

	 cmd[0] = 0;
	 *rCt = 1;
	 return 0;
      }

      case EICAN_CMD_PCITEST:
      {
         if( !pCard->hwPtr ) 
            return EICAN_ERR_INTERNAL;

	 int err = TestCard( pCard );
	 cmd[0] = err<<16;
	 *rCt = 1;
	 return 0;
      }

      case EICAN_CMD_DRIVERVER:
	 cmd[1] = (VER_MAJOR<<16) | (VER_MINOR<<8) | VER_RELEASE;
	 cmd[0] = 1;
	 *rCt = 2;
	 return 0;

      case EICAN_CMD_RESET:
         if( !pCard->hwPtr ) 
            break;

	 *rCt = 1;
	 return ResetCard( pCard, cmd );

      case EICAN_CMD_RECV_THRESH:
      {
         if( !pCard->hwPtr ) 
            return EICAN_ERR_INTERNAL;

	 uint32_t qSize = readl( &(pFile->pPort->recv.ctrl->size) );

	 if( cmd[1] >= (qSize-1) )
	    cmd[0] = (EICAN_ERR_BAD_PARAM<<16);
	 else
	 {
	    cmd[0] = 0;
	    writel( cmd[1], &(pFile->pPort->recv.ctrl->threshold) );
	 }
	 *rCt = 1;
	 return 0;
      }
   }

   if( pCard->hwPtr ) 
      return PCI_SendCmd( pCard, cmd, xCt, rCt );
   else
      return USB_SendCmd( pCard, cmd, xCt, rCt );
}

static int PCI_SendCmd( CardInfo *pCard, uint32_t *cmd, uint32_t xCt, uint32_t *rCt )
{
   // Wait on a mutex which synchronizes access to the command area
   long ret;
   MutexGet( pCard->cmdMutex, 1000, ret );
   if( ret )
   {
      errmsg( "Error %ld waiting on command mutex\n", ret );
      return EICAN_ERR_CMDMUTEX_HELD;
   }

   // Check the command interrupt flag.  This needs to be set for me
   // to access the command area
   EICAN_INTR_CTRL *intCtl = (EICAN_INTR_CTRL *)(pCard->hwPtr + EICAN_INTCTL_BASE);
   uint32_t flags = readl(&intCtl->flags);
   if( (flags & EICAN_INT_CMD) == 0 )
   {
      errmsg( "Card busy error.\n"
	     " Flags:  0x%08x\n"
	     " Mask:   0x%08x\n"
	     " Remote: 0x%08x\n", readl(&intCtl->flags), readl( &intCtl->mask ), readl( &intCtl->remote ) );

      if( (readl( &intCtl->remote ) & 1) == 0 )
	 errmsg( "Attempting to send command anyway\n" );
      else
      {
	 ret = EICAN_ERR_CARD_BUSY;
	 goto done;
      }
   }

   // Write the command data to the card
   uint32_t *cmdBuff = (uint32_t *)(pCard->hwPtr + EICAN_CMD_BASE);
   for( int i=0; i<xCt; i++ )
      writel( cmd[i], &cmdBuff[i] );

   // Typcal command timeout is 200ms.
   // Flash commands can take longer
   int timeout;
   switch( cmd[0] >> 16 )
   {
      // Theoretical worst case erase time.
      // Normally takes about 15 seconds
      case EICAN_CMD_FIRMWARE:
         timeout = 200000; 
         break;

      // Writing to flash
      case EICAN_CMD_FWDATA:
      case EICAN_CMD_SETPARAM:
         timeout = 1000; 
         break;

      // Normal commands
      default:
         timeout = 200;
         break;
   }

   // Clear my command interrupt, and set one on the micro.
   // This passes control of the command area to the card.
   writel( EICAN_INT_CMD, &intCtl->flags);
   writel( EICAN_INT_CMD, &intCtl->remote);

   // Clear my command event
   EventClear( &pCard->cmdEvent );

   // Wait for the interrupt when the command finishes.
   IntMaskUpdt( pCard, EICAN_INT_CMD, 1 );

   ret = EventWait( &pCard->cmdEvent, &timeout );
   if( ret )
   {
      errmsg( "error waiting on card (%ld)\n", ret );
      goto done;
   }

   // Just return if no data was requested
   if( !rCt[0] ) goto done;

   // Find the number of response words to copy
   cmd[0] = readl( &cmdBuff[0] );

   uint32_t ct = 1 + (cmd[0] & 0x003F);

   if( ct > *rCt ) ct = *rCt;
   *rCt = ct;

   for( int i=1; i<ct; i++ )
      cmd[i] = readl( &cmdBuff[i] );

done:
   MutexPut( pCard->cmdMutex );
   return ret;
}

/**
 * The following code is different for 2.4.x and 2.6.x versions of the Linux 
 * kernel.
 */

#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0))

static void Save_PCI_Config( CardInfo *pCard )
{
   pci_save_state( pCard->os.dev );
}

static void Restore_PCI_Config( CardInfo *pCard )
{
   pci_restore_state( pCard->os.dev );
}

#else

// 2.4.x kernel support

static void Save_PCI_Config( CardInfo *pCard )
{
   pci_save_state( pCard->os.dev, pCard->os.pci_cfg );
}

static void Restore_PCI_Config( CardInfo *pCard )
{
   pci_restore_state( pCard->os.dev, pCard->os.pci_cfg );
}

#endif

/**
 * Reset the card.
 */
static int ResetCard( CardInfo *pCard, uint32_t *cmd )
{
   EICAN_BDM_REGS *reg = (EICAN_BDM_REGS *)(pCard->hwPtr + EICAN_BDM_BASE);
   EVENT_TYPE event;
   int to;

   // Disable all ints from this card
   CardShutDown( pCard );

   // Save PCI configuration
   Save_PCI_Config( pCard );

   // Init an event which will be used to delay
   EventInit( &event, 0 );

   // Reset the card
   writel( 4, &reg->ctrl );

   // delay for 1 ms
   to = 1;
   EventWait( &event, &to );

   // Take the card out of reset
   writel( 0, &reg->ctrl );

   // delay for 200ms to give the card time to boot
   to = 200;
   EventWait( &event, &to );

   // Restore the configuration space
   Restore_PCI_Config( pCard );

   // Initialize some pointers which could have changed if the 
   // revision of firmware changed
   InitCardPointers( pCard );

   // Restore basic card ints
   CardEnable( pCard );

   cmd[0] = 0;
   return 0;
}

