
#include "MsgQueue.h"
#include "os/support.h"
#include "os/debug.h"

// My default number of messages is the number that will 
// fit in 0x20000 (128k) bytes.  This should be around 5,550 
// messages which gives me around 330ms at the fastest CAN rate.
// I picked 0x20000 bytes because this should make good use
// of paged memory (pages are either 0x1000 or 0x2000 bytes).
#define DEFAULT_MEM_SIZE    0x20000

/**
 * Allocate a new message queue structure of the specified size.
 * The queue is allocated in non-paged memory.
 *
 * @param ct The size (in messages) for the queue.  If 0 use 
 *           the default size.
 * @return A pointer to the allocated queue, or 0 if unable.
 */
MsgQueue *MsgQueueAlloc( uint32_t ct )
{
   MsgQueue *q;
   uint32_t size;

   if( !ct ) 
      ct = 1 + (DEFAULT_MEM_SIZE - sizeof(MsgQueue)) / sizeof(CANFW_CANMSG);

   size = ((ct-1) * sizeof(CANFW_CANMSG)) + sizeof(MsgQueue);

   q = (MsgQueue*)AllocBigBlock( size );
   if( !q ) return 0;

   memset( q, 0, sizeof(MsgQueue) );
   SpinLockInit( &q->lock );
   EventInit( &q->event, 0 );
   q->size = size;
   q->max  = ct;

   return q;
}

/**
 * Free a previously allocated message queue.  If the passed pointer
 * is null, then this just returns.
 *
 * @param q A pointer to the queue to free.
 */
void MsgQueueFree( MsgQueue *q )
{
   if( !q ) return;
   EventDestroy( &q->event, EICAN_ERR_PORT_CLOSED );
   FreeBigBlock( q, q->size );
}

/**
 * Add a message to the queue.  The message is copied into the next
 * free location in the queue.  The passed message isn't modified.
 * Returns 0 on success, or an error code.
 *
 * @param q A pointer to the message queue.
 * @param msg A pointer to the message to add.
 * @return non-zero on success, 0 if the queue is full
 */
int MsgQueuePut( MsgQueue *q, const CANFW_CANMSG *msg )
{
   SPINLOCK_STATE state;
   int ret = 1;
   uint32_t newHead;

   SpinLockGet( &q->lock, &state );
   newHead = q->head + 1;
   if( newHead == q->max )
      newHead = 0;

   if( newHead == q->tail )
   {
      ret = 0;
      q->lost++;
   }
   else
   {
      q->buff[ q->head ] = *msg;
      q->head = newHead;

      if( q->lost )
	 q->lost = 0;
   }
   SpinLockPut( &q->lock, &state );
   EventSignal( &q->event );
   return ret;
}

/**
 * Get the next message from the queue.
 * @param q A pointer to the message queue
 * @param msg A pointer to the message structure to copy 
 * the message into.
 * @return The number of messages remaining in the queue
 *         after removing this one, or -1 if the queue was empty.
 */
int  MsgQueueGet( MsgQueue *q, CANFW_CANMSG *msg )
{
   SPINLOCK_STATE state;
   int ct;

   SpinLockGet( &q->lock, &state );

   ct = q->head - q->tail;
   if( ct < 0 ) ct += q->max;

   if( ct > 0 )
   {
      *msg = q->buff[ q->tail ];
      q->tail++;
      if( q->tail == q->max)
	 q->tail = 0;

      if( ct > q->highwater )
      {
         debug( "New high water mark for queue: %d\n", ct );
         q->highwater = ct;
      }

      if( q->lost )
      {
         debug( "Lost %d messages\n", q->lost );
         q->lost = 0;
      }
   }

   SpinLockPut( &q->lock, &state );
   return ct-1;
}

int MsgQueueWait( MsgQueue *q, CANFW_CANMSG *msg, int timeout )
{
   int err, ct;

   while( 1 )
   {
      EventClear( &q->event );

      ct = MsgQueueGet( q, msg );
      if( ct >= 0 ) return 0;

      err = EventWait( &q->event, &timeout );
      if( err )
	 return err;
   }
}

/**
 * Flush the message queue.
 * @param q A pointer to the message queue
 */
void MsgQueueFlush( MsgQueue *q )
{
   SPINLOCK_STATE state;
   SpinLockGet( &q->lock, &state );
   q->head = 0;
   q->tail = 0;
   SpinLockPut( &q->lock, &state );
}

/**
 * Return the number of messages in the queue
 */
int  MsgQueueCount( MsgQueue *q )
{
   int ct;

   SPINLOCK_STATE state;
   SpinLockGet( &q->lock, &state );

   ct = q->head - q->tail;
   if( ct < 0 ) ct += q->max;
   SpinLockPut( &q->lock, &state );

   return ct;
}

