#ifndef __MAH__HEAD__H__
#define __MAH__HEAD__H__


#define pNum 15 
#define pNum_1 (pNum-1) 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct mahj{
	int color;
	int value;
}MAHJ;

typedef struct jiang{
	MAHJ *mahj[2];
}JIANG;

typedef struct treeNode{
	HAND *hand;
	struct treeNode *child[3];//shun1,shun2,shun3,ke
	struct treeNode *parent;
}TreeNode;

typedef struct nodeRes{
	struct treeNode *node;
	struct nodeRes *next;
	struct nodeRes *prev;
}NodeRes;

typedef struct huPai{
	int type;//1:将，2:刻,3:顺
}HuPai;


typedef struct card{
	int amount;
	int index;
}Card;

typedef struct shouPai{
	MAHJ *mahj;
	struct shouPai *next;
	struct shouPai *prev;
}SPai;

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
	int jkN;
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


typedef struct linkSet LinkSet;
typedef struct setLink SetLink;

struct setLink{
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

typedef struct unitIndex{
	int front;
	int back;
}UnitIndex;


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

int getRunTime(int start,int end);
int getCpuTime();
int sortP(int mah[pNum],MAHJ mahj[pNum]);
int getHuNum(int mah[]);
int sumGroup(int group[]);
int g_getShouPai();
int initRes();
int freeRes();
int initJiang();
int initHu();
int freeTreeNode(TreeNode *node);
int freeHu();
int freeJiang();
int saveRes(TreeNode *node);
int saveGu(Card cards[4][9],SPai *&spai,int group[],int &normalN,int &huNum,int &hasJiang);
int checkHu(int);
HAND *newHand();
MAHJ *newMahj();
int str_to_value(char Str_Hex[],MAHJ *pMahj);
int setPaiFromMah(int mah[],int normalN,char Pai[]);
MAHJ value_to_mahj(int value);
int mahj_to_value(MAHJ mahj);
int clearSetLink(SetLink *&link, LinkSet *parent = NULL);
int clearLinkSet(LinkSet *&set);
int clearSetLink(HuSetLink *&link, HuLinkSet *parent = NULL);
int clearLinkSet(HuLinkSet *&set);
HuSetLink *createHuSetLink(SetLink *link);
HuLinkSet *createHuLinkSet(LinkSet *set);
int travelHu(HuSetLink *link,int jkN);
int travelHu(HuLinkSet *set,int jkN);
MAHJ *getMahjPointer(int color,int value);
int printSPai(SPai *ss);
int travel(SetLink *link);
int travel(LinkSet *set);
int travelGu(HuSetLink *link);
int travelGu(HuLinkSet *set);
int travelGuHu(HuSetLink *link,int jkN,SPai *gu);
int travelGuHu(HuLinkSet *set,int jkN,SPai *gu);
int clearHuPai(HuSetLink *link);
int clearHuPai(HuLinkSet *set);
int travelHasGu(HuSetLink *link, SPai *gu);
int travelHasGu(HuLinkSet *set, SPai *gu);

#endif
