
#ifndef _DEF_INC_EICAN_CMD_INTF
#define _DEF_INC_EICAN_CMD_INTF

/***************************************************************
 * The PCI Host <--> CAN card interface consists of two sections;
 * a block of memory mapped registers, and a section of dual port
 * RAM.  The following address ranges are byte address's and are
 * offsets from the start of the CAN interface.
 *    0x0000 - 0x0FFF:   register space
 *    0x1000 - 0x1FFF:   dual port RAM
 *
 * The RAM space is itself split into several sections:
 *    0x1000 - 0x10FF:  Command block
 *    0x1100 - 0x11FF:  reserved
 *    0x1200 - 0x18FF:  Port 0 queues
 *    0x1900 - 0x1FFF:  Port 1 queues
 **************************************************************/
#define EICAN_INTCTL_BASE      0x0000
#define EICAN_BDM_BASE         0x0400
#define EICAN_TIMER_BASE       0x0600
#define EICAN_CMD_BASE         0x1000
#define EICAN_PORT0_BASE       0x1200
#define EICAN_PORT1_BASE       0x1900

// Number of 32-bit words in each queue entry
#define EICAN_QUEUE_ENTRY_LEN  5

// Number of 32-bit words in the command block
#define EICAN_CMD_LEN          64

/***************************************************************
 * Each port starts with a structure that gives the memory offset
 * of the transmit and receive queues.
 **************************************************************/
typedef struct
{
   uint32_t  recvOff;      // Byte offset of receive queue
   uint32_t  xmitOff;      // Byte offset of transmit queue
   uint32_t  time;         // Time stamp (port 0 only)
   uint32_t  rsvd[5];
} EICAN_PORT_RAM_INFO;

/***************************************************************
 * The interrupt controller consists of three registers; the 
 * interrupt flags, the interrupt mask, and the remote interrupt
 * setting register
 *
 * The interrupt flags register identifies the active interrupts
 * pending for this processor.  These interrupts may be cleared
 * by writing a 1 to the corresponding flag register bit.
 *
 * The interrupt mask register (read/write) allows interrupts to 
 * be masked off.  Only those bits that are set will actually 
 * cause the processor to be interrupted.
 *
 * The remote register shows the interrupt flags of the remote 
 * processor.  Bits can be set by writing 1 to this register.
 * Writes of zero have no effect, only the remote processor can
 * clear it's interrupt flags.
 **************************************************************/
#define EICAN_INT_CMD      0x0001    // Command buffer is owned by this processor
#define EICAN_INT_RECV0    0x0002    // Messages available from CAN port 0
#define EICAN_INT_RECV1    0x0004    // Messages available from CAN port 1
#define EICAN_INT_SEND0    0x0008    // Space available in transmit queue 0
#define EICAN_INT_SEND1    0x0010    // Space available in transmit queue 1
#define EICAN_INT_TIME     0x0100    // Time to update timestamp
typedef struct
{
   uint32_t flags;        // Active interrupts for this processor
   uint32_t mask;         // Mask of enabled ints
   uint32_t remote;       // Active ints on remote processor
   uint32_t revision;     // PCI interface revision number
} EICAN_INTR_CTRL;

// BDM interface (Mostly obsolete, just used to reset card)
typedef struct
{
   uint32_t ctrl;
   uint32_t data;
   uint32_t brr;
   uint32_t to;
} EICAN_BDM_REGS;

/***************************************************************
 * CAN message.  This command structure is used for passing
 * CAN messages to/from the card.  CAN messages may be passed
 * to the card using the normal command space, but normally they
 * are placed on the transmit/receive queue.  Messages on the 
 * queue will be transmitted in FIFO order.  Any message passed
 * using the command structure will be transmitted immediately.
 *
 * Receive messages are always placed in the receive queue.
 **************************************************************/
typedef struct 
{
   uint32_t time;            // Time tag
   uint32_t flags;           // Various flags
   uint32_t msgID;           // CAN message ID.
   uint32_t data[16];        // CAN message data
} CANFW_CANMSG;

/***************************************************************
 * Queue control registers.  Each CAN port has two queues, one
 * for transmit messages and one for received messages.
 *
 * All PCI accesses to the card must access a full 32-bits of 
 * memory.
 *
 * Each queue begins with a structure of this type.
 * Only the microcontroller is allowed to write to the size value, and
 * this will only be done when the micro holds the command block.
 * Only the processor adding data to the queue can modify the head.
 * Only the processor removing data from the queue can modify the tail.
 *
 **************************************************************/
typedef struct 
{
   uint32_t head;
   uint32_t tail;
   uint32_t size;
   uint32_t time;	   // Used on port 0 receive queue only.  Timestamp
   uint32_t fpgaTime;	   // Reserved for use by the FPGA only (local timestamp copy)
   uint32_t threshold;     // Receive interrupt threshold in LSB.
   uint32_t rsvd[2];
   uint32_t data[1];
} EICAN_QUEUE_CTRL;

#endif

