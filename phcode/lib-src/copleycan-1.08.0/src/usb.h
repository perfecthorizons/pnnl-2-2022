/* usb.h */

#ifndef _DEF_INC_USB
#define _DEF_INC_USB

#include "internal.h"

// prototypes
int USB_CardEnable( CardInfo *pCard );
int USB_SendCmd( CardInfo *pCard, uint32_t *cmd, uint32_t xCt, uint32_t *rCt );
int USB_SendCAN( PortInfo *pPort, CANFW_CANMSG *msg, int timeout, int wct );

#endif
