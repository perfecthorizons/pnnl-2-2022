
#ifndef _DEF_INC_CANCARD_CMD_INTF
#define _DEF_INC_CANCARD_CMD_INTF

#include "cancard_hw_intf.h"

#define EICAN_PARAM_SERIAL      0
#define EICAN_PARAM_MFGINFO     1
#define EICAN_PARAM_PCIVOLT     2
#define EICAN_PARAM_33VOLT      3
#define EICAN_PARAM_25VOLT      4
#define EICAN_PARAM_STATUS      5
#define EICAN_PARAM_OPTIONS     6
#define EICAN_PARAM_CHIPDATE    7
#define EICAN_PARAM_CHIPSERIAL  8
#define EICAN_PARAM_INTINHIBIT  10
#define EICAN_PARAM_CANLOAD     11
#define EICAN_PARAM_CARDSTAT    12

/**************************************************************
 * The following structure is used to pass a CAN message 
 * frame to/from the CAN card.
 *************************************************************/
#define EICAN_CANFLG_LENGTH        0x0000000F   // Length of message data (encoded)
#define EICAN_CANFLG_RTR           0x00000010   // Set for RTR messages
#define EICAN_CANFLG_SENDNOTIFY    0x00000020   // For transmits, notify on success
#define EICAN_CANFLG_NAK           0x00000040   // Don't retry on missing ACK
#define EICAN_CANFLG_ERROR         0x00000080   // Message contains error counter info
#define EICAN_CANFLG_FD            0x00000100   // Send in FD mode
#define EICAN_CANFLG_BRS           0x00000200   // BRS for FD mode
#define EICAN_CANFLG_ESI           0x00000400   // ESI bit for FD mode (rx only)
#define EICAN_CANFLG_PORT          0x00001000   // Identifies which port the message is from/to

typedef struct __CANCARD_MSG
{
   int32_t  timeout;
   uint32_t timestamp;
   uint32_t id;
   uint32_t flags;
   uint8_t  data[8];
} CANCARD_MSG;

typedef struct __CANCARD_MSG_FD
{
   int32_t  timeout;
   uint32_t timestamp;
   uint32_t id;
   uint32_t flags;
   uint8_t  data[64];
} CANCARD_MSG_FD;

/**************************************************************
 * Most other communications with the card use a generic command
 * structure consisting of an array of 32-bit words.  
 *
 * The first 32-bit word of this array has a fixed structure for 
 * all commands.  The remaining words are command dependent.
 *
 * All commands are at least 1 word long.  This first word is 
 * formatted as follows:
 *
 *    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *    ..........................\\\\\\-- Number of data words
 *    .......................\---------- CAN channel
 *    ........\\\\\\\\------------------ Command / Response code
 *    \\\\\\\\-------------------------- reserved
 *
 * The number of data words should specify the number of additional
 * 32-bit words of data passed with the command.  On response, the
 * CAN card will initialize this to the number of data words in
 * the response message.
 *
 * The CAN channel bit identifies which of the two CAN ports 
 * the command refers to.  Not all commands are specific to a 
 * port.  For these commands the channel number is reserved.
 *
 * The Command/Response code should take on one of the values
 * documented below.  Also documented is the format of any 
 * additional data that needs to be passed with the command.
 *************************************************************/

// The following command codes are currently defined.
#define EICAN_CMD_OPENPORT          0x01      // Open a CAN channel (no data)
#define EICAN_CMD_CLOSEPORT         0x02      // Close a CAN channel (no data)
#define EICAN_CMD_SETBPS            0x03      // Set bitrate info
#define EICAN_CMD_GETBPS            0x04      // Get bitrate info
#define EICAN_CMD_GETVERSION        0x05      // Get firmware version info
#define EICAN_CMD_FIRMWARE          0x06      // Update card firmware
#define EICAN_CMD_FWDATA            0x07      // Used to pass data during firmware update
#define EICAN_CMD_SWAPFW            0x08      // Switch between boot and normal firmware
#define EICAN_CMD_SETPARAM          0x09      // Set a parameter
#define EICAN_CMD_GETPARAM          0x0A      // Get a parameter
#define EICAN_CMD_SELFTEST          0x0B      // Run a quick self-test
#define EICAN_CMD_ANALYZER          0x0C      // Access CAN signal analyzer
#define EICAN_CMD_SET_FDBPS         0x0D      // Set bitrate for CAN-FD data
#define EICAN_CMD_GET_FDBPS         0x0E      // Get bitrate for CAN-FD data
#define EICAN_CMD_RECV_THRESH       0xF9      // Set the receive buffer interrupt threshold 
#define EICAN_CMD_PCIPEEK           0xFA      // Peek at card memory from PCI interface
#define EICAN_CMD_PCIPOKE           0xFB      // Poke to card memory from PCI interface
#define EICAN_CMD_RECVXMIT          0xFC      // Receive transmit messages
#define EICAN_CMD_PCITEST           0xFD      // Run a quick PCI bus test
#define EICAN_CMD_DRIVERVER         0xFE      // Get driver version
#define EICAN_CMD_RESET             0xFF      // Reset the card

// The following response codes will be returned by the card
#define EICAN_ERR_OK                0x00       // No error
#define EICAN_ERR_UNKNOWN_CMD       0x01       // Passed command ID was unknown
#define EICAN_ERR_BAD_PARAM         0x02       // Illegal parameter passed
#define EICAN_ERR_PORT_OPEN         0x03       // Specified CAN port is already open
#define EICAN_ERR_PORT_CLOSED       0x04       // Specified CAN port is not open
#define EICAN_ERR_CARD_BUSY         0x05       // Card command area is busy
#define EICAN_ERR_INTERNAL          0x06       // Some sort of internal device failure
#define EICAN_ERR_TIMEOUT           0x07       // Card failed to respond to command
#define EICAN_ERR_SIGNAL            0x08       // Signal received by driver
#define EICAN_ERR_MISSING_DATA      0x09       // Not enough data was passed
#define EICAN_ERR_CMDMUTEX_HELD     0x0A       // The command mutex was being held
#define EICAN_ERR_QUEUECTRL         0x0B       // The CAN message queue head/tail was invalid
#define EICAN_ERR_FLASH             0x0C       // Failed to erase/program flash memory
#define EICAN_ERR_NOTERASED         0x0D       // Attempt to write firmware before erasing flash
#define EICAN_ERR_FLASHFULL         0x0E       // Too much data sent when programming flash
#define EICAN_ERR_UNKNOWN_IOCTL     0x0F       // Specified IOCTL code was unknown
#define EICAN_ERR_CMD_TOO_SMALL     0x10       // Command passed without required header
#define EICAN_ERR_CMD_TOO_BIG       0x11       // Command passed with too much data
#define EICAN_ERR_CMD_IN_PROGRESS   0x12       // Command already in progress on card
#define EICAN_ERR_CAN_DATA_LENGTH   0x13       // More then 8 bytes of data sent with CAN message
#define EICAN_ERR_QUEUE_FULL        0x14       // Transmit queue is full
#define EICAN_ERR_QUEUE_EMPTY       0x15       // Receive queue is full
#define EICAN_ERR_READ_ONLY         0x16       // Parameter is read only
#define EICAN_ERR_MEMORYTEST        0x17       // Memory read/write test failure
#define EICAN_ERR_ALLOC             0x18       // Memory allocation failure
#define EICAN_ERR_CMDPENDING        0x19       // Used internally by driver
#define EICAN_ERR_DRIVER            0x1A       // Generic device driver error
#define EICAN_ERR_OK_WITHDATA       0x1B       // Used internally by driver
#define EICAN_ERR_BOOT_MODE         0x1C       // Card currently running in boot mode

/**************************************************************
 * The SETBPS command passes the following data to the card:
 * The GETBPS command will send a response with the same data.
 *
 * data[1]: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *          ........................\\\\\\\\ - rate code
 *          ................\\\\\\\\---------- tq
 *          ........\\\\\\\\------------------ ts1
 *          \\\\\\\\-------------------------- ts2
 *
 * data[2]: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *          ........................\\\\\\\\ - sjw
 *          .......................\---------- samp
 *          \\\\\\\\\\\\\\\\\\\\\\\----------- reserved
 *
 * Normally, the only field with data is the rate code which 
 * should be one of the values specified below.  If the 
 * 'user defined' rate code is specified, then all the other 
 * fields are used to configure the CAN bitrate.
 *
 *   tq   - Number of 16MHz clock cycles / time quanta (1-64)
 *   ts1  - Number of time quanta in time segment 1 (1-16)
 *   ts2  - Number of time quanta in time segment 2 (1-8)
 *   sjw  - Sync jump width (1-4)
 *   samp - If zero, take one sample.  If 1, average three samples.
 *************************************************************/

// Bit rate codes
#define EICAN_BITRATE_USER                  0
#define EICAN_BITRATE_1000000               1
#define EICAN_BITRATE_800000                2
#define EICAN_BITRATE_500000                3
#define EICAN_BITRATE_250000                4
#define EICAN_BITRATE_125000                5
#define EICAN_BITRATE_100000                6
#define EICAN_BITRATE_50000                 7
#define EICAN_BITRATE_20000                 8

/**************************************************************
 *
 * The Get Version command returns four words of data:
 *  - Hardware version code
 *  - Boot loader version
 *  - Main firmware version number
 *  - FPGA code version
 *
 * Each version is broken down as follows
 *    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 *    ........................\\\\\\\\----- Release number
 *    ................\\\\\\\\------------- Minor version 
 *    ........\\\\\\\\--------------------- Major version 
 *    \\\\\\\\----------------------------- reserved
 *
 *************************************************************/

#endif

