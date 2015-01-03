#ifndef __COMMON_H__
#define __COMMON_H__

#define debug_level 2
#if debug_level != 0
#define MyTrace(level,fmt, args...) do { \
	if( level <= debug_level )		\
		printf(fmt, ## args); \
} while (0)
#else
	#define MyTrace(a...)	
#endif

#endif
