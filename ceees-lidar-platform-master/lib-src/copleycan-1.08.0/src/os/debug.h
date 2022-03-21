
#ifndef _DEF_INC_DEBUG
#define _DEF_INC_DEBUG

#include "../internal.h"

// Debug levels
extern int msgLevel;
#define DEBUG_ERROR     0
#define DEBUG_WARN      1
#define DEBUG_INFO      2
#define DEBUG_DBG       3

#define msg( level, kern, fmt, args... ) \
	do{ if( msgLevel >= level ) printk( kern DEVICE_NAME " " fmt, ##args ); } while( 0 )

#define errmsg( fmt,args... ) msg( DEBUG_ERROR, KERN_ERR, fmt, ##args )
#define warn( fmt,args... )  msg( DEBUG_WARN, KERN_WARNING, fmt, ##args )
#define info( fmt,args... )  msg( DEBUG_INFO, KERN_INFO, fmt, ##args )
#define debug( fmt,args... ) msg( DEBUG_DBG, KERN_DEBUG, fmt, ##args )

#endif

