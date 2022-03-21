
#ifndef _DEF_INC_RECV
#define _DEF_INC_RECV

#include "os/support.h"
#include "cancard_cmd_intf.h"
#include "internal.h"

// prototypes
int RecvCAN( FileInfo *pFile, CANCARD_MSG *can );
void RecvInt( PortInfo *pPort );
void FlushRecvQueue( PortInfo *pPort );

#endif
