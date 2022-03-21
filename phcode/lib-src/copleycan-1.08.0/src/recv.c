/* recv.c */

#include "recv.h"
#include "utils.h"
#include "xmitbuff.h"
#include "os/debug.h"

// local functions
static void SplitBytes( uint32_t L, uint8_t data[], int bigEndian );

/**************************************************************
 * Receive a CAN message
 *************************************************************/
int RecvCAN( FileInfo *pFile, CANCARD_MSG *can )
{
   CardInfo *pCard = pFile->pPort->pCard;
   CANFW_CANMSG msg;
   int err;

   err = MsgQueueWait( pFile->recvMsgQueue, &msg, can->timeout );
   if( err ) return err;

   can->timestamp = msg.time;
   can->flags     = msg.flags;
   can->id        = msg.msgID;

   for( int i=0; i<2; i++ )
      SplitBytes( msg.data[i], &can->data[4*i], pCard->bigEndianData );
   return 0;
}

/**
 * This function is called from the interrupt handler when 
 * a new message is added one of the receive queues.
 */
void RecvInt( PortInfo *pPort )
{
   CardInfo *pCard = pPort->pCard;
   EICAN_INTR_CTRL *intr = (EICAN_INTR_CTRL *)(pCard->hwPtr + EICAN_INTCTL_BASE);
   uint32_t head, tail, size, *pos;
   SPINLOCK_STATE state;
   CANFW_CANMSG msg;
   int i, count;

   // Clear any pending receive interrupts.
   writel( pPort->recv.mask, &intr->flags );

   // Grab the spinlock.
   SpinLockGet( &pCard->spin, &state );

   // Read the queue info
   ReadRxqHead( pPort, &head, &tail, &size );

   // Do some quick sanity checks here
   if( (head>=size) || (tail>=size) )
   {
      SpinLockPut( &pCard->spin, &state );
      errmsg( "Read invalid command queue: 0x%08x, tail: 0x%08x, size: 0x%08x\n", head, tail, size );
      return;
   }

   count = head-tail;
   if( count < 0 ) count += size;
   if( count > pPort->recvHighWater )
   {
      pPort->recvHighWater = count;
      debug( "Receive high water mark %d/%d\n", count, size );
   }

   pos = &(pPort->recv.ctrl->data[tail * EICAN_QUEUE_ENTRY_LEN]);

   // Read all available data from the card's queue 
   // and add it to the file receive queues.
   while( tail != head )
   {
      // Read the CAN message out of the card.
      // Note that the old PCI card doesn't work with a simple memcpy
      if( pCard->useSafeReads )
      {
         uint32_t *msgPtr = (uint32_t*)&msg;
         for( i=0; i<EICAN_QUEUE_ENTRY_LEN; i++ )
            msgPtr[i] = readl(pos++);
      }
      else
      {
         memcpy_fromio( &msg, pos, EICAN_QUEUE_ENTRY_LEN*4 );
         pos += EICAN_QUEUE_ENTRY_LEN;
      }

      // Update the queue tail pointer
      if( ++tail >= size )
      {
	 tail = 0;
	 pos = &(pPort->recv.ctrl->data[0]);
      }

      // If this was a send notify, then only the timestamp and flags
      // are valid.  I have a local copy of the sent message.
      if( msg.flags & EICAN_CANFLG_SENDNOTIFY )
      {
	 if( !XmitBuffGet( pPort, &msg ) )
	    continue;
      }

      // Add this message to the receive queue of each open file.
      for( i=0; i<MAX_FILES; i++ )
      {
	 FileInfo *pFile = pPort->files[i];
	 if( !pFile ) break;

	 if( (msg.flags & EICAN_CANFLG_SENDNOTIFY) && !pFile->needXmit )
	    continue;

	 MsgQueuePut( pFile->recvMsgQueue, &msg );
      }
   }

   writel( tail, &pPort->recv.ctrl->tail );

   SpinLockPut( &pCard->spin, &state );

   // Enable receive interrupts
   IntMaskUpdt( pCard, pPort->recv.mask, 1 );

   return;
}

/**************************************************************
 * Split a 32-bit integer into 4 bytes.
 *************************************************************/
static void SplitBytes( uint32_t L, uint8_t data[], int bigEndian )
{
   if( bigEndian )
   {
      data[0] = (uint8_t)(L >> 24);
      data[1] = (uint8_t)(L >> 16);
      data[2] = (uint8_t)(L >> 8);
      data[3] = (uint8_t)(L);
   }
   else
   {
      data[0] = (uint8_t)(L);
      data[1] = (uint8_t)(L >> 8);
      data[2] = (uint8_t)(L >> 16);
      data[3] = (uint8_t)(L >> 24);
   }
}

/**************************************************************
 * Flush the port's hardware receive queue
 *************************************************************/
void FlushRecvQueue( PortInfo *pPort )
{
   CardInfo *pCard = pPort->pCard;

   // Don't do this for the USB card.
   if( !pCard->hwPtr ) 
      return;

   SPINLOCK_STATE state;
   uint32_t head;

   // Grab the spinlock.
   SpinLockGet( &pCard->spin, &state );
   head = 0x00FF & ReadSafe( pPort, &pPort->recv.ctrl->head );
   writel( head, &pPort->recv.ctrl->tail );
   SpinLockPut( &pCard->spin, &state );

   // Enable receive interrupts
   IntMaskUpdt( pCard, pPort->recv.mask, 1 );

   return;
}
