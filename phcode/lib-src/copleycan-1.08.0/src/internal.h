/*******************************************************
 * Driver internal data structures
 ******************************************************/

#ifndef _DEF_INC_INTERNAL
#define _DEF_INC_INTERNAL

#include "version.h"

#include "os/support.h"
#include "cancard_hw_intf.h"
#include "bdm.h"
#include "MsgQueue.h"

// Device and class names
#define DEVICE_CLASS  "cancard"
#define DEVICE_NAME   "copleycan"

// Some arbitrary limits
#define MAX_CARDS      10        // Max number of cards that can be used at any time
#define MAX_PORTS       2        // Maximum number of CAN ports / card
#define MAX_FILES       8        // Maximum number of open files / port
#define MAX_XMIT_HOLD 512        // Number of transmit messages that can be held by the card at once

/****************************************************
 * Hardware message queue control.  Each card has two
 * receive, and two transmit queues.  This structure 
 * is used to access them.
 ****************************************************/
typedef struct _QueueInfo
{
   EICAN_QUEUE_CTRL *ctrl;
   uint32_t mask;

   EVENT_TYPE event;
   MUTEX_TYPE mutex;
} QueueInfo;

/****************************************************
 * Port specific data.
 * Each device has one or more CAN ports associated with it.
 * This structure holds the port specific data.
 ****************************************************/
typedef struct _PortInfo
{
   struct _CardInfo *pCard;

   // OS specific port data
   OS_PortData os;

   // Hardware queues on the card
   QueueInfo send;
   QueueInfo recv;

   // This mask is passed with all commands sent 
   // through this port
   uint32_t mask;

   // Count of the number of files have this port open
   ATOMIC_INT openCt;

   // Count of files that require transmit acks
   ATOMIC_INT xmitAckCt;

   // Keep a list of all files open on this port
   struct _FileInfo *files[ MAX_FILES ];

   // Next transmit message ID
   ATOMIC_INT nextXmitID;

   // Messages currently being sent
   CANFW_CANMSG xmitBuff[ MAX_XMIT_HOLD ];
   SPINLOCK_TYPE xmitBuffLock;
   int xmitBuffHead;
   int xmitBuffTail;

   // High water mark for on-card receive buffer
   int recvHighWater;

   // USB support
   struct urb *usbintX;
   uint32_t *usbTxBuff;
   int usbTxLen;
   EVENT_TYPE usbTxEvent;

} PortInfo;

/****************************************************
 * Card information structure.
 *
 * This structure holds data that's global to a single 
 * CAN card.  
 ****************************************************/
typedef struct _CardInfo 
{
   // OS specific data 
   OS_CardData os;       

   // Spinlock for controlling device specific data
   SPINLOCK_TYPE spin;

   // Pointer to my card's memory and it's size
   char *hwPtr;
   uint32_t hwSize;

   // Location where timestamps should be written 
   // to the card.
   uint32_t *timePtr;

   // Local copy of my interrupt mask.  Updates 
   // to this are protected by my spinlock
   uint32_t intMask;

   // Resources used to handle the command area
   EVENT_TYPE cmdEvent;
   MUTEX_TYPE cmdMutex;

   // Port specific data for both CAN ports
   int portCt;
   PortInfo port[MAX_PORTS];

   // True if safe reads are required for some registers
   int useSafeReads;

   // True if CAN-FD is supported by card
   int supportFD;

   // Non-zero if CAN data is written big endian
   int bigEndianData;

   // Non-zero if the card has been removed
   int cardRemoved;

   // Debug only
   BdmInfo bdm;

   // USB support
   struct urb *usbcmd;
   struct urb *usbintR;
   int usbTxBuffLen;
   int usbRxBuffLen;

} CardInfo;

/****************************************************
 * File information structure.
 *
 * Each of my CAN ports may be accessed through one 
 * or more file pointers.
 *
 * Each file pointer identifies a specific port that
 * it accesses.  
 *
 * All writes are sent in order through the CAN port,
 * but reads are copied into a file pointer specific 
 * queue so that each file pointer sees the same receive
 * data.
 ****************************************************/
typedef struct _FileInfo
{
   // The port that this file references
   PortInfo *pPort;

   // Queue of received messages
   MsgQueue *recvMsgQueue;

   // non-zero if the port is currently open by this file
   int open;

   // Receive port transmissions
   int needXmit;
} FileInfo;

#endif
