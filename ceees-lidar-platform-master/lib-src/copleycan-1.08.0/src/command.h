/******************************************************
 * Command interface to CAN card
 *****************************************************/

#ifndef _DEF_INC_COMMAND
#define _DEF_INC_COMMAND

#include "internal.h"

/* prototypes */
int SendCmd( FileInfo *pFile, uint32_t *cmd, uint32_t xCt, uint32_t *rCt );

#endif

