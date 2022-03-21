
#include "internal.h"

/**
 * One time init of transmit holding buffer
 */
void XmitBuffInit( CardInfo *pCard )
{
   int i;

   for( i=0; i<pCard->portCt; i++ )
   {
      pCard->port[i].xmitBuffHead = 0;
      pCard->port[i].xmitBuffTail = 0;
      SpinLockInit( &pCard->port[i].xmitBuffLock );
   }
}

/**
 * Add the passed message to my transmit holding buffer.
 */
void XmitBuffAdd( PortInfo *pPort, CANFW_CANMSG *msg )
{
   SPINLOCK_STATE state;
   int head, tail;

   SpinLockGet( &pPort->xmitBuffLock, &state );

   head = pPort->xmitBuffHead;
   tail = pPort->xmitBuffTail;

   pPort->xmitBuff[ head ] = *msg;
   if( ++head == MAX_XMIT_HOLD )
      head = 0;

   if( head == tail )
   {
      if( ++tail == MAX_XMIT_HOLD )
	 tail = 0;
   }

   pPort->xmitBuffHead = head;
   pPort->xmitBuffTail = tail;
   SpinLockPut( &pPort->xmitBuffLock, &state );
}

int XmitBuffGet( PortInfo *pPort, CANFW_CANMSG *msg )
{
   SPINLOCK_STATE state;
   uint32_t x = 0;
   uint32_t id = (msg->flags & 0xFFFF0000);
   int head, tail;
   int found = 0;

   SpinLockGet( &pPort->xmitBuffLock, &state );

   head = pPort->xmitBuffHead;
   tail = pPort->xmitBuffTail;

   while( tail != head )
   {
      x = (pPort->xmitBuff[tail].flags & 0xFFFF0000);

      if( id <= x )
	 break;

      if( ++tail == MAX_XMIT_HOLD ) tail = 0;
   }

   if( id == x )
   {
      pPort->xmitBuff[tail].time = msg->time;
      *msg = pPort->xmitBuff[tail];
      if( ++tail == MAX_XMIT_HOLD ) tail = 0;
      found = 1;
   }

   pPort->xmitBuffTail = tail;

   SpinLockPut( &pPort->xmitBuffLock, &state );

   msg->flags |= EICAN_CANFLG_SENDNOTIFY;
   return found;
}

