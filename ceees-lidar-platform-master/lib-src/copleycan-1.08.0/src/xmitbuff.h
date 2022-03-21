

#ifndef _DEF_INC_XMITBUFF
#define _DEF_INC_XMITBUFF

#include "internal.h"

void XmitBuffInit( CardInfo *pCard );
void XmitBuffAdd( PortInfo *pPort, CANFW_CANMSG *msg );
int XmitBuffGet( PortInfo *pPort, CANFW_CANMSG *msg );

#endif
