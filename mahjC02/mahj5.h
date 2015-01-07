#ifndef __MAH__HEAD__H__
#define __MAH__HEAD__H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"


typedef struct mahj{
	int color;
	int value;
}MAHJ;

typedef struct card{
	int amount;
	int index;
}Card;

typedef struct shouPai{
	MAHJ *mahj;
	struct shouPai *next;
	struct shouPai *prev;
}SPai;

typedef struct hand{
	int type;
	SPai *sp;
	SPai *ep;
	int num;
	struct hand *next;
	struct hand *prev;
}Hand;


typedef struct sameShouPai{
	SPai *sp;
	SPai *ep;
	int num;
	struct sameShouPai *next;
	struct sameShouPai *prev;
}SSPai;

typedef struct chunk{
	int si;
	int ei;
	int color;
	SSPai *sp;
	SSPai *ep;
	int num;//块内麻将的个数
	struct chunk *next;
	struct chunk *prev;
}Chunk;

typedef struct unit{
	int type;//跨度：000,孤,001,对,010,连(相隔1),011(相隔2) 
	MAHJ *mahj;
	struct unit *next;
	struct unit *prev;
}Unit;

typedef struct huLinkSet HuLinkSet;
typedef struct huSetLink HuSetLink;
struct huSetLink{
	HuLinkSet *parent;
	SPai *hs;
	SPai *he;
	SPai *gs;
	SPai *ge;
	Hand *hds;
	Hand *hde;
	int hasGu;
	int jkN;
	int guN;
	HuLinkSet *ss;
	HuLinkSet *se;
	HuSetLink *next;
	HuSetLink *prev;
};

struct huLinkSet{
	HuSetLink *parent;
	SPai *hs;
	SPai *he;
	SPai *gs;
	SPai *ge;
	int hasGu;
	int jkN;
	int guN;
	Unit *u;
	HuSetLink *ls;
	HuSetLink *le;
	HuLinkSet *next;
	HuLinkSet *prev;
};

typedef struct huUnitSetLink{
	Unit *us;
	Unit *ue;
	Hand *hs;
	Hand *he;
	int jkN; 
	struct huUnitSetLink *next;
	struct huUnitSetLink *prev;
}HuUnitSetLink;

typedef struct huUnitLinkSet{
	SPai *hus;
	SPai *hue;
	HuUnitSetLink *ls;
	HuUnitSetLink *le;
	struct huUnitLinkSet *next;
	struct huUnitLinkSet *prev;
}HuUnitLinkSet;


typedef struct linkSet LinkSet;
typedef struct setLink SetLink;

struct setLink{
	Hand *hs;
	Hand *he;
	LinkSet *ss;
	LinkSet *se;
	SetLink *next;
	SetLink *prev;
};

struct linkSet{
	Unit *u;
	SetLink *ls;
	SetLink *le;
	LinkSet *next;
	LinkSet *prev;
};


template<class T>
T *& newStruct(T *&t){
	t = (T *)malloc(sizeof(T));
	if(t)
		memset(t,0,sizeof(T));
	return t;
}
template<class T>
int deleteStruct(T *&t){
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
	t=tn;
	return 1;
}

template<class T>
int clearStruct(T *&t){
	if(t == NULL)return -1;
	while(t)
		deleteStruct(t);
}

template<class T>
T * insertStruct(T *&t, T *&start, T *&end){
	if(t==NULL)return NULL;
	if(start == NULL)
	{
		start = t;
		end = t;
	}
	else
	{
		end -> next = t;
		t -> prev = end;
		end = t;
	}
	return t;
}

template<class T>
T * addStruct(T *&start,T *&end)
{
	T *t = NULL;
	newStruct(t);
	if(t)
		return insertStruct(t,start,end);
}

template<class T>
int getDistance(T *start,T *end)
{
	int dis = 0;
	while(start != end)
	{
		dis++;
		start = start->next;
	}
	return dis;
}

template<class T>
T * getPointer(T *start,int dis)
{
	T * t = start;
	while(dis!=0 && t && t->next)
	{
		t = t->next;
		dis --;
	}
	return t;
}

template<class T>
int swapValue(T &t1,T &t2)
{
	T t = t1;
	t1 = t2;
	t2 = t;
}
//function 
int g_init();
struct timeval setTime(int i);
int getRunTime(int start,int end);
int getCpuTime();
int clearChunk(Chunk *chunkS);
int clearSetLink(SetLink *&link, LinkSet *parent);
int clearLinkSet(LinkSet *&set);
int clearSetLink(HuSetLink *&link, HuLinkSet *parent);
int clearLinkSet(HuLinkSet *&set);
int splitStr(char *src, const char *delim, char *ps[]);
int splitLinkStr(char *linkStr,int front[],int back[]);
int splitSetStr(char *setStr,int* front[],int* back[],int *num);
int sortP(int mah[],MAHJ mahj[], int huNum);
int getHuNum(int mah[], int huNum);
int sumGroup(int group[]);
SPai *copySPai(SPai *start,SPai *end,SPai *deletes[],int sdi,int num);
Chunk *splitSubChunk(SPai *sp);
int splitChunk(Chunk *&chunkS,int normalNum);
int diff(int obj[],int src[],int type,int num);//差分
int product(int doc[],int vec1[],int vec2[],int num);//点积
int addUnit(SetLink *link, Chunk *pChunk, int value[], int front, int back);
int addLink(LinkSet *set, Chunk *pChunk, int value[], int front[], int back[], int num);
int addTwoATwoSet(SetLink *link, Chunk *pChunk,int *value, int front[],int fi,int back[],int bi,int num);
int addArraySet(SetLink *link, Chunk *pChunk,int *value, int index[],int si,int num);
int build(Chunk *chunkS,SetLink *link,int huNumJ,int height);
int travelHasGu(HuSetLink *link, SPai *gu);
int travelHasGu(HuLinkSet *set, SPai *gu);
HuSetLink *createHuSetLink(SetLink *link);
HuLinkSet *createHuLinkSet(LinkSet *set);
int getOutJKN(HuLinkSet *set);
int getOutGuJKN(HuLinkSet *set);
int getOutGu(HuLinkSet *set);
int addSPai(SPai *&ss,SPai *&se,int color,int value);
int addSPai(SPai *&ss,SPai *&se,SPai *sp);
MAHJ *getMahjPointer(int color,int value);
int printSPai(SPai *ss);
int printMahj(MAHJ *mahj);
int travelHasGu(HuSetLink *link, SPai *gu);
int travelHasGu(HuLinkSet *set, SPai *gu);
int travelHu(HuLinkSet *set,HuUnitLinkSet *huset=NULL);
int travelHu(HuSetLink *link,HuUnitLinkSet *huset=NULL);
int travelGuHu(HuSetLink *link);
int travelGuHu(HuLinkSet *set);
int clearHuPai(HuSetLink *link);
int clearHuPai(HuLinkSet *set);
int travelGu(HuSetLink *link);
int travelGu(HuLinkSet *set);
int travel(SetLink *link,int hight=0);
int travel(LinkSet *set,int hight=0);
int checkHu(int huNum, int pNum);
int str_to_value(char Str_Hex[],MAHJ *pMahj);
int setPaiFromMah(int mah[],int normalN,char Pai[]);
MAHJ value_to_mahj(int value);
int mahj_to_value(MAHJ mahj);
int saveHuUnitLink(HuUnitSetLink *hulink,HuLinkSet *set);
int saveHuUnitLink(HuUnitLinkSet *huset,HuLinkSet *set);
int travel(HuSetLink *link,int hight=0);
int travel(HuLinkSet *set,int hight=0);
int travelHand(Hand *hs,int debugLevel=2);
int printUnit(Unit *u);

#endif
