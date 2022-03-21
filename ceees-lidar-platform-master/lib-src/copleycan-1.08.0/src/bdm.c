/***************************************************
 * BDM hardware interface
 **************************************************/

#include "os/support.h"
#include "os/debug.h"
#include "cardintf.h"
#include "internal.h"
#include "bdm.h"

#define PCICLK_TO_TC(x,hc)     ((x)*33333/hc+1)

static int16_t CalcBRR( int t1, int t2, int t3, int hc )
{
   int16_t brr;

//   debug( "Calculating bit rates: %d, %d, %d, %d\n", t1, t2, t3, hc );
   t1 = PCICLK_TO_TC(t1,hc);
   t2 = PCICLK_TO_TC(t2,hc);
   t3 = PCICLK_TO_TC(t3,hc);

//   debug( "  Results: %d %d %d\n", t1, t2, t3 );
   t3 -= t2+1;
   t2 -= t1+1;
   t1 -= 1;
//   debug( "  Results: %d %d %d\n", t1, t2, t3 );

   if( t1 > 0x001F ) t1 = 0x001F;
   if( t2 > 0x003F ) t2 = 0x003F;
   if( t3 > 0x001F ) t3 = 0x001F;
//   debug( "  Results: %d %d %d\n", t1, t2, t3 );

   brr = t1;
   brr |= t2<<5;
   brr |= t3<<11;
   return brr;
}

static int BDM_AddSeq( CardInfo *pCard, int16_t data, int16_t to, int flags )
{
   int ndx;

   if( pCard->bdm.seqNdx >= MAX_BDM_SEQ )
   {
      errmsg( "Unable to add BDM write to sequence, too many entries\n" );
      return -1;
   }

   if( pCard->bdm.hc < 1 )
   {
      errmsg( "Unable to add BDM command.  Host clock not set (%d)\n", pCard->bdm.hc );
      return -1;
   }

   // Single byte writes are passed in the upper bits
   if( flags & BDM_FLG_BYTE )
      data <<= 8;

   ndx = pCard->bdm.seqCt++;

   pCard->bdm.seq[ ndx ].flags = flags;
   pCard->bdm.seq[ ndx ].data  = data;
   pCard->bdm.seq[ ndx ].to    = PCICLK_TO_TC( to, pCard->bdm.hc );
   return 0;
}

static void BDM_Program( CardInfo *pCard )
{
   EICAN_BDM_REGS *reg = (EICAN_BDM_REGS *)(pCard->hwPtr + EICAN_BDM_BASE);
   int ndx, T, flags;

   ndx = pCard->bdm.seqNdx++;
   T = pCard->bdm.target;

   writel( pCard->bdm.seq[ ndx ].to, &reg->to );
   writel( pCard->bdm.seq[ ndx ].data, &reg->data );

   flags = pCard->bdm.seq[ ndx ].flags;
   if( flags & BDM_FLG_WRITE )
   {
      writel( pCard->bdm.brrWrite, &reg->brr );
      if( flags & BDM_FLG_BYTE )
	 writel( T | 0x0a, &reg->ctrl );
      else
	 writel( T | 0x0b, &reg->ctrl );
   }
   else
   {
      writel( pCard->bdm.brrRead, &reg->brr );
      writel( T | 0x09, &reg->ctrl );
   }
}

static int BDM_Execute( CardInfo *pCard, EICAN_BDM *bdm )
{
   EICAN_BDM_REGS *reg = (EICAN_BDM_REGS *)(pCard->hwPtr + EICAN_BDM_BASE);
   unsigned long mask;
   int i;

   if( pCard->bdm.seqCt < 1 )
   {
      debug( "Warning, BDM_Execute called with count of %d\n", pCard->bdm.seqCt );
      return 0;
   }

   pCard->bdm.seqNdx = 0;

   // I've measured the actual number of loops required and even running
   // with a 1MHz host clock I can handle a hardware read (longest command)
   // in about 350 loops.  I'll therefore quite after 10 times that which 
   // should really handle any reasonable situation.
   local_irq_save(mask);
   for( i=0; i<3500; i++ )
   {
      // Wait for the BDM module to finish the current transfer.
      if( readl( &reg->ctrl ) & 0x08 )
	 continue;

      // If we have already sent the last sequence, then we're done
      if( pCard->bdm.seqNdx >= pCard->bdm.seqCt )
	 break;

      // Program the next sequence
      BDM_Program( pCard );
   }
   local_irq_restore(mask);

   // Keep track of how many loops I executed
   pCard->bdm.loops = i;

   if( i == 3500 )
      debug( "BDM timed out!\n" );

   bdm->data = readl( &reg->data );

   return 0;
}


#if 0
static int BDM_Execute( CardInfo *pCard, EICAN_BDM *bdm )
{
   EICAN_BDM_REGS *reg = (EICAN_BDM_REGS *)(pCard->hwPtr + EICAN_BDM_BASE);

   if( pCard->bdm.seqCt < 1 )
   {
      debug( "Warning, BDM_Execute called with count of %d\n", pCard->bdm.seqCt );
      return 0;
   }

   pCard->bdm.seqNdx = 0;
//   IntMaskUpdt( pCard, EICAN_INT_BDM, 1 );
//
//   // Wait for the command to finish
   int ret = 0;
//   EventWait( pCard->bdm.event, (pCard->bdm.seqNdx >= pCard->bdm.seqCt), 10000, ret );

BDMinterrupt( pCard );

   bdm->data = readl( &reg->data );

   return ret;
}

/**
 * BDM Interrupt handler.  This uses a busy waiting loop to transfer 
 * all the data to the BDM.  I need to do this because the BDM has 
 * very tight timing requirements that I can't meet unless I spin here
 * with ints disabled.
 *
 * The max amount of time that a BDM transfer should take would be 
 * about 950 host clock cycles.  That's about 120 microseconds at 
 * 8 MHz host clock rate.
 */
void BDMinterrupt( CardInfo *pCard )
{
   EICAN_BDM_REGS *reg = (EICAN_BDM_REGS *)(pCard->hwPtr + EICAN_BDM_BASE);
   int i;
   unsigned long mask;

   // I've measured the actual number of loops required and even running
   // with a 1MHz host clock I can handle a hardware read (longest command)
   // in about 350 loops.  I'll therefore quite after 10 times that which 
   // should really handle any reasonable situation.
   local_irq_save(mask);
   for( i=0; i<3500; i++ )
   {
      // Wait for the BDM module to finish the current transfer.
      if( readl( &reg->ctrl ) & 0x08 )
	 continue;

      // If we have already sent the last sequence, then we're done
      if( pCard->bdm.seqNdx >= pCard->bdm.seqCt )
	 break;

      // Program the next sequence
      BDM_Program( pCard );
   }
   local_irq_restore(mask);

   // Keep track of how many loops I executed
   pCard->bdm.loops = i;
//   EventSignal( pCard->bdm.event );
}
#endif

/**
 * Handle background debug commands.
 */
int HandleBDM( CardInfo *pCard, EICAN_BDM *bdm )
{
   int err = 0;
   EICAN_BDM_REGS *reg = (EICAN_BDM_REGS *)(pCard->hwPtr + EICAN_BDM_BASE);

   // Default the host clock to 12,500,000
   if( pCard->bdm.hc < 1 )
   {
      pCard->bdm.brrRead  = CalcBRR( 3, 10, 20, 12500 );
      pCard->bdm.brrWrite = CalcBRR( 3, 13, 20, 12500 );
      pCard->bdm.hc = 12500;
      pCard->bdm.target = 0x20;
   }

   pCard->bdm.seqCt  = 0;

   switch( bdm->op )
   {
      case EICAN_BDM_SET_HC:
	 debug( "BDM clock rate set to %d\n", bdm->data );
	 pCard->bdm.brrRead  = CalcBRR( 3, 10, 20, bdm->data );
	 pCard->bdm.brrWrite = CalcBRR( 3, 13, 20, bdm->data );
	 pCard->bdm.hc = bdm->data;
	 return 0;

      case EICAN_BDM_SET_CTRL:
	 writel( bdm->data, &reg->ctrl );
	 pCard->bdm.target = bdm->data & 0x20;
	 return 0;

      case EICAN_BDM_GET_STATUS:
	 bdm->data = readl( &reg->ctrl );
	 return 0;

      case EICAN_BDM_SET_RATE:
	 writel( bdm->data, &reg->brr);
	 return 0;

      case EICAN_BDM_GET_RATE:
	 bdm->data = readl( &reg->brr );
	 return 0;

      case EICAN_BDM_HW_READ:
	 if( !err ) err = BDM_AddSeq( pCard, bdm->cmd,    0, BDM_FLG_WRITE|BDM_FLG_BYTE );
	 if( !err ) err = BDM_AddSeq( pCard, bdm->addr, 150, BDM_FLG_WRITE|BDM_FLG_WORD );
	 if( !err ) err = BDM_AddSeq( pCard,         0,   0, BDM_FLG_READ |BDM_FLG_WORD );
	 if( !err ) err = BDM_Execute( pCard, bdm );
	 return err;

      case EICAN_BDM_HW_WRITE:
	 if( !err ) err = BDM_AddSeq( pCard, bdm->cmd,    0, BDM_FLG_WRITE|BDM_FLG_BYTE );
	 if( !err ) err = BDM_AddSeq( pCard, bdm->addr,   0, BDM_FLG_WRITE|BDM_FLG_WORD );
	 if( !err ) err = BDM_AddSeq( pCard, bdm->data, 150, BDM_FLG_WRITE|BDM_FLG_WORD );
	 if( !err ) err = BDM_Execute( pCard, bdm );
	 return err;

      case EICAN_BDM_FW_READ:
	 if( !err ) err = BDM_AddSeq( pCard, bdm->cmd, 64, BDM_FLG_WRITE|BDM_FLG_BYTE );
	 if( !err ) err = BDM_AddSeq( pCard,        0,  0, BDM_FLG_READ |BDM_FLG_WORD );
	 if( !err ) err = BDM_Execute( pCard, bdm );
	 return err;

      case EICAN_BDM_FW_WRITE:
	 if( !err ) err = BDM_AddSeq( pCard, bdm->cmd,   0, BDM_FLG_WRITE|BDM_FLG_BYTE );
	 if( !err ) err = BDM_AddSeq( pCard, bdm->data, 32, BDM_FLG_WRITE|BDM_FLG_WORD );
	 if( !err ) err = BDM_Execute( pCard, bdm );
	 return err;

      case EICAN_BDM_CMD:
	 if( !err ) err = BDM_AddSeq( pCard, bdm->cmd, 64, BDM_FLG_WRITE|BDM_FLG_BYTE );
	 if( !err ) err = BDM_Execute( pCard, bdm );
	 return err;

      default:
	 debug( "Unknown BDM command %d\n", bdm->cmd );
	 return -ENOTTY;
   }
   return 0;
}

