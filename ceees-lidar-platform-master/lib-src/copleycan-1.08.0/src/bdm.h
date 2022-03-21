// bdm.h

#ifndef _DEF_INC_BDM
#define _DEF_INC_BDM

#define MAX_BDM_SEQ     5
#define BDM_FLG_BYTE    1
#define BDM_FLG_WORD    0
#define BDM_FLG_WRITE   2
#define BDM_FLG_READ    0

// BDM states
#define BDM_IDLE            0
#define BDM_DONE            1
#define BDM_HW_READ_CMD     2
#define BDM_HW_READ_ADR     3
#define BDM_HW_WRITE_CMD    4
#define BDM_HW_WRITE_ADR    5
#define BDM_FW_READ_CMD     6
#define BDM_FW_WRITE_CMD    7

typedef struct
{
   int16_t brrRead;
   int16_t brrWrite;
   int16_t hc;
   int16_t result;
   int16_t loops;
   int16_t target;

   struct BdmSeq
   {
      int flags;
      int16_t data;
      int16_t to;
   } seq[MAX_BDM_SEQ];
   int seqNdx;
   int seqCt;
} BdmInfo;

// BDM commands to driver
#define EICAN_BDM_SET_HC        0
#define EICAN_BDM_SET_CTRL      1
#define EICAN_BDM_GET_STATUS    2
#define EICAN_BDM_SET_RATE      3
#define EICAN_BDM_GET_RATE      4
#define EICAN_BDM_HW_READ       5
#define EICAN_BDM_HW_WRITE      6
#define EICAN_BDM_FW_READ       7
#define EICAN_BDM_FW_WRITE      8
#define EICAN_BDM_CMD           9

// BDM message
typedef struct
{
   uint8_t op;
   uint8_t cmd;
   uint16_t data;
   uint16_t addr;
} EICAN_BDM;

// prototypes
struct _CardInfo;
int HandleBDM( struct _CardInfo *pCard, EICAN_BDM *bdm );

#endif
