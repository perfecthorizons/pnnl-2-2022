
#ifndef _DEF_INC_MAIN
#define _DEF_INC_MAIN

#include <linux/ioctl.h>

// I/O Ctrl codes
#define EICAN_IOCTL_TYPE            0xB7
#define EICAN_IOCTL_SENDCAN         _IOW ( EICAN_IOCTL_TYPE, 0x04, CANCARD_MSG )
#define EICAN_IOCTL_SENDFD          _IOW ( EICAN_IOCTL_TYPE, 0x04, CANCARD_MSG_FD )
#define EICAN_IOCTL_RECVCAN         _IOWR( EICAN_IOCTL_TYPE, 0x05, CANCARD_MSG )
#define EICAN_IOCTL_RECVFD          _IOWR( EICAN_IOCTL_TYPE, 0x05, CANCARD_MSG_FD )
#define EICAN_IOCTL_CMD             _IOWR( EICAN_IOCTL_TYPE, 0x06, long        )
#define EICAN_IOCTL_BDM             _IOWR( EICAN_IOCTL_TYPE, 0x07, EICAN_BDM   )
#define EICAN_IOCTL_PCITEST         _IO( EICAN_IOCTL_TYPE, 0xF1 )

#endif

