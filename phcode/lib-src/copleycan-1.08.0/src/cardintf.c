/***************************************************
 * OS generic card interface to CAN card
 **************************************************/

#include "os/support.h"
#include "os/debug.h"
#include "cardintf.h"
#include "internal.h"
#include "recv.h"
#include "utils.h"
#include "xmitbuff.h"

// local functions 
static void UpdateTimeStamp( CardInfo *pCard );
static void QueueInit( QueueInfo *queue );

/**************************************************************
 * One time init of CardInfo structure 
 *************************************************************/
int CardInit( CardInfo *pCard, char *hwPtr, uint32_t hwSize )
{
   int i;
   for( i=0; i<pCard->portCt; i++ )
   {
      QueueInit( &pCard->port[i].send );
      QueueInit( &pCard->port[i].recv );
   }

   // Initialize the card structure
   SpinLockInit( &pCard->spin );
   pCard->hwPtr  = hwPtr;
   pCard->hwSize = hwSize;

   if( hwSize )
      InitCardPointers( pCard );
   XmitBuffInit( pCard );

   for( i=0; i<pCard->portCt; i++ )
   {
      pCard->port[i].pCard = pCard;
      pCard->port[i].mask  = i * 0x100;
      pCard->port[i].recvHighWater = 0;
      AtomicInit( &pCard->port[i].openCt,     0 );
      AtomicInit( &pCard->port[i].xmitAckCt,  0 );
      AtomicInit( &pCard->port[i].nextXmitID, 0 );
   }

   EventInit( &pCard->cmdEvent, 0 );
   MutexInit( pCard->cmdMutex );

   return 0;
}

/**
 * Enable interrupts from this card.
 */
void CardEnable( CardInfo *pCard )
{
   uint32_t mask = EICAN_INT_TIME;
   int i;
   for( i=0; i<pCard->portCt; i++ )
      mask |= pCard->port[i].recv.mask;

   IntMaskUpdt( pCard, mask, 1 );
}

void CardShutDown( CardInfo *pCard )
{
   IntMaskUpdt( pCard, -1, 0 );
}

/**************************************************************
 * Top half of the ISR.  
 *
 * This function clears the interrupt mask on the card to 
 * stop it from generating it's interrupt.
 *
 * @return 1 if the card generated the interrupt, else 0.
 *************************************************************/
int EICAN_ISR_Top( CardInfo *pCard )
{
   EICAN_INTR_CTRL *intr = (EICAN_INTR_CTRL *)(pCard->hwPtr + EICAN_INTCTL_BASE);

   // Read the mask and flag values from the card
   uint32_t flag = readl(&intr->flags);
   uint32_t mask = readl(&intr->mask);

   // Return false if the card is giving bad data.
   // This is a test to try to fix a lockup during reset.
   if( flag == 0xFFFFFFFF )
      return 0;

   // Return false if I didn't generate the int
   if( !(mask & flag) )
      return 0;

   // Disable all ints
   writel( 0, &intr->mask );

   return 1;
}

/**************************************************************
 * Bottom half of the ISR
 *
 * This function does the actual work of signaling events based
 * on any active interrupt conditions on the card.
 *************************************************************/
void EICAN_ISR_Bottom( CardInfo *pCard )
{
   EICAN_INTR_CTRL *intr = (EICAN_INTR_CTRL *)(pCard->hwPtr + EICAN_INTCTL_BASE);
   SPINLOCK_STATE state;
   uint32_t flag;
   int i;

   /*
    * At this point the hardware interrupt mask will have been 
    * cleared.  I'll see what interrupts are actually active now
    * and remove them from the local mask.  I'll then update the
    * hardware mask to enable any desired interrupts that aren't
    * currently being serviced.
    */
   SpinLockGet( &pCard->spin, &state );
   flag	= readl(&intr->flags);
   flag &= pCard->intMask;
   pCard->intMask &= ~flag;
   writel( pCard->intMask, &intr->mask );
   SpinLockPut( &pCard->spin, &state );

   /* Service any active interrupts. */
   if( flag & EICAN_INT_CMD   ) EventSignal( &pCard->cmdEvent );
   if( flag & EICAN_INT_TIME  ) UpdateTimeStamp( pCard );

   for( i=0; i<pCard->portCt; i++ )
   {
      if( flag & pCard->port[i].send.mask ) EventSignal( &pCard->port[i].send.event );
      if( flag & pCard->port[i].recv.mask ) RecvInt( &pCard->port[i] );
   }
}

static void UpdateTimeStamp( CardInfo *pCard )
{
   EICAN_INTR_CTRL *intr = (EICAN_INTR_CTRL *)(pCard->hwPtr + EICAN_INTCTL_BASE);
   SPINLOCK_STATE state;
   uint32_t *tmr = (uint32_t *)(pCard->hwPtr + EICAN_TIMER_BASE);
   int i;

   // Update the timestamp.  The timestamp value is stored in the queue control
   // area of port 0 receive queue.
   uint32_t stamp = GetTimeStamp();
   writel( stamp, pCard->timePtr );

   // Clear the interrupt
   writel( 0, tmr );

   for( i=0; i<3; i++ )
   {
      writel( EICAN_INT_TIME, &intr->flags );
      if( (readl(&intr->flags) & EICAN_INT_TIME) == 0 )
	 break;
   }

   if( i==3 )
   {
      errmsg( "Time stamp interrupt stuck on.  Masking\n" );
      return;
   }

   // Enable the next interrupt
   SpinLockGet( &pCard->spin, &state );
   pCard->intMask |= EICAN_INT_TIME;
   writel( pCard->intMask, &intr->mask );
   SpinLockPut( &pCard->spin, &state );
}

/**************************************************************
 * One time initialization of a message Queue.
 *************************************************************/
static void QueueInit( QueueInfo *queue )
{
   EventInit( &queue->event, 0 );
   MutexInit( queue->mutex );
}


