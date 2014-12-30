#include <stdlib.h>
#include <string.h>
typedef struct mahj{
	int color;
	int value;
	mahj *next;
	mahj *prev;
}MAHJ;
template<class T>
T * newStruct(T **t){
	*t = (T *)malloc(sizeof(T));
	memset(*t,0,sizeof(T));
	return *t;
}
template<class T>
int deleteStruct(T *t){
	if(t==NULL)return 1;
	T *tn = t->next;
	T *tp = t->prev;
	if(tn)
		tn->prev = tp;
	if(tp)
		tp->next = tn;
	t->prev = NULL;
	t->next = NULL;
	free(t);
	return 1;
}
