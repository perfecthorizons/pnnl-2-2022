/*********************************************************************
 * Linux specific header included at the top of all OS generic files
 ********************************************************************/

#ifndef _DEF_INC_OSSUPPORT
#define _DEF_INC_OSSUPPORT

#include <linux/pci.h>
#include <linux/types.h>
#include <asm/errno.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/usb.h>
#include <linux/version.h>

#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,27))
#include <linux/semaphore.h>
#endif

#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0))
#include <linux/cdev.h>
#endif

#include "../cancard_hw_intf.h"
#include "../cancard_cmd_intf.h"

/***************************************************
 * Spin lock support.  
 **************************************************/
typedef spinlock_t           SPINLOCK_TYPE;
typedef unsigned long        SPINLOCK_STATE;
static inline void SpinLockInit( SPINLOCK_TYPE *l ){ spin_lock_init( l ); }
static inline void SpinLockGet ( SPINLOCK_TYPE *l, SPINLOCK_STATE *s ){ spin_lock_irqsave( l, *s ); }
static inline void SpinLockPut ( SPINLOCK_TYPE *l, SPINLOCK_STATE *s ){ spin_unlock_irqrestore( l, *s ); }

/***************************************************
 * Atomic integer operations.
 * Note that the 2.4.x kernel doesn't have some of 
 * the operations that I need, so I have to build
 * my own using a spinlock.
 **************************************************/
#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0))
// Atomic integer type
typedef atomic_t             ATOMIC_INT;
static inline void AtomicInit(ATOMIC_INT *i, int v ){ atomic_set(i,v); }                 // Initialize the atomic value
static inline int  AtomicInc(ATOMIC_INT *i)        { return atomic_inc_return(i); }      // Increment and return the NEW value
static inline int  AtomicDec(ATOMIC_INT *i)        { return atomic_dec_return(i); }      // Decrement and return the NEW value
static inline int  AtomicGet(ATOMIC_INT *i)        { return atomic_read(i); }            // Return the current value
static inline void AtomicSet(ATOMIC_INT *i, int v) { atomic_set(i,v); }                  // Set the value
#else
typedef struct
{
   SPINLOCK_TYPE lock;
   int value;
}ATOMIC_INT;

// Init the atomic value
static inline void AtomicInit(ATOMIC_INT *i, int v )
{
   i->value = v;
   SpinLockInit( &i->lock );
}

// Increment and return the NEW value
static inline int  AtomicInc(ATOMIC_INT *i)
{
   SPINLOCK_STATE state;
   int v;
   SpinLockGet( &i->lock, &state );
   v = ++(i->value);
   SpinLockPut( &i->lock, &state );
   return v;
}

// Decrement and return the NEW value
static inline int AtomicDec(ATOMIC_INT *i)
{
   SPINLOCK_STATE state;
   int v;
   SpinLockGet( &i->lock, &state );
   v = --(i->value);
   SpinLockPut( &i->lock, &state );
   return v;
}

// Return the current value
static inline int  AtomicGet(ATOMIC_INT *i)
{
   SPINLOCK_STATE state;
   int v;
   SpinLockGet( &i->lock, &state );
   v = i->value;
   SpinLockPut( &i->lock, &state );
   return v;
}

// Set the value
static inline void AtomicSet(ATOMIC_INT *i, int v)
{
   SPINLOCK_STATE state;
   SpinLockGet( &i->lock, &state );
   i->value = v;
   SpinLockPut( &i->lock, &state );
}

#endif

// Get timestamp in microseconds
static inline uint32_t GetTimeStamp( void )
{
#if( LINUX_VERSION_CODE > KERNEL_VERSION(5,0,0))
   return (ktime_get_ns() / 1000);
#else
   struct timeval tv;
   do_gettimeofday(&tv);
   return (tv.tv_sec * 1000000) + tv.tv_usec;
#endif
}

// Memory allocation / free
static inline void *AllocUnpaged( size_t size )
{
   return kmalloc( size, GFP_KERNEL );
}

static inline void FreeUnpaged( void *ptr )
{
   kfree( ptr );
}

static inline void *AllocBigBlock( size_t size )
{
   return (void*)__get_free_pages( GFP_KERNEL, get_order(size) );
}

static inline void FreeBigBlock( void *ptr, size_t size )
{
   free_pages( (unsigned long)ptr, get_order(size) );
}

/***************************************************************
 * Event.  An event is either signaled or not.  When a thread 
 * tries to wait on an event, it will become ready to run 
 * immediately if the event is signaled.  If not it will wait.
 * When an event is signaled, all threads that were waiting on 
 * the event become ready to run.
 **************************************************************/
typedef struct
{
   int err;
   atomic_t state;
   atomic_t count;
   wait_queue_head_t wait;
} EVENT_TYPE;

static inline void EventInit( EVENT_TYPE *e, int val )
{
   e->err = 0;
   atomic_set(&e->state, val);
   atomic_set(&e->count, 0);
   init_waitqueue_head(&e->wait);
}

static inline int EventCheck( EVENT_TYPE *e )
{
   return atomic_read( &e->state );
}

static inline void EventSignal( EVENT_TYPE *e )
{
   atomic_set( &e->state, 1 );
   wake_up_interruptible( &e->wait );
   return;
}

static inline void EventClear( EVENT_TYPE *e )
{
   atomic_set( &e->state, 0 );
}

// Destroy the event with the passed error code.
static inline void EventDestroy( EVENT_TYPE *e, int err )
{
   unsigned long timeout;
   e->err = err;
   EventSignal( e );

   // Wait up to 100ms for other threads to start running
   timeout = jiffies + HZ/10;
   while( atomic_read(&e->count) && (jiffies < timeout) )
      schedule();
}

// The 2.4.x kernel didn't have the macro wait_event_interruptible_timeout.
// I'm adding a version here.  This is pulled from the 2.6 source
#if( LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
#define __wait_event_interruptible_timeout(wq, condition, ret)		\
do {									\
   wait_queue_t __wait;						        \
   init_waitqueue_entry(&__wait, current);				\
									\
   add_wait_queue(&wq, &__wait);					\
   for (;;) {							        \
      set_current_state(TASK_INTERRUPTIBLE);			        \
      if (condition)						        \
	 break;						                \
      if (!signal_pending(current)) {				        \
	 ret = schedule_timeout( ret );					\
	 if( !ret ) break;                                              \
	 continue;					                \
      }							                \
      ret = -ERESTARTSYS;					        \
      break;							        \
   }								        \
   current->state = TASK_RUNNING;					\
   remove_wait_queue(&wq, &__wait);				        \
} while (0)

#define wait_event_interruptible_timeout(wq, condition, timeout)	\
({									\
   long __ret = timeout;						\
   if (!(condition))						        \
      __wait_event_interruptible_timeout(wq, condition, __ret);         \
   __ret;								\
})

#endif

static inline int EventWait( EVENT_TYPE *e, int *timeout )
{
   int ret = 0;

   if( e->err )
      return e->err;

   if( atomic_read( &e->state ) )
      return 0;

   if( !*timeout )
      return EICAN_ERR_TIMEOUT;

   atomic_inc( &e->count );
   if( *timeout < 0 )
   {
      if( wait_event_interruptible( e->wait, ((e->err) || atomic_read(&e->state)) ) )
	 ret = EICAN_ERR_SIGNAL;
   }

   else
   {
      // Convert the timeout from milliseconds to jiffies
      int err, to = (*timeout) * HZ / 1000;
      if( to < 1 ) to = 1;

      err = wait_event_interruptible_timeout( e->wait, ((e->err) || atomic_read(&e->state)), to );

      if( e->err )
	 ret = e->err;
      else if( err < 0 )
	 ret = EICAN_ERR_SIGNAL;
      else if( err == 0 )
	 ret = EICAN_ERR_TIMEOUT;
      else
	 *timeout = err;
   }
   atomic_dec( &e->count );

   return ret;
}

// Mutex 
#define MUTEX_TYPE           struct semaphore
#define MutexInit(x)         sema_init( &x, 1 )
#define MutexPut(x)          up(&x)
#define MutexGet(x,to,ret)           \
   do{                               \
      if( down_interruptible(&x) )   \
	 ret = EICAN_ERR_SIGNAL;     \
      else                           \
	 ret = 0;                    \
   }                                 \
   while(0)

/**
 * The following structures hold additional opperating system 
 * specific information that's added to our internal data structures.
 * These structures are different for the 2.4.x and 2.6.x kernel
 * branches.
 */
#if( LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0))

// Additional info for the CardData structure
typedef struct 
{
   struct pci_dev *dev;
   struct usb_interface *usbintf;
} OS_CardData;

// Additional info for the PortData structure
typedef struct
{
   struct cdev cdev;
   dev_t num;
} OS_PortData;

#else

// Additional info for the CardData structure
typedef struct 
{
   struct pci_dev *dev;
   uint32_t pci_cfg[16];
} OS_CardData;

// Additional info for the PortData structure
typedef struct
{
   dev_t num;
} OS_PortData;

#endif



#endif

