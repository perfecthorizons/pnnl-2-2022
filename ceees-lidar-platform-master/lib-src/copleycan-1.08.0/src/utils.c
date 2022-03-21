
#include "os/debug.h"
#include "utils.h"

// Only used for the PCI card rev1 interface
#define PORT0_RECV_BASE        0x1400
#define PORT0_XMIT_BASE        0x1600
#define PORT1_RECV_BASE        0x1800
#define PORT1_XMIT_BASE        0x1A00

/**
 * Read a location on the cards dual ported RAM multiple times until
 * I see the same reading twice in a row.  This should get around problems
 * caused by the card writing to the buffer head/tail pointers at the same
 * time that I read.  In this case the dual port RAM can give me bad data.
 *
 * Note that this is really not necessary anymore with the PCIe cards, just
 * for backward compatibility with the older PCI cards.
 */
uint32_t ReadSafe( PortInfo *pPort, uint32_t *addr )
{
   uint32_t a, b;
   int i;

   a = readl(addr);

   if( !pPort->pCard->useSafeReads )
      return a;

   for( i=0; i<10; i++ )
   {
      b = readl(addr);
      if( a==b ) return a;
      a = b;
   }
   return b;
}

// Read the head, tail & size info from the receive queue 
void ReadRxqHead( PortInfo *pPort, uint32_t *head, uint32_t *tail, uint32_t *size )
{
   if( pPort->pCard->useSafeReads )
   {
      *head = 0x00FF & ReadSafe( pPort, &pPort->recv.ctrl->head );
      *tail = 0x00FF & readl( &pPort->recv.ctrl->tail );
      *size = 0x00FF & readl( &pPort->recv.ctrl->size );
   }
   else
   {
      EICAN_QUEUE_CTRL qctrl;
      memcpy_fromio( &qctrl, pPort->recv.ctrl, 3*sizeof(uint32_t) );
      *size = qctrl.size;
      *tail = qctrl.tail;
      *head = qctrl.head;
   }
}

// Read the head, tail & size info from the transmit queue 
void ReadTxqHead( PortInfo *pPort, uint32_t *head, uint32_t *tail, uint32_t *size )
{
   if( pPort->pCard->useSafeReads )
   {
      *head = 0x00FF & readl( &pPort->send.ctrl->head );
      *tail = 0x00FF & ReadSafe( pPort, &pPort->send.ctrl->tail );
      *size = 0x00FF & readl( &pPort->send.ctrl->size );
   }
   else
   {
      EICAN_QUEUE_CTRL qctrl;
      memcpy_fromio( &qctrl, pPort->send.ctrl, 3*sizeof(uint32_t) );
      *size = qctrl.size;
      *tail = qctrl.tail;
      *head = qctrl.head;
   }
}

// Decode the count in the CAN message and return the number of bytes
int DecodeMsgLen( uint8_t ct )
{
   if( ct <= 8 ) return ct;
   switch( ct )
   {
      case  9: return 12;
      case 10: return 16;
      case 11: return 20;
      case 12: return 24;
      case 13: return 32;
      case 14: return 48;
      default: return 64;
   }
}


/**************************************************************
 * Enable / Disable an interrupt.
 *************************************************************/
void IntMaskUpdt( CardInfo *pCard, uint32_t bits, int enable )
{
   EICAN_INTR_CTRL *intr = (EICAN_INTR_CTRL *)(pCard->hwPtr + EICAN_INTCTL_BASE);
   SPINLOCK_STATE state;

   SpinLockGet( &pCard->spin, &state );

   if( enable )
      pCard->intMask |= bits;
   else
      pCard->intMask &= ~bits;

   writel( pCard->intMask, &intr->mask );

   SpinLockPut( &pCard->spin, &state );
}

/**
 * Get the revision code of the PCI interface for the card.
 * One compilication is that the very first rev did not map
 * the revision code.  I can detect this revision however 
 * because it had memory at locations 0x1C00-0x1FFF.
 */
uint32_t GetPciRev( CardInfo *pCard )
{
   uint32_t x, *ptr;
   EICAN_INTR_CTRL *intr;

   // First, check for revision 0
   // I happen to know that location 0x1FFC wasn't ever used.
   ptr = (uint32_t*)(pCard->hwPtr + 0x1FFC);
   writel( 0x12345678, ptr );
   x = readl( ptr );

   if( x == 0x12345678 )
      return 0;

   // OK, read and return the revision code
   intr = (EICAN_INTR_CTRL *)( pCard->hwPtr + EICAN_INTCTL_BASE );

   return readl(&intr->revision);
}

/**
 * Init the various pointers to locations within the card's memory 
 * space.  These pointer values depend on the card firmware rev and
 * can therefore change after a reset.
 *
 * This is called on startup, and also after a reset.
 *
 * @param pCard Card Info pointer.  This structure should already have
 *        the hwPtr member initialized.
 */
void InitCardPointers( CardInfo *pCard )
{
   uint32_t pciRev = GetPciRev( pCard );

   // These values are constant for all PCI revisions currently 
   // known about:
   pCard->port[0].send.mask = EICAN_INT_SEND0;
   pCard->port[0].recv.mask = EICAN_INT_RECV0;

   if( pCard->portCt > 1 )
   {
      pCard->port[1].send.mask = EICAN_INT_SEND1;
      pCard->port[1].recv.mask = EICAN_INT_RECV1;
   }

   switch( pciRev )
   {
      case 0:
      {
	 EICAN_PORT_RAM_INFO *ramInfo;

	 ramInfo = (EICAN_PORT_RAM_INFO *)(pCard->hwPtr + EICAN_PORT0_BASE);
	 pCard->port[0].send.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + EICAN_PORT0_BASE + readl( &ramInfo->xmitOff ) );
	 pCard->port[0].recv.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + EICAN_PORT0_BASE + readl( &ramInfo->recvOff ) );
	 pCard->timePtr = &ramInfo->time;

         if( pCard->portCt > 1 )
         {
            ramInfo = (EICAN_PORT_RAM_INFO *)(pCard->hwPtr + EICAN_PORT1_BASE);
            pCard->port[1].send.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + EICAN_PORT1_BASE + readl( &ramInfo->xmitOff ) );
            pCard->port[1].recv.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + EICAN_PORT1_BASE + readl( &ramInfo->recvOff ) );
         }
	 break;
      }

      case 1:
      default:
	 if( pciRev != 1 )
	    errmsg( "PCI Revision code (%d) isn't known by this driver.  You should update\n", pciRev );

	 pCard->port[0].send.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + PORT0_XMIT_BASE );
	 pCard->port[0].recv.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + PORT0_RECV_BASE );
	 pCard->timePtr = &pCard->port[0].recv.ctrl->time;

         if( pCard->portCt > 1 )
         {
            pCard->port[1].send.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + PORT1_XMIT_BASE );
            pCard->port[1].recv.ctrl = (EICAN_QUEUE_CTRL*)(pCard->hwPtr + PORT1_RECV_BASE );
         }
	 break;
   }
}
