#ifndef __COMMON_H__
#define __COMMON_H__

#define debug_level 1
#define printLine 0
#if debug_level != 0
#define MyTrace(level,fmt, args...) do { \
	if( level <= debug_level )		\
	{\
		if(printLine)\
			printf("____LINE::%d____,____FUNC_____:%s______\n",__LINE__,__FUNCTION__);\
		printf(fmt, ## args); \
	}\
} while (0)
#else
	#define MyTrace(a...)	
#endif

#endif
