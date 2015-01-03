#include <stdio.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<ctype.h>
#include <stdlib.h>
#include "mahj3.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

int g_zong=0;
TreeNode *g_root = NULL;
Chunk *g_chunkS=NULL;
Chunk *g_chunkE=NULL;
NodeRes *g_resS=NULL;
NodeRes *g_resE=NULL;
int g_huNum = 0;
int g_normalNum = 0;

JIANG g_jiang;
MAHJ g_mahj[pNum];
SPai *g_spaiS[4];
SPai *g_spaiE[4];
SSPai *g_sspaiS[4];
SSPai *g_sspaiE[4];
int g_init()
{
	initJiang();
	memset(g_mahj,0,sizeof(g_mahj));
	memset(g_spaiS,0,sizeof(g_spaiS));
	memset(g_spaiE,0,sizeof(g_spaiE));
	memset(g_sspaiS,0,sizeof(g_sspaiS));
	memset(g_sspaiE,0,sizeof(g_sspaiE));
}

struct timeval timeVal[10];

struct timeval setTime(int i)
{
	gettimeofday(&timeVal[i],0);
}

int getRunTime(int start,int end)
{
	setTime(end);
	return 1000000 * (timeVal[end].tv_sec-timeVal[start].tv_sec)+ timeVal[end].tv_usec-timeVal[start].tv_usec;
}

int getCpuTime()
{
	struct rusage rup;
	getrusage(RUSAGE_SELF, &rup);
	long sec = rup.ru_utime.tv_sec + rup.ru_stime.tv_sec;
	long usec = rup.ru_utime.tv_usec + rup.ru_stime.tv_usec;
	printf("%d:%d",sec,usec);
	usec += sec*1000000;
	return usec;
}

int clearChunk(Chunk *pChunk)
{
	clearStruct(pChunk ->sp ->sp);
	while(pChunk)
	{
		clearStruct(pChunk ->sp);
		pChunk = pChunk ->next;
	}
	clearStruct(pChunk);
}

int splitStr(char *src, const char *delim, char *ps[])
{
	int num = 0;
	while(1)
	{
		char *s = NULL;
		if(num == 0)
			s = src;
		char *p = strtok(s,delim);
		if(p == NULL)
			break;
		ps[num] = p;
		num ++;
	}
	return num;
}

int splitLinkStr(char *linkStr,int front[],int back[])
{
	char *ps[10];
	int num = splitStr(linkStr,",",ps);
	int i;
	for(i=0;i<num;i++)
	{
		char *index[2];
		splitStr(ps[i],":",index);
		front[i] = *(index[0]) - '0';
		back[i] = *(index[1]) - '0';
		if(back[i] == -3)//'-'
			back[i] = -1;
	}
	return num;
}

int splitSetStr(char *setStr,int* front[],int* back[],int *num)
{
	char *linkStr[10];
	int n = splitStr(setStr,";",linkStr);
	int i;
	for(i=0;i<n;i++)
	{
		int f[10];
		int b[10];
		num[i] = splitLinkStr(linkStr[i],f,b);
		front[i] = (int *)malloc(num[i] * sizeof(int));
		back[i] = (int *)malloc(num[i] * sizeof(int));
		int j;
		for(j=0;j<num[i];j++)
		{
			*(front[i]+j) = f[j]; 
			*(back[i]+j) = b[j]; 
		}
	}
	return n;
}



static int huShu;
int main(int argc, char *argv[])
{	
	huShu = 0;
	setTime(0);
	FILE * fp = NULL;
	char * line = NULL;
    size_t len = 0;
	const char *delim=" ";
	char *p;
	int mah[pNum], mah_num = 0, j; 
	int count = 0,flag = 0;
	int color, val, ture_color;
	Card cards[4][9]; 
	 fp = fopen("data3", "r");//打开文件
	 if (fp == NULL)
	 {
		 printf("打开文件失败！\n");
       exit(EXIT_FAILURE);
	 }

     while ((getline(&line, &len, fp)) != EOF) //读取一行
	{	
		memset(&mah,0,sizeof(mah));//二维数组初始化为0
		memset(&g_mahj,0,sizeof(g_mahj));
		mah_num = 0;
		p = strtok(line,delim);//分割一行中的第一个数
		if((p[0] == ' ') || (p[0] == 13) || (p[0] == 10))//屏蔽所有空格，换行。
		{
			continue;
		}
		if((mah[0] = str_to_value(p,&g_mahj[0])) == -1)//第0个都不合法//本行分割不成功，读取下一行
		{
			count++;//行计数器加一
			continue;//结束本行分割，读取下一行
		}
		for(mah_num=1;(NULL!=(p=strtok(NULL,delim)));mah_num++)//分割，直到无从分割
		{
			if((p[0]==' ') || (p[0]==13) || (p[0]==10))//屏蔽所有空格，换行。
			{
				continue;
			}	
			if((mah[mah_num] = str_to_value(p,&g_mahj[mah_num])) == -1)//第i个不合法
			{
				flag = 1;//本行分割不成功，读取下一行
				break;//结束本行分割
			}
		}
		if(flag == 1)// 第%d行的第%d个数据不合法
		{
			flag = 0;
			count++;	
			continue;
		}
		if((mah_num > pNum)||(mah_num < pNum_1))//第%d行只有%d个数据不合法,本行分割不成功，读取下一行
		{
			count++;	
			continue;
		}
		count++;//成功，行计数器加一

		//////////////////////将麻将放入二维数组//本行分割成功，放入二维数组	
		memset(&cards,0,sizeof(cards));//二维数组初始化为0
		//将要分析的十四张牌放入二维数组中 //赖子值设置为-1；
		for(j = 0; j < pNum_1 ;j++)
		{
			int color = g_mahj[j].color;	//取的花色真值：1、2、4、8
			int val = g_mahj[j].value;
			cards[color][val].amount ++; //牌数
			if(cards[color][val].index == 0)
				cards[color][val].index = j;	//牌的开始索引
		}
		///查看是否有四张以上的牌，有则不合法。读取下一行
		flag = 0;
		for(color = 0; color < 4; color++)
		{
			int valMax = 9;
			if(color == 3)valMax = 7;
			for(val = 0;val < valMax;val++)
			{
				if(cards[color][val].amount> 4)
				{
					flag=1;
					break;
				}
			}
			if(flag)
			{
				break;
			}
		}
		if(flag)
		{
			flag=0;
			continue;
		}
		g_zong++; //合法的数据条数
		g_init();
		sortP(mah,g_mahj);
		g_huNum = getHuNum(mah);
		g_normalNum =  pNum - g_huNum;
		g_getShouPai();//获取手牌
		int n = g_split();
		printf("chunkNum:%d\n",n);
		if(n>5)//分块
		{
			printf("there is too many chunks to huPai\n");
		}
		checkHu(g_huNum);
		printf("time:%d\n",getRunTime(0,1));
	}
	 printf("total huShu:%d\n",huShu);

	fclose(fp);
	fp = NULL;
	if (line)
	{
       free(line);
	}
    return EXIT_SUCCESS;
}

int sortP(int mah[pNum],MAHJ mahj[pNum])
{
	int i;
	for(i = 0; i<pNum_1; i++)
	{
		int j;
		for(j = 0;j<pNum_1-i;j++)
		{
			if(mah[j] > mah[j+1])
			{
				MAHJ tmp = mahj[j];
				mahj[j] = mahj[j+1]; 
				mahj[j+1] = tmp;
			}
		}
	}
}

int getHuNum(int mah[])
{
	int i = pNum;
	while(mah[--i]==33);
	return pNum - i - 1;
}




int sumGroup(int group[])
{
	return group[0]+group[1]+group[2];
}

int g_getShouPai()
{
	int i = 0;
	for(i=0;i<g_normalNum;i++)
	{
		MAHJ* mahj = &(g_mahj[i]);
		int color = mahj ->color;
		SPai *spai = addStruct(g_spaiS[color],g_spaiE[color]);
		spai ->mahj=mahj;
	}
	for(i=0;i<4;i++)
	{
		SPai *p = g_spaiS[i];
		int oldValue = -1;
		while(p)
		{
			int value = p ->mahj ->value;
			if(value!=oldValue)
			{
				oldValue = value;
				SSPai *sspai = addStruct(g_sspaiS[i],g_sspaiE[i]);
				sspai ->sp = p;
				sspai ->ep = p;
				sspai ->num = 1;
			}
			else
			{
				g_sspaiE[i] ->ep = p;
				g_sspaiE[i] ->num ++;
			}
			p = p ->next;
		}
	}
}

int initRes()
{
	g_resS = NULL;
	g_resE = g_resS;
}

int freeRes()
{
	NodeRes *node = g_resS;
	int i = 0;
	while(node)
	{
		NodeRes *nodeN = NULL;
		if(node ->next)
			nodeN = node ->next;
		if(node)
		{
			i++;
			free(node);
		}
		node = nodeN;
	}
	g_resS = NULL;
	g_resE = NULL;
}

int initJiang()
{
	g_jiang.mahj[0] = newMahj();
	g_jiang.mahj[1] = newMahj();
}

int initHu()
{
	initRes();
}

int freeTreeNode(TreeNode *node)
{
	if(node == NULL)
		return 0;
	int i;
	int j = 0;
	for(i=0;i<4;i++)
	{
		j = j + freeTreeNode(node ->child[i]);
		node ->child[i] = NULL;
	}
	if(node ->hand)
	{
		int i;
		for(i=0;i<3;i++)
		{
			free(node ->hand ->mahj[i]);
			node ->hand ->mahj[i] = NULL;
		}
		free(node ->hand);
		node ->hand = NULL;
	}
	free(node);
	return j+1;
}

int freeHu()
{
	freeRes();
	//freeTreeNode(root);
}

int freeJiang()
{
	free(g_jiang.mahj[0]);
	free(g_jiang.mahj[1]);
	g_jiang.mahj[0] = NULL;
	g_jiang.mahj[0] = NULL;
}

int saveRes(TreeNode *node)
{
	huShu++;
	NodeRes *nodeRes = (NodeRes *)malloc(sizeof(NodeRes));
	if(nodeRes==NULL)
		return -1;
	TreeNode *pNode = node;
	nodeRes ->node = pNode;
	nodeRes ->next = NULL;
	nodeRes ->prev = g_resE;
	if(g_resS == NULL)
	{
		g_resS = nodeRes;
	}
	else
	{
		g_resE ->next = nodeRes;
	}
	g_resE = nodeRes;
}


int saveGu(Card cards[4][9],SPai *&spai,int group[],int &normalN,int &huNum,int &hasJiang){
	MAHJ *mahj = spai ->mahj;
	int color = mahj ->color;
	int value = mahj ->value;
	Card card = cards[color][value];
	int amount = card.amount;
	if(amount == 1)
	{
		if(hasJiang == 0)
		{
			hasJiang = 1;
			card.amount = 0;
			card.index = 0;
			normalN--;
			huNum--;
		}
		else{
			if(group[0]==0)
			{
				return -1;
			}
			card.amount = 0;
			card.index = 0;
			normalN--;
			huNum--;
			huNum--;
			group[0]--;
		}
	}
	else if(amount == 2)
	{
		if(hasJiang == 0)
		{
			hasJiang = 1;
			card.amount = 0;
			card.index = 0;
			normalN--;
			normalN--;
		}
		else{
			if(group[1]==0)
			{
				return -1;
			}
			card.amount = 0;
			card.index = 0;
			normalN--;
			normalN--;
			huNum--;
			group[1]--;
		}
	}
	else if(amount == 3)
	{
		if(group[2]==0)
		{
			return -1;
		}
		card.amount = 0;
		card.index = 0;
		normalN--;
		normalN--;
		normalN--;
		group[2]--;
	}
	if(huNum<0)return -1;
	TreeNode *node;
	newStruct(node);
	HAND *hand;
	newStruct(hand);
	node ->hand = hand;
	hand ->type = 0;
	hand ->num = amount;
	int i1;
	for(i1=0;i1<amount;i1++)
	{
		hand ->mahj[i1] = spai ->mahj; 
		deleteStruct(spai);
	}
	TreeNode *bnode;
	newStruct(bnode);
	if(g_root==NULL)
		g_root = bnode;
	bnode ->child[amount-1] = node;
	node ->parent = bnode;
	bnode = node;
	if(sumGroup(group)==0 && hasJiang)
	{
		saveRes(bnode);
	}
}


SPai *copySPai(SPai *start,SPai *deletes[],int num)
{
	SPai *ps = start;
	SPai *ss = NULL;
	SPai *se = NULL;
	while(ps)
	{
		int isDel = 0;
		int i = 0;
		for(i = 0;i<num;i++)
		{
			if(deletes[i] == ps)
			{
				isDel = 1;
				break;
			}
		}
		if(isDel==0)
		{
			SPai *pc = addStruct(ss,se);
			pc->mahj = ps->mahj;
		}
		ps = ps->next;
	}
}

Chunk *split(SPai *sp)
{
	Chunk *cs = NULL;
	Chunk *ce = NULL;
	SSPai *ss = NULL;
	SSPai *se = NULL;
	int valueS = -1;
	while(sp)
	{
		if(sp ->mahj ->value != valueS)
		{
			valueS = sp ->mahj ->value;
			SSPai *ssp = addStruct(ss,se);
			ssp ->sp = sp;
		}
		se ->ep = sp;
		sp = sp ->next;
	}
	SSPai *ssp = ss;
	while(ssp)
	{
		ssp ->num = getDistance(ssp ->sp ,ssp ->ep) + 1;
		ssp = ssp ->next;
	}
	ssp = ss;
	int valueC = -3;
	while(ssp)
	{
		if(ssp ->sp ->mahj ->value - valueC > 2)
		{
			valueC = ssp ->sp ->mahj ->value;
			Chunk *chunk = addStruct(cs,ce);
			chunk ->color = ssp ->sp ->mahj ->color;
			chunk ->si = ssp ->sp ->mahj ->value;
			chunk ->sp = ssp;
		}
		ce ->num += ssp->num;
		ce ->ep = ssp;
		ce ->ei = ssp ->sp ->mahj ->value;
		ssp = ssp ->next;
	}
	return cs;
}


int g_split()
{
	SSPai *sspai = g_sspaiS[3];
	int chunkNum = 0;
	while(sspai)
	{
		Chunk *chunk = addStruct(g_chunkS,g_chunkE);
		chunk ->color = sspai ->sp ->mahj ->color;
		chunk ->si = sspai ->sp ->mahj ->value;
		chunk ->ei = chunk ->si; 
		chunk ->sp = sspai;
		chunk ->ep = sspai;
		chunk ->num = sspai ->num;
		sspai = sspai ->next;
		chunkNum++;
	}
	int i = 0;
	for(i=0;i<3;i++)
	{
		sspai = g_sspaiS[i];
		int oldValue = -3;
		while(sspai)
		{
			int value = sspai ->sp ->mahj ->value;
			if(value - oldValue > 2)
			{
				Chunk *chunk = addStruct(g_chunkS,g_chunkE);
				chunk ->color = sspai ->sp ->mahj ->color;
				chunk ->si = sspai ->sp ->mahj ->value;
				chunk ->ei = chunk ->si; 
				chunk ->sp = sspai;
				chunk ->ep = sspai;
				chunk ->num = sspai ->num;
				chunkNum++;
			}
			else{
				g_chunkE ->ep = sspai;
				g_chunkE ->ei = sspai ->sp ->mahj ->value;
				g_chunkE ->num += sspai->num;
			}
			oldValue = value;
			sspai = sspai ->next;
		}
	}
	return chunkNum;
}


int addJKN(int &jkN,Unit *u)
{
	jkN ++;
	u->jkN ++;
}

int diff(int obj[],int src[],int type,int num=9)//差分
{
	memset(obj,-1,num);
	int i;
	if(type == 0)
	{
		for(i = 0;i<num;i++)
			obj[i] = src[i];
	}
	else if(type == 1)
	{
		for(i = 0;i<num-1;i++)//差分
			obj[i] = src[i+1] - src[i];
	}
	else if(type == 2)
	{
		for(i = 0;i<num-2;i++)//二阶差分
			obj[i] = src[i+2] + src[i] - 2*src[i+1];
	}
}

int product(int doc[],int vec1[],int vec2[],int num=9)//点积
{
	memset(doc,-1,num);
	int i;
	for(i=0;i<num;i++)
		doc[i] = vec1[i] * vec2[i];
	return 1;
}

int addUnit(SetLink *link, Chunk *pChunk, int value[], int front, int back, int &jkN)
{
	if(back < 0)
	{
		LinkSet *set = addStruct(link ->ss,link ->se); 
		Unit *u = newStruct(set->u);
		u ->type = 0;
		u ->sp = getPointer(pChunk ->sp ->sp, front);
		if(u ->type <2)
			addJKN(jkN,u);
	}
	else
	{
		LinkSet *set = addStruct(link ->ss,link ->se); 
		Unit *u = newStruct(set->u);
		u ->type = value[back] - value[front] + 1;
		u ->sp = getPointer(pChunk ->sp ->sp, front);
		if(u ->type <2)
			addJKN(jkN,u);
	}
}

int addLink(LinkSet *set, Chunk *pChunk, int value[], int front[], int back[], int num, int &jkN)
{
	SetLink *link = addStruct(set ->ls, set ->le);
	int i;
	for(i=0;i<num;i++)
	{
		addUnit(link, pChunk, value, front[i], back[i], jkN);
	}
}

int addTwoATwoSet(SetLink *link, Chunk *pChunk,int *value, int front[],int back[],int num, int &jkN)//2+2+...
{
	if(num==1)
	{
		addUnit(link, pChunk, value, front[0], back[0], jkN);
	}
	else if(num==2)
	{
		LinkSet *set = addStruct(link ->ss, link ->se);
		{
			addLink(set, pChunk, value, front, back, num, jkN);
		}
		if(value[front[0]] == value[back[0]] && value[front[1]] == value[back[1]] && value[front[1]] - value[front[0]] < 3)
		{
			SetLink *link = addStruct(set ->ls, set ->le);
			{
				addUnit(link, pChunk, value, front[0], front[1], jkN);
			}
		}
	}
	else if(num==3)
	{
		LinkSet *set = addStruct(link ->ss, link ->se);
		{
			{
				SetLink *link = addStruct(set ->ls, set->le);
				{
					addTwoATwoSet(link,pChunk,value,front,back,1,jkN);
					addTwoATwoSet(link,pChunk,value,front+1,back+1,num-1,jkN);
				}
			}
			{
				SetLink *link = addStruct(set ->ls, set->le);
				{
					addTwoATwoSet(link,pChunk,value,front+2,back+2,1,jkN);
					addTwoATwoSet(link,pChunk,value,front,back,num-1,jkN);
				}
			}
		}
	}
	else if(num>3)
	{
		LinkSet *set = addStruct(link ->ss, link ->se);
		{
			{
				SetLink *link = addStruct(set ->ls, set->le);
				{
					addTwoATwoSet(link,pChunk,value,front,back,1,jkN);
					addTwoATwoSet(link,pChunk,value,front+1,back+1,num-1,jkN);
				}
			}
			{
				SetLink *link = addStruct(set ->ls, set->le);
				{
					addTwoATwoSet(link,pChunk,value,front,back,2,jkN);
					addTwoATwoSet(link,pChunk,value,front+2,back+2,num-2,jkN);
				}
			}
		}
	}
}

int addArraySet(SetLink *link, Chunk *pChunk,int *value, int index[],int num, int &jkN)//2+2+...+1
{
	if(num==1)
	{
		addUnit(link, pChunk, value, index[0], -1, jkN);
	}
	else if(num==2)
	{
		addUnit(link, pChunk, value, index[0], index[1], jkN);
	}
	else if(num>2)
	{
		if(num%2 == 0)
		{
			int pairs = num/2;
			int i;
			int front[10];
			int back[10];
			for(i=0;i<pairs;i++)
			{
				front[i] = index[i*2];
				back[i] = index[i*2+1];
			}
			addTwoATwoSet(link,pChunk,value,front,back,pairs,jkN);
		}
		else
		{
			LinkSet *set = addStruct(link ->ss, link ->se);
			{
				int i = 0;
				while(i<num)
				{
					SetLink *link = addStruct(set ->ls, set->le);
					{
						addArraySet(link,pChunk,value,index+i,1,jkN);
						addArraySet(link,pChunk,value,index,i,jkN);
						addArraySet(link,pChunk,value,index+i+1,num-i-1,jkN);
					}
				}
			}
		}
	}
}


int build(Chunk *chunkS,SetLink *link,int huNumJ,int &jkN)
{
	Chunk *pChunk = chunkS;
	while(pChunk)
	{
		if(pChunk ->ei == pChunk ->si)
		{
			if(pChunk ->num==1 || pChunk ->num==4)
			{
				huNumJ-=2;
				jkN++;
			}
			if(pChunk ->num==2)
			{
				huNumJ-=1;
				jkN++;
			}
		}
		else
		{
			if(pChunk ->num==2)
				huNumJ-=1;
		}
		pChunk=pChunk ->next;
	}
	if(huNumJ < 0)
		return -1;
	if(huNumJ==0 && jkN == 0)
		return -1;

	int hj1 = huNumJ;
	pChunk = chunkS;
	while(pChunk)
	{
		int num = pChunk ->num;
		if(num>=3 && pChunk ->ei != pChunk ->si)
		{
			num = num%3;
			if(num==1)
			{
				huNumJ-=2;//必须用混补充为３的倍数，这是用的最少情况
			}
			else if(num==2)
			{
				huNumJ-=1;
			}
		}
		pChunk=pChunk ->next;
	}
	if(huNumJ<0)
		return -1;


	pChunk = chunkS;
	while(pChunk)
	{
		int isGu = 0;
		if(pChunk ->ei == pChunk ->si)
		{
			if(pChunk ->num !=3)
			{
				isGu = 1;
			}
		}
		else if(pChunk ->num==2)
		{
			isGu = 2;
		}
		else
		{
			pChunk=pChunk ->next;
		}
		if(isGu > 0)
		{
			LinkSet *set = addStruct(link ->ss,link ->se);
			Unit *u = newStruct(link->se->u);
			if(isGu == 1)
			{
				u ->type = pChunk ->num%3 - 1;
				u ->sp = pChunk ->sp ->sp;
			}
			else
			{
				SPai *p = pChunk ->sp ->sp;
				u ->type = p ->next ->mahj ->value - p ->mahj ->value + 1;
			}
			deleteStruct(pChunk);
		}
	}

	huNumJ = hj1;//还原为之前的混数，再重新检测
	pChunk = chunkS;
	//剩下的是连顺分拆问题
	while(pChunk)
	{
		int num = pChunk ->num;//牌的个数
		int dis = pChunk ->ei - pChunk ->si + 1;//跨度
		int value[9];
		memset(value,-1,sizeof(value));
		SPai *sp = pChunk ->sp ->sp;
		int i = 0;
		while(sp)
		{
			value[i++] = sp ->mahj ->value;
			sp = sp ->next;
		}
		int s_value[9];
		memset(s_value,-1,sizeof(s_value));
		int s_num[9];
		memset(s_num,0,sizeof(s_num));
		SSPai *ssp = pChunk ->sp;
		i = 0;
		while(ssp)
		{
			s_value[i] = ssp ->sp ->mahj ->value;
			s_num[i] = 1;
			SPai *sp = ssp ->sp;
			while(sp != ssp ->ep)
			{
				s_num[i] ++;
				sp = sp ->next;
			}
			i++;
			ssp = ssp ->next;
		}
		int sspN = i;//ssp的数目
		int dsv[9];
		diff(dsv,s_value,1);
		int ddsv[9];
		diff(ddsv,s_value,2);

		i=0;
		int sv = value[0];
		while(value[i]>-1)
		{
			value[i] = value[i] - sv; 
			i++;
		}

		i=0;
		while(s_value[i]>-1)
		{
			s_value[i] = s_value[i] - sv; 
			i++;
		}

		LinkSet *set = addStruct(link ->ss,link ->se);
		if(num==3)
		{
			if(value[1] == 1 && value[2] == 2)//3
			{
				//顺不处理
			}
			else//2+1
			{
				huNumJ -= 3;
				if(huNumJ < 0)
					return -1;
				{
					SetLink *link = addStruct(set ->ls, set ->le);
					{
						LinkSet *set = addStruct(link ->ss,link ->se); 
						Unit *u = newStruct(set->u);
						u ->type = 0;
						u ->sp = pChunk ->sp ->sp;
						addJKN(jkN,u);
					}
					{
						LinkSet *set = addStruct(link ->ss,link ->se); 
						Unit *u = newStruct(set->u);
						u ->type = s_value[2] - s_value[1] + 1;
						u ->sp = pChunk ->sp ->sp ->next;
						if(u ->type <2)
							addJKN(jkN,u);
					}
				}
				{
					SetLink *link = addStruct(set ->ls, set ->le);
					{
						LinkSet *set = addStruct(link ->ss,link ->se); 
						Unit *u = newStruct(set->u);
						u ->type = 0;
						u ->sp = pChunk ->ep ->sp;
						addJKN(jkN,u);
					}
					{
						LinkSet *set = addStruct(link ->ss,link ->se); 
						Unit *u = newStruct(set->u);
						u ->type = s_value[1] - s_value[0] + 1;
						u ->sp = pChunk ->sp ->sp;
						if(u ->type <2)
							addJKN(jkN,u);
					}
				}
			}
			if(huNumJ == 0 && jkN == 0)
			{
				return -1;
			}
			deleteStruct(pChunk);
		}
		else if(num==4)
		{
			huNumJ -= 2;
			if(huNumJ < 0)
			{
				return -1;
			}
			{
				SetLink *link = addStruct(set ->ls, set ->le);
				{
					LinkSet *set = addStruct(link ->ss,link ->se); 
					Unit *u = newStruct(set->u);
					u ->type = value[1] + 1;
					if(u ->type < 2)
						addJKN(jkN,u);
					u ->sp = pChunk ->sp ->sp;
				}
				{
					LinkSet *set = addStruct(link ->ss,link ->se); 
					Unit *u = newStruct(set->u);
					u ->type = value[3] - value[2] + 1;
					if(u ->type < 2)
						addJKN(jkN,u);
					u ->sp = pChunk ->sp ->sp;
				}
			}
			if(s_num[0] == 3 || s_num[0] == 4)
			{
				SetLink *link = addStruct(set ->ls, set ->le);
				{
					LinkSet *set = addStruct(link ->ss,link ->se); 
					Unit *u = newStruct(set->u);
					u ->type = 0;
					u ->sp = pChunk ->ep ->sp;
					addJKN(jkN,u);
				}
			}
			else if(s_num[1] == 3)
			{
				SetLink *link = addStruct(set ->ls, set ->le);
				{
					LinkSet *set = addStruct(link ->ss,link ->se); 
					Unit *u = newStruct(set->u);
					u ->type = 0;
					u ->sp = pChunk ->sp ->sp;
					addJKN(jkN,u);
				}
			}
			else
			{
				for(;i<2;i++)
				{
					if(dsv[i] == 1 && ddsv[i] == 0)
					{
						int snc[4];
						diff(snc,s_num,0,4);
						snc[i] --;
						snc[i+1] --;
						snc[i+2] --;
						int guIndex = -1; 
						int j;
						for(j=0;j<4;j++)
						{
							if(snc[j] == 1)
							{
								guIndex = j;
								break;
							}
						}
						SSPai *ssp = pChunk ->sp;
						while(guIndex-- > 0)
							ssp = ssp ->next; 
						SetLink *link = addStruct(set ->ls, set ->le);
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = 0;
							u ->sp = ssp ->sp;
							addJKN(jkN,u);
						}
					}
				}

				if(sspN==2)
				{
					SetLink *link = addStruct(set ->ls, set ->le);
					{
						LinkSet *set = addStruct(link ->ss,link ->se); 
						Unit *u = newStruct(set->u);
						u ->type = value[2] + 1;
						u ->sp = pChunk ->sp ->sp;
					}
				}
			}


			if(huNumJ == 0 && jkN == 0)
			{
				return -1;
			}
			deleteStruct(pChunk);
		}
		else if(num==5)
		{
			huNumJ -= 1;
			if(huNumJ < 0)
			{
				return -1;
			}
			if(s_num[0] == 3 || s_num[0] == 4)
			{
				SetLink *link = addStruct(set ->ls, set ->le);
				{
					LinkSet *set = addStruct(link ->ss,link ->se); 
					Unit *u = newStruct(set->u);
					u ->type = value[4] - value[3]+1;
					u ->sp = pChunk ->ep ->sp ->prev;
					if(u ->type < 2)
						addJKN(jkN,u);
				}
			}
			else if(s_num[2] == 3 || s_num[1] == 4)
			{
				SetLink *link = addStruct(set ->ls, set ->le);
				{
					LinkSet *set = addStruct(link ->ss,link ->se); 
					Unit *u = newStruct(set->u);
					u ->type = value[1]+1;
					u ->sp = pChunk ->sp ->sp;
					if(u ->type < 2)
						addJKN(jkN,u);
				}
			}
			else if(s_num[1] == 3)
			{
				if(s_value[2] > 2)
				{
					huNumJ -= 3;
					if(huNumJ < 0)
						return -1;
					SetLink *link = addStruct(set ->ls, set ->le);
					{
						SSPai *ssp = pChunk ->sp;
						LinkSet *set = addStruct(link ->ss,link ->se); 
						Unit *u = newStruct(set->u);
						u ->type = 0;
						u ->sp = ssp ->sp;
						addJKN(jkN,u);
					}
					{
						SSPai *ssp = pChunk ->sp;
						int gi = 2;
						while(gi-- > 0)
							ssp = ssp ->next; 
						LinkSet *set = addStruct(link ->ss,link ->se); 
						Unit *u = newStruct(set->u);
						u ->type = 0;
						u ->sp = ssp ->sp;
						addJKN(jkN,u);
					}
				}
			}
			else
			{
				int hasShun = 0;
				int i = 0;
				for(;i<3;i++)
				{
					if(dsv[i] == 1 && ddsv[i] == 0)
					{
						hasShun = 1;
						int snc[5];
						diff(snc,s_num,0,5);
						snc[i] --;
						snc[i+1] --;
						snc[i+2] --;
						int guIndex[2];
						int j;
						int k = 0;
						for(j=0;j<5;j++)
						{
							if(snc[j] == 2)
							{
								guIndex[0] = j;
								guIndex[1] = j;
								break;
							}
							else if(snc[j] == 1)
							{
								guIndex[k++] = j;
								continue;
							}
						}
						if(guIndex[1] - guIndex[0] <3)
						{
							SSPai *ssp = pChunk ->sp;
							int gi = guIndex[0];
							while(gi-- > 0)
								ssp = ssp ->next; 
							SetLink *link = addStruct(set ->ls, set ->le);
							{
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = s_value[guIndex[1]]- s_value[guIndex[0]] + 1;
								u ->sp = ssp ->sp;
								if(u ->type<2)
									addJKN(jkN,u);
							}
						}
						else
						{
							huNumJ -= 3;
							if(huNumJ < 0)
								return -1;
							SetLink *link = addStruct(set ->ls, set ->le);
							{
								SSPai *ssp = pChunk ->sp;
								int gi = guIndex[0];
								while(gi-- > 0)
									ssp = ssp ->next; 
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = 0;
								u ->sp = ssp ->sp;
								addJKN(jkN,u);
							}
							{
								SSPai *ssp = pChunk ->sp;
								int gi = guIndex[1];
								while(gi-- > 0)
									ssp = ssp ->next; 
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = 0;
								u ->sp = ssp ->sp;
								addJKN(jkN,u);
							}
						}
					}
				}
				if(hasShun == 0)
				{
					huNumJ -= 3;
					if(huNumJ < 0)
						return -1;
					SPai *sp = pChunk ->sp ->sp;
					//中间为孤
					{
						SetLink *link = addStruct(set ->ls, set ->le);
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = 0;
							u ->sp = sp ->next ->next;
							addJKN(jkN,u);
						}
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = value[1] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN,u);
						}
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = value[4] - value[3] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN,u);
						}
					}
					//右边为孤
					{
						SetLink *link = addStruct(set ->ls, set ->le);
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = 0;
							u ->sp = sp ->next ->next ->next ->next;
							addJKN(jkN,u);
						}
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = value[1] - value[0] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN,u);
						}
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = value[3] - value[2] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN,u);
						}
						if(value[3] - value[1] < 3 && value[2] - value[0] < 3)
						{
							SetLink *link = addStruct(set ->ls, set ->le);
							{
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = 0;
								u ->sp = sp ->next ->next ->next ->next;
								addJKN(jkN,u);
							}
							{
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = value[3] - value[1] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN,u);
							}
							{
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = value[2] - value[0] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN,u);
							}
						}
					}
					//左边为孤
					{
						SetLink *link = addStruct(set ->ls, set ->le);
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = 0;
							u ->sp = sp;
							addJKN(jkN,u);
						}
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = value[2] - value[1] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN,u);
						}
						{
							LinkSet *set = addStruct(link ->ss,link ->se); 
							Unit *u = newStruct(set->u);
							u ->type = value[4] - value[3] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN,u);
						}
						if(value[2] - value[1] < 3 && value[4] - value[3] < 3)
						{
							SetLink *link = addStruct(set ->ls, set ->le);
							{
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = 0;
								u ->sp = sp;
								addJKN(jkN,u);
							}
							{
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = value[2] - value[1] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN,u);
							}
							{
								LinkSet *set = addStruct(link ->ss,link ->se); 
								Unit *u = newStruct(set->u);
								u ->type = value[4] - value[3] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN,u);
							}
						}
					}
				}
			}
			if(huNumJ == 0 && jkN == 0)
			{
				return -1;
			}
			deleteStruct(pChunk);
		}
		else if(num == 6)
		{
			SPai *delSp[3];
			int delNum = 0;
			int i;
			for(i=0;i<6;i++)
			{
				if(s_num[i] >= 3)
				{
					SSPai *ssp = getPointer(pChunk ->sp ,i); 
					delSp[0] = ssp->sp;
					delSp[1] = ssp->sp->next;
					delSp[2] = ssp->sp->next->next;
					delNum = 3;
					break;
				}
				if(dsv[i] == 1 && ddsv[i] == 0)
				{
					SSPai *ssp = getPointer(pChunk ->sp ,i); 
					delSp[0] = ssp->sp;
					delSp[1] = ssp->next->sp;
					delSp[2] = ssp->next->next->sp;
					delNum = 3;
					break;
				}
			}
			int res = huNumJ;
			if(delNum > 0)
			{
				SPai *sp = copySPai(pChunk ->sp ->sp, delSp, 3);	
				Chunk *subChunk = split(sp);
				SetLink *link = addStruct(set ->ls, set ->le);
				res = build(subChunk,link,huNumJ,jkN);
				clearChunk(subChunk);
			}
			if(res<huNumJ)
			{
				huNumJ -= 3;
				if(huNumJ < 0)
					return -1;
				//2+2+2
				{
					SetLink *link = addStruct(set ->ls, set ->le);
					{
						{
							LinkSet *set = addStruct(link ->ss, link ->se);
							{
								int front[10];
								int back[10];
								char str[] = "4:5";
								int num = splitLinkStr(str,front,back);
								addLink(set, pChunk, value, front, back, num, jkN);
							}
						}
						{
							LinkSet *set = addStruct(link ->ss, link ->se);
							{
								int front[10];
								int back[10];
								char str[] = "0:1,2:3";
								int num = splitLinkStr(str,front,back);
								addLink(set, pChunk, value, front, back, num, jkN);
							}
						}
						if(value[1] == value[0] && value[3] == value[2])
						{
							int front[10];
							int back[10];
							char str[] = "1:2";
							int num = splitLinkStr(str,front,back);
							addLink(set, pChunk, value, front, back, num, jkN);
						}
					}
				}
				{
					SetLink *link = addStruct(set ->ls, set ->le);
					{
						{
							LinkSet *set = addStruct(link ->ss, link ->se);
							{
								int front[10];
								int back[10];
								char str[] = "0:1";
								int num = splitLinkStr(str,front,back);
								addLink(set, pChunk, value, front, back, num, jkN);
							}
						}
						{
							LinkSet *set = addStruct(link ->ss, link ->se);
							{
								int front[10];
								int back[10];
								char str[] = "2:3,4:5";
								int num = splitLinkStr(str,front,back);
								addLink(set, pChunk, value, front, back, num, jkN);
							}
							if(value[1] == value[0] && value[3] == value[2])
							{
								int front[10];
								int back[10];
								char str[] = "3:4";
								int num = splitLinkStr(str,front,back);
								addLink(set, pChunk, value, front, back, num, jkN);
							}
						}
					}
				}
			}
			if(huNumJ == 0 && jkN == 0)
			{
				return -1;
			}
			deleteStruct(pChunk);
		}
		else if(num >= 7)
		{
			if(huNumJ < num%3)
				return -1;
			SPai *delSp[3];
			int delNum = 0;
			int i;
			for(i=0;i<7;i++)
			{
				if(s_num[i] >= 3)
				{
					SSPai *ssp = getPointer(pChunk ->sp ,i); 
					delSp[0] = ssp->sp;
					delSp[1] = ssp->sp->next;
					delSp[2] = ssp->sp->next->next;
					delNum = 3;
					break;
				}
				if(dsv[i] == 1 && ddsv[i] == 0)
				{
					SSPai *ssp = getPointer(pChunk ->sp ,i); 
					delSp[0] = ssp->sp;
					delSp[1] = ssp->next->sp;
					delSp[2] = ssp->next->next->sp;
					delNum = 3;
					break;
				}
			}
			int res = -1;
			if(delNum > 0)
			{
				SPai *sp = copySPai(pChunk ->sp ->sp, delSp, 3);	
				Chunk *subChunk = split(sp);
				SetLink *link = addStruct(set ->ls, set ->le);
				res = build(subChunk,link,huNumJ,jkN);
				clearChunk(subChunk);
			}
			do
			{
				if(res < huNumJ)
				{
					huNumJ -= num/2;
					huNumJ -= (num%2)*2;
					if(huNumJ < res || huNumJ < 0)
						break;
					res = huNumJ;
					SetLink *link = addStruct(set ->ls, set ->le);
					int index[] = {0,1,2,3,4,5,6,7,8,9,10};
					addArraySet(link, pChunk,value, index, num, jkN);
				}
			}while(0);
			if(res < 0)
				return -1;
			if(res == 0 && jkN == 0)
			{
				return -1;
			}
			huNumJ = res;
			deleteStruct(pChunk);
		}
		else
			pChunk=pChunk ->next;
	}
	return huNumJ;
}

int checkHu(int huNum)
{
	int i;
	int huNumJ = huNum+1;//假定多加一个赖子,把将变成一个刻
	int jkN = 0;
	SetLink *link;
	newStruct(link);
	huNumJ = build(g_chunkS,link,huNumJ,jkN);
	printf("huNumJ:%d\n",huNumJ);
}


TreeNode *newTreeNode()
{
	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	if(node == NULL)
		return NULL;
	memset(node,0,sizeof(node));
	return node;
}

HAND *newHand()
{
	HAND *hand = (HAND *)malloc(sizeof(HAND));
	if(hand==NULL)
		return NULL;
	memset(hand,0,sizeof(HAND));
	return hand;
}

MAHJ *newMahj()
{
	MAHJ *mahj = (MAHJ *)malloc(sizeof(MAHJ));
	if(mahj==NULL)
		return NULL;
	memset(mahj,0,sizeof(MAHJ));
	return mahj;
}
		
int str_to_value(char Str_Hex[],MAHJ *pMahj)//传进字符串
{
    int i;
    int Pai=-1;
	if(strlen(Str_Hex)<2)
	{
		return -1;
	}

	for (i = 0; i < 2; i++)
	{
		if((!isgraph(Str_Hex[i]))) 
		{
			return -1;
		}

		if (!isdigit(Str_Hex[i]))
		{
			return -1;
		}
	}
	int color = Str_Hex[0] - '0';
	int value = Str_Hex[1] - '0';
	if(color>4){
		return -1;
	}
	if(color<3 && value == 9){
		return -1;	
	}

	if(color==3 && value > 6){//东南西北　白　发　中
		return -1;
	}

	Pai = color*9+ value;
	
	pMahj ->color = color;
	pMahj ->value = value;
	
    return Pai;
}

int setPaiFromMah(int mah[],int normalN,char Pai[])
{
	int i;
	for(i=0;i<34;i++)
	{
		Pai[i] = 0;
	}
	for(i=0;i<normalN;i++)
	{
		Pai[mah[i]]++;
	}
}

MAHJ value_to_mahj(int value)
{
	MAHJ mahj;
	mahj.color = value/9;
	mahj.value = value - mahj.color * 9;
	return mahj;
}

int mahj_to_value(MAHJ mahj)
{
	return mahj.color*9 + mahj.value;
}
