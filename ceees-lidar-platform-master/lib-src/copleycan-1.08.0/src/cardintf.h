/******************************************************
 * OS Generic interface to the CAN card
 *****************************************************/

#ifndef _DEF_INC_CARDINTF
#define _DEF_INC_CARDINTF

#include "internal.h"

/* prototypes */
int CardInit( CardInfo *card, char *hwPtr, uint32_t hwSize );
void CardEnable( CardInfo *card );
void CardShutDown( CardInfo *card );
int EICAN_ISR_Top( CardInfo *card );
void EICAN_ISR_Bottom( CardInfo *card );
int TestCard( CardInfo *card );


#endif

