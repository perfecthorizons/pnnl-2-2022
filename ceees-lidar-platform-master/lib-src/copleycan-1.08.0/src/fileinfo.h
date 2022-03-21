
#ifndef _DEF_INC_FILEINFO
#define _DEF_INC_FILEINFO

#include "internal.h"

// prototypes 
FileInfo *CreateFile( PortInfo *pPort );
void FreeFile( FileInfo *pFile );
int CountFiles( PortInfo *pPort );

#endif
