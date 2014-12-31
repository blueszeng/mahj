#include <stdio.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<ctype.h>
#include <stdlib.h>
#include "mahj4.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

int g_zong=0;
TreeNode *g_root = NULL;
NodeRes *g_resS=NULL;
NodeRes *g_resE=NULL;
SetLink *g_link=NULL;

MAHJ g_mahj[pNum];
MAHJ g_mahjs[4][9];

int g_init()
{
	memset(g_mahj,0,sizeof(g_mahj));
	memset(g_mahjs,0,sizeof(g_mahjs));
	int i;
	int j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<9;j++)
		{
			g_mahjs[i][j].value = j;
			g_mahjs[i][j].color = i;
		}
	}
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

int clearChunk(Chunk *chunkS)
{
	Chunk *pChunk = chunkS;
	if(pChunk == NULL) return 0;
	if(pChunk -> sp)
		clearStruct(pChunk->sp->sp);
	clearStruct(pChunk->sp);
	clearStruct(chunkS);
}

int clearSetLink(SetLink *&link)
{
	if(link == NULL) return 0;
	LinkSet *set = link->ss;
	while(set)
	{
		clearLinkSet(set);
	}
	deleteStruct(link);
}

int clearLinkSet(LinkSet *&set)
{
	if(set == NULL)return 0;
	if(set->u)
	{
		free(set->u);
		set->u = NULL;
	}
	else
	{
		SetLink *link = set->ls;
		while(link)
		{
			clearSetLink(link);
		}
	}
	deleteStruct(set);
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
	g_init();
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
		sortP(mah,g_mahj);
		int huNum = getHuNum(mah);
		printf("huNum:%d\n",huNum);
		checkHu(huNum);
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


int saveGu(Card cards[4][9],SPai *&spai,int group[],int &normalN,int &huNum,int &hasJiang)
{
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


SPai *copySPai(SPai *start,SPai *end,SPai *deletes[],int sdi,int num)
{
	SPai *ps = start;
	SPai *ss = NULL;
	SPai *se = NULL;
	while(ps != end)
	{
		int isDel = 0;
		int i = 0;
		for(i = 0;i<num;i++)
		{
			if(deletes[sdi+i] == ps)
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
	return ss;
}

Chunk *splitSubChunk(SPai *sp)
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
			Chunk *chunk = addStruct(cs,ce);
			chunk ->color = ssp ->sp ->mahj ->color;
			chunk ->si = ssp ->sp ->mahj ->value;
			chunk ->sp = ssp;
		}
		valueC = ssp ->sp ->mahj ->value;
		ce ->num += ssp->num;
		ce ->ep = ssp;
		ce ->ei = ssp ->sp ->mahj ->value;
		ssp = ssp ->next;
	}
	return cs;
}


int splitChunk(Chunk *&chunkS,int normalNum)
{
	Chunk *chunkE = NULL;
	SPai *sps = NULL;
	SPai *spe = NULL;
	SSPai *ssps = NULL;
	SSPai *sspe = NULL;
	int i = 0;
	for(i=0;i<normalNum;i++)
	{
		MAHJ* mahj = &(g_mahj[i]);
		int color = mahj ->color;
		SPai *spai = addStruct(sps,spe);
		spai ->mahj=mahj;
	}
	SPai *sp = sps;
	int oldValue = -1;
	int oldColor = -1;
	while(sp)
	{
		int value = sp ->mahj ->value;
		int color = sp ->mahj ->color;
		if(value!=oldValue || color != oldColor)
		{
			SSPai *ssp = addStruct(ssps,sspe);
			ssp ->sp = sp;
			ssp ->ep = sp;
			ssp ->num = 1;
		}
		else
		{
			sspe ->ep = sp;
			sspe ->num ++;
		}
		oldValue = value;
		oldColor = color;
		sp = sp ->next;
	}
	int chunkNum = 0;
	SSPai *ssp = ssps;

	oldValue = -3;
	oldColor = -1;
	while(ssp)
	{
		int value = ssp ->sp ->mahj ->value;
		int color = ssp ->sp ->mahj ->color;
		if(color != oldColor || color == 3 || value - oldValue > 2)
		{
			Chunk *chunk = addStruct(chunkS,chunkE);
			chunk ->color = ssp ->sp ->mahj ->color;
			chunk ->si = ssp ->sp ->mahj ->value;
			chunk ->ei = chunk ->si; 
			chunk ->sp = ssp;
			chunk ->ep = ssp;
			chunk ->num = ssp ->num;
			chunkNum++;
		}
		else{
			chunkE ->ep = ssp;
			chunkE ->ei = ssp ->sp ->mahj ->value;
			chunkE ->num += ssp ->num;
		}
		oldValue = value;
		oldColor = color;
		ssp = ssp ->next;
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
		SPai *sp = getPointer(pChunk ->sp ->sp, front);
		u ->mahj = sp ->mahj;
		if(u ->type <2)
			addJKN(jkN,u);
	}
	else
	{
		LinkSet *set = addStruct(link ->ss,link ->se); 
		Unit *u = newStruct(set->u);
		u ->type = value[back] - value[front] + 1;
		SPai *sp = getPointer(pChunk ->sp ->sp, front);
		u ->mahj = sp ->mahj;
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
					i++;
				}
			}
		}
	}
}


int build(Chunk *chunkS,SetLink *link,int huNumJ,int &jkN,int height)
{
	Chunk *pChunk = chunkS;
	while(pChunk)
	{
		if(pChunk ->ei == pChunk ->si)
		{
			if(pChunk ->num==1 || pChunk ->num==4)
			{
				huNumJ -=2;
				jkN++;
			}
			if(pChunk ->num==2)
			{
				huNumJ -=1;
				jkN++;
			}
		}
		else
		{
			if(pChunk ->num==2)
				huNumJ -=1;
		}
		pChunk=pChunk ->next;
	}
	if(huNumJ < 0)
	{
		clearChunk(chunkS);
		return -1;
	}

	/*
	if(huNumJ==0 && jkN == 0)
	{
		clearChunk(chunkS);
		return -1;
	}
	*/

	int hj1 = huNumJ;
	int jkN1 = jkN;

	pChunk = chunkS;
	while(pChunk)
	{
		int num = pChunk ->num;
		if(num>=3 && pChunk ->ei != pChunk ->si)
		{
			num = num%3;
			if(num==1)
			{
				huNumJ -=2;//必须用混补充为３的倍数，这是用的最少情况
			}
			else if(num==2)
			{
				huNumJ -=1;
			}
		}
		pChunk=pChunk ->next;
	}
	if(huNumJ<0)
	{
		clearChunk(chunkS);
		return -1;
	}

	pChunk = chunkS;
	while(pChunk)
	{
		int isGu = 0;
		if(pChunk ->ei == pChunk ->si)
		{
			isGu = pChunk ->num %3;
		}
		else if(pChunk ->num==2)
		{
			isGu = 2;
		}
		if(isGu > 0)
		{
			if(isGu == 1)
			{
				int value[1];
				value[0] = pChunk->sp->sp->mahj->value;
				int index[] = {0};
				addArraySet(link,pChunk,value,index,1,jkN);
			}
			else
			{
				int value[2];
				value[0] = pChunk->sp->sp->mahj->value;
				value[1] = pChunk->sp->sp->next->mahj->value;
				if(value[0] == 2 && value[1] == 3)
				{
					printf("Debug...\n");
					travel(g_link);
				}
				int index[] = {0,1};
				addArraySet(link,pChunk,value,index,2,jkN);
				travel(g_link);
			}
		}
		pChunk = pChunk ->next;
	}

	huNumJ = hj1;//还原为之前的混数，再重新检测
	jkN = jkN1;
	pChunk = chunkS;
	//剩下的是连顺分拆问题
	while(pChunk)
	{
		int num = pChunk ->num;//牌的个数
		int dis = pChunk ->ei - pChunk ->si + 1;//跨度
		if(dis == 1 || num == 2)
		{
			pChunk = pChunk->next;
			continue;//前面已经处理过了
		}
		int jn = 0;
		if(num%3 == 1)
			jn = 2;
		else if(num%3 == 2)
			jn = 1;
		if(huNumJ < jn)
		{
			clearChunk(chunkS);
			return -1;
		}
		int value[14];
		memset(value,-1,sizeof(value));
		SPai *sp = pChunk ->sp ->sp;
		int i = 0;
		while(sp)
		{
			value[i++] = sp ->mahj ->value;
			if(i==num)
				break;
			sp = sp ->next;
		}
		int s_value[9];
		memset(s_value,-1,sizeof(s_value));
		int s_num[9];
		memset(s_num,0,sizeof(s_num));
		SSPai *ssp = pChunk ->sp;
		i = 0;
		int j = 0;
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
			j+= s_num[i];
			if(j >= num)
				break;
			i++;
			ssp = ssp ->next;
		}
		int sspN = i;//ssp的数目
		int dsv[9];
		memset(dsv,-1,sizeof(dsv));
		diff(dsv,s_value,1);
		int ddsv[9];
		memset(ddsv,-1,sizeof(dsv));
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
		int keIndex[9];
		memset(keIndex,0,sizeof(keIndex));
		int kei = 0;
		int keIndex_[9];
		memset(keIndex_,0,sizeof(keIndex_));
		for(i=0;i<9;i++)
		{
			if(s_num[i] >= 3)
			{
				keIndex[kei++] = i; 
				keIndex_[i] = 1;
			}
		}
		int shunIndex[7];
		memset(shunIndex,0,sizeof(shunIndex));
		int shuni = 0;
		int shunIndex_[7];
		memset(shunIndex_,0,sizeof(shunIndex_));
		for(i=0;i<7;i++)
		{
			if(dsv[i] == 1 && ddsv[i] == 0)
			{
				shunIndex[shuni++] = i;
				shunIndex_[i] = 1;
			}
		}
		SPai *delSp[30];
		memset(delSp,0,sizeof(delSp));
		int delNum = 0;
		if(shuni > 0)
		{
			int shunei = shunIndex[0];
			while(shunIndex_[++shunei] > 0 && shunei < 7);
			for(i=shunIndex[0];i<shunei;i++)
			{
				SSPai *ssp = getPointer(pChunk ->sp ,i); 
				delSp[delNum++] = ssp->sp;
				delSp[delNum++] = ssp->next->sp;
				delSp[delNum++] = ssp->next->next->sp;
			}
			for(i=shunIndex[0];i<shunei+2;i++)
			{
				if(keIndex_[i] == 1)
				{
					SSPai *ssp = getPointer(pChunk ->sp ,i); 
					delSp[delNum++] = ssp->sp;
					delSp[delNum++] = ssp->sp->next;
					delSp[delNum++] = ssp->sp->next->next;
				}
			}
		}
		else if(kei >0)
		{
			SSPai *ssp = getPointer(pChunk ->sp ,keIndex[0]); 
			delSp[delNum++] = ssp->sp;
			delSp[delNum++] = ssp->sp->next;
			delSp[delNum++] = ssp->sp->next->next;
		}
		int res[10];
		memset(res,-1,sizeof(res));
		int ri = 0;
		SetLink *links[10];
		while(delNum > 0)
		{
			SPai *sp = copySPai(pChunk ->sp ->sp,pChunk->ep->ep->next, delSp,ri*3, 3);	
			Chunk *subChunk = splitSubChunk(sp);
			addStruct(set ->ls, set ->le);
			links[ri] = (set->le);
			res[ri] = build(subChunk,set->le,huNumJ,jkN,height+1);
			ri++;
			delNum -= 3;
		}
		if(height == 0)
		{
			printf("hi\n");
		}
		int resMax = -1;
		for(i=0;i<ri;i++)
		{
			if(res[i] > resMax)
				resMax = res[i];
		}
		if(resMax >= 0)
		{
			printf("res: ");
			for(i=0;i<ri;i++)
			{
				printf("%d ",res[i]);
				if(res[i] < resMax)
				{
					clearSetLink(links[i]);
				}
			}
			printf("\n");
		}
		do
		{
			printf("huNum:%d,num:%d\n",huNumJ,num);
			if(num == 8)
			{
				printf("huNum:%d,num:%d\n",huNumJ,num);
			}
			huNumJ -= num/2;
			huNumJ -= (num%2)*2;
			if(huNumJ < resMax || huNumJ < 0)
			{
				huNumJ = resMax;
				break;
			}
			SetLink *link = addStruct(set ->ls, set ->le);
			int index[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};
			addArraySet(link, pChunk,value, index, num, jkN);
		}while(0);
		if(huNumJ < 0)
		{
			clearChunk(chunkS);
			return -1;
		}
		/*
		if(huNumJ == 0 && jkN == 0)
		{
			clearChunk(chunkS);
			return -1;
		}
		*/
		pChunk = pChunk ->next;
	}
	clearChunk(chunkS);
	return huNumJ;
}

HuSetLink *createHuSetLink(SetLink *link)
{
	HuSetLink *huLink = NULL;
	newStruct(huLink);
	LinkSet *set = link ->ss;
	while(set)
	{
		HuLinkSet *huSet = createHuLinkSet(set);
		insertStruct(huSet,huLink->ss,huLink->se);
		huSet->parent = huLink;
		huLink->jkN += huSet->jkN;
		set = set ->next;
	}
	return huLink;
}

HuLinkSet *createHuLinkSet(LinkSet *set)
{
	HuLinkSet *huSet = NULL;
	newStruct(huSet);
	if(set->u)
	{
		Unit *u = NULL; 
		newStruct(u);
		u->mahj = set->u->mahj;
		u->jkN = set->u->jkN;
		u->type = set->u->type;
		huSet->u  = u;
		if(huSet->u->type < 2)
			huSet->jkN = 1;
	}
	else
	{
		setLink *link = set ->ls;
		while(link)
		{
			HuSetLink *huLink = createHuSetLink(link);
			insertStruct(huLink,huSet->ls,huSet->le);
			huLink ->parent = huSet;
			if(huLink->jkN > huSet->jkN)
				huSet->jkN = huLink->jkN;
			link = link->next;
		}
	}
	return huSet;
}


int getOutJKN(HuSetLink *link)
{
	if(link == NULL) return -1;
	HuLinkSet *set = link->parent;
	while(set)
	{
		if(link->jkN == 1)return 1;
		if(set->jkN == 1 && link->jkN == 0)
			return 0;
		link = set->parent;
		set = link->parent;
	}
	return 1;
}

int addHuPai(SPai *&ss,SPai *&se,int color,int value)
{
	MAHJ *mahj = getMahjPointer(color,value);
	if(mahj == NULL) return -1;
	int havePai = 0;
	SPai *ps = ss;
	if(se)
	{
		while(ps!=se->next)
		{
			if(ps->mahj == mahj)
				havePai = 1;
			ps = ps->next;
		}
	}
	if(havePai == 0)
	{
		SPai *sp = addStruct(ss,se); 
		sp -> mahj = mahj;
	}
}

int addHuPai(SPai *&ss,SPai *&se,SPai *sp)
{
	addHuPai(ss,se,sp->mahj->color,sp->mahj->value);
}


MAHJ *getMahjPointer(int color,int value)
{
	if(color < 0 || color > 3)
		return NULL;
	if(value < 0 || value > 8)
		return NULL;
	if(color == 3 && value > 5)
		return NULL;
	return &(g_mahjs[color][value]);
}


int travelHu(HuSetLink *link,int jkN)
{
	HuLinkSet *set = link ->ss;
	while(set)
	{
		Unit *u = set->u;
		if(u)
		{
			MAHJ *mahj = u->mahj;
			printf("color:%d value:%d type:%d\n",mahj->color,mahj->value,u->type);
			if(u->type == 0)
			{
				addHuPai(link->hs,link->he,mahj->color,mahj->value);
				if(jkN > 1)
				{
					addHuPai(link->hs,link->he,mahj->color,mahj->value+1);
					addHuPai(link->hs,link->he,mahj->color,mahj->value+2);
					addHuPai(link->hs,link->he,mahj->color,mahj->value-1);
					addHuPai(link->hs,link->he,mahj->color,mahj->value-2);
				}
			}
			else if(u->type == 1 && jkN > 1)
				addHuPai(link->hs,link->he,mahj->color,mahj->value);
			else if(u->type == 2)
			{
				if(jkN > 1 || getOutJKN(link) == 1)
				{
					addHuPai(link->hs,link->he,mahj->color,mahj->value+2);
					addHuPai(link->hs,link->he,mahj->color,mahj->value-1);
				}
			}
			else if(u->type == 3)
			{
				if(jkN > 1 || getOutJKN(link) == 1)
				{
					addHuPai(link->hs,link->he,mahj->color,mahj->value+1);
				}
			}
		}
		else
		{
			travelHu(set,jkN);
			SPai *ss = set ->hs;
			SPai *se = set ->he;
			if(se)
			{
				while(ss!=se->next)
				{
					addHuPai(link->hs,link->he,ss);
					ss = ss ->next;
				}
			}
		}
		set = set ->next;
	}
}

int printSPai(SPai *ss)
{
	while(ss)
	{
		printf("hu:%d:%d\n",ss->mahj->color,ss->mahj->value);
		ss = ss ->next;
	}
}


int travelHu(HuLinkSet *set,int jkN)
{
	HuSetLink *link = set ->ls;
	while(link)
	{
		travelHu(link,jkN);
		SPai *ss = link ->hs;
		SPai *se = link ->he;
		if(se)
		{
			while(ss!=se->next)
			{
				addHuPai(set->hs,set->he,ss);
				ss = ss ->next;
			}
		}
		link = link -> next;
	}
}

int travel(SetLink *link)
{
	if(link==NULL)return -1;
	LinkSet *set = link ->ss;
	while(set)
	{
		Unit *u = set->u;
		if(u)
		{
			MAHJ *mahj = u->mahj;
			printf("color:%d value:%d type:%d\n",mahj->color,mahj->value,u->type);
		}
		else
		{
			travel(set);
		}
		set = set ->next;
	}
}

int travel(LinkSet *set)
{
	if(set==NULL) return -1;
	SetLink *link = set->ls;
	while(link)
	{
		travel(link);
		link = link->next;
	}
}

int checkHu(int huNum)
{
	Chunk *pChunk = NULL;
	int n = splitChunk(pChunk,pNum - huNum);
	printf("chunkNum:%d\n",n);
	if(n>5)//分块
	{
		printf("there is too many chunks to huPai\n");
		return -1;
	}
	int i;
	int huNumJ = huNum+1;//假定多加一个赖子,把将变成一个刻
	int jkN = 0;
	SetLink *link;
	newStruct(link);
	g_link = link;
	huNumJ = build(pChunk,link,huNumJ,jkN,0);
	travel(link);
	HuSetLink *hLink = createHuSetLink(link);
	clearSetLink(link);
	printf("huNumJ:%d,jkN:%d\n",huNumJ,hLink->jkN);
	if(huNumJ > 2)
	{
		printf("全胡\n");
	}
	else if(huNumJ < 0 || hLink->jkN == 0)
	{
		printf("没胡\n");
	}
	else
	{
		travelHu(hLink,hLink->jkN);
		printSPai(hLink->hs);
	}
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
