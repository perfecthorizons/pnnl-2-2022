
#ifndef _DEF_INC_MSGQUEUE
#define _DEF_INC_MSGQUEUE

#include "os/support.h"
#include "cancard_cmd_intf.h"

typedef struct
{
   EVENT_TYPE event;
   SPINLOCK_TYPE lock;
   uint32_t size;
   uint32_t max;
   uint32_t head;
   uint32_t tail;
   uint32_t lost;
   uint32_t highwater;

   CANFW_CANMSG buff[1];
} MsgQueue;

/* prototypes */
MsgQueue *MsgQueueAlloc( uint32_t size );
void MsgQueueFree( MsgQueue *q );
int  MsgQueuePut( MsgQueue *q, const CANFW_CANMSG *msg );
int  MsgQueueGet( MsgQueue *q, CANFW_CANMSG *msg );
int  MsgQueueWait( MsgQueue *q, CANFW_CANMSG *msg, int timeout );
void MsgQueueFlush( MsgQueue *q );
int  MsgQueueCount( MsgQueue *q );

#endif
