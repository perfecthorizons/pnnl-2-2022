
#include "fileinfo.h"
#include "os/debug.h"

// local functions
static int AddFile( FileInfo *pFile );
static void RemFile( FileInfo *pFile );

FileInfo *CreateFile( PortInfo *pPort )
{
   FileInfo *pFile;

   // Allocate the file info structure from unpaged memory
   pFile = (FileInfo *)kmalloc( sizeof(FileInfo), GFP_KERNEL );
   if( !pFile ) goto fail;

   // Clear the structure
   memset( pFile, 0, sizeof(FileInfo) );

   // Allocat a receive message queue for this file
   pFile->recvMsgQueue = MsgQueueAlloc( 0 );
   if( !pFile->recvMsgQueue )
      goto freeFile;

   // Initialize the file info contents
   pFile->pPort = pPort;
   pFile->open = 0;
   pFile->needXmit = 0;

   // Add this file to the list held by the port
   if( !AddFile( pFile ) )
      goto freeMsgQueue;

   return pFile;

freeMsgQueue:
   MsgQueueFree( pFile->recvMsgQueue );

freeFile:
   kfree( pFile );

fail:
   errmsg( "Unable to allocate file info\n" );
   return 0;
}

/**
 * Release any memory allocated to this file
 */
void FreeFile( FileInfo *pFile )
{
   RemFile( pFile );
   MsgQueueFree( pFile->recvMsgQueue );
   kfree( pFile );
}

/**
 * Add a pointer to this file to the port's list.
 * @return non-zero on success
 */
static int AddFile( FileInfo *pFile )
{
   PortInfo *pPort = pFile->pPort;
   CardInfo *pCard = pPort->pCard;
   SPINLOCK_STATE state;
   int i;

   SpinLockGet( &pCard->spin, &state );
   for( i=0; i<MAX_FILES; i++ )
   {
      if( pPort->files[i] == 0 )
      {
	 pPort->files[i] = pFile;
	 break;
      }
   }

   SpinLockPut( &pCard->spin, &state );

   return i<MAX_FILES;
}

/**
 * Remove this file from the port's list.
 */
void RemFile( FileInfo *pFile )
{
   PortInfo *pPort = pFile->pPort;
   CardInfo *pCard = pPort->pCard;
   SPINLOCK_STATE state;
   int i;

   SpinLockGet( &pCard->spin, &state );

   for( i=0; i<MAX_FILES; i++ )
   {
      if( pPort->files[i] == pFile )
      {
	 pPort->files[i++] = 0;
	 break;
      }
   }

   for(; i<MAX_FILES; i++ )
   {
      pPort->files[i-1] = pPort->files[i];
      pPort->files[i] = 0;
   }
   SpinLockPut( &pCard->spin, &state );
}

// Count the number of files currently open on this port
int CountFiles( PortInfo *pPort )
{
   SPINLOCK_STATE state;
   CardInfo *pCard = pPort->pCard;
   SpinLockGet( &pCard->spin, &state );

   int ct = 0;
   for( int i=0; i<MAX_FILES; i++ )
   {
      if( pPort->files[i] )
         ct++;
   }

   SpinLockPut( &pCard->spin, &state );
   return ct;
}
