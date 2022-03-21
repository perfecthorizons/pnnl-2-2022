
#include "test.h"
#include "os/debug.h"

/**************************************************************
 * Run a quick test on the PCI interface.  The test consists of 
 * a short memory test on the dual port RAM block and an interrupt
 * test.
 *
 * I only test the command area of the memory since I'm not 
 * really worried about testing the memory itself, just the 
 * PCI interface.
 *************************************************************/
int TestCard( CardInfo *card )
{
   EICAN_INTR_CTRL *intCtl = (EICAN_INTR_CTRL *)(card->hwPtr + EICAN_INTCTL_BASE);
   uint32_t *cmdBuff = (uint32_t *)(card->hwPtr + EICAN_CMD_BASE);
   uint32_t flags;
   uint32_t i;
   long ret;
   uint32_t mask;

   MutexGet( card->cmdMutex, 1000, ret );
   if( ret )
   {
      errmsg( "Error %ld waiting on command mutex\n", ret );
      return EICAN_ERR_CMDMUTEX_HELD;
   }

   // Check the command interrupt flag.  This needs to be set for me
   // to access the command area
   flags = readl(&intCtl->flags);
   if( (flags & EICAN_INT_CMD) == 0 )
   {
      errmsg( "Card busy error.\n"
	     " Flags:  0x%08x\n"
	     " Mask:   0x%08x\n"
	     " Remote: 0x%08x\n", readl(&intCtl->flags), readl( &intCtl->mask ), readl( &intCtl->remote ) );

      ret = EICAN_ERR_CARD_BUSY;
      goto done;
   }

   // Do the memory test
   for( i=0, mask=1; i<32; i++, mask<<=1 )
   {
      uint32_t x;

      writel( mask, &cmdBuff[0] );
      writel( 0x55555555, &cmdBuff[1] );
      writel( 0xAAAAAAAA, &cmdBuff[1] );

      x = readl( &cmdBuff[0] );
      if( x != mask )
      {
	 errmsg( "Single bit memory test error.  Wrote 0x%08x, read 0x%08x\n", mask, x );
	 ret = EICAN_ERR_MEMORYTEST;
	 goto done;
      }
   }
   
   for( i=0; i<EICAN_CMD_LEN; i++ )
      writel( 0x01010101*i, &cmdBuff[i] );

   for( i=0; i<EICAN_CMD_LEN; i++ )
   {
      uint32_t x = readl( &cmdBuff[i] );
      if( x != 0x01010101*i )
      {
	 errmsg( "Memory read/write failure.  Wrote 0x%08x, read 0x%08x\n", 0x01010101*i, x );
	 ret = EICAN_ERR_MEMORYTEST;
	 goto done;
      }
   }

   ret = 0;
done:
   MutexPut( card->cmdMutex );

   return ret;
}


