/* utils.h */

#ifndef _DEF_INC_UTILS
#define _DEF_INC_UTILS

#include "internal.h"

// prototypes 
uint32_t ReadSafe( PortInfo *pPort, uint32_t *addr );
void IntMaskUpdt( CardInfo *card, uint32_t bits, int enable );
uint32_t GetPciRev( CardInfo *pCard );
void InitCardPointers( CardInfo *pCard );
void ReadRxqHead( PortInfo *pPort, uint32_t *head, uint32_t *tail, uint32_t *size );
void ReadTxqHead( PortInfo *pPort, uint32_t *head, uint32_t *tail, uint32_t *size );
int DecodeMsgLen( uint8_t ct );

#endif
