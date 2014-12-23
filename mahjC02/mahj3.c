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
JIANG g_jiang;
TreeNode *g_root;
SPai *g_spai[4];
SPai *g_spaiEnd[4];
SSPai *g_sspai[4];
SSPai *g_sspaiEnd[4];
MAHJ g_mahj[pNum];
Chunk *g_chunk=NULL;
Chunk *g_chunkEnd=NULL;
NodeRes *g_result=NULL;
NodeRes *g_resultEnd=NULL;
int g_huNum = 0;
int g_normalNum = 0;

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
		checkHu(mah,g_mahj,cards);
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
	for(i=0;i<4;i++)
	{
		memset(g_spai,0,sizeof(g_spai));
		memset(g_spaiEnd,0,sizeof(g_spaiEnd));
		memset(g_sspai,0,sizeof(g_sspai));
		memset(g_sspaiEnd,0,sizeof(g_sspaiEnd));
	}

	for(i=0;i<g_normalNum;i++)
	{
		MAHJ* mahj = &(g_mahj[i]);
		int color = mahj->color;
		SPai *spai;
		newStruct(&spai);
		spai->mahj=mahj;
		spai->next=NULL;
		spai->prev=g_spaiEnd[color];
		if(g_spai[color]==0)
		{
			g_spai[color] = spai;
		}
		else{
			g_spaiEnd[color]->next = spai;
		}
		g_spaiEnd[color] = spai;
	}
	for(i=0;i<4;i++)
	{
		SPai *p = g_spai[i];
		int value0 = -1;
		while(p)
		{
			int value = p->mahj->value;
			printf("---value:%d\n",value);
			if(value!=value0)
			{
				SSPai *sspai;
				newStruct(&sspai);
				sspai->sp = p;
				sspai->ep = p;
				sspai->next = NULL;
				sspai->prev = g_sspaiEnd[i];
				if(g_sspai[i]==0)
				{
					g_sspai[i] = sspai;
				}
				else{
					g_sspaiEnd[i]->next = sspai;
				}
				g_sspaiEnd[i] = sspai;
			}
			else
			{
				g_sspaiEnd[i]->ep = p;
			}
			value0 = value;
			p = p->next;
		}
	}
}

int initRes()
{
	g_result = NULL;
	g_resultEnd = g_result;
}

int freeRes()
{
	NodeRes *node = g_result;
	int i = 0;
	while(node)
	{
		NodeRes *nodeN = NULL;
		if(node->next)
			nodeN = node->next;
		if(node)
		{
			i++;
			free(node);
		}
		node = nodeN;
	}
	g_result = NULL;
	g_resultEnd = NULL;
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
		j = j + freeTreeNode(node->child[i]);
		node->child[i] = NULL;
	}
	if(node->hand)
	{
		int i;
		for(i=0;i<3;i++)
		{
			free(node->hand->mahj[i]);
			node->hand->mahj[i] = NULL;
		}
		free(node->hand);
		node->hand = NULL;
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
	nodeRes->node = pNode;
	nodeRes->next = NULL;
	nodeRes->prev = g_resultEnd;
	if(g_result == NULL)
	{
		g_result = nodeRes;
	}
	else
	{
		g_resultEnd ->next = nodeRes;
	}
	g_resultEnd = nodeRes;
}


int saveGu(Card cards[4][9],SPai *&spai,int group[],int &normalN,int &huNum,int &hasJiang){
	MAHJ *mahj = spai->mahj;
	int color = mahj->color;
	int value = mahj->value;
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
	newStruct(&node);
	HAND *hand;
	newStruct(&hand);
	node->hand = hand;
	hand->type = 0;
	hand->num = amount;
	int i1;
	for(i1=0;i1<amount;i1++)
	{
		hand->mahj[i1] = spai->mahj; 
		deleteStruct(spai);
	}
	TreeNode *bnode;
	newStruct(&bnode);
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




int g_split()
{
	SSPai *sspai = g_sspai[3];
	g_chunk = NULL;
	g_chunkEnd = NULL;
	int chunkNum = 0;
	while(sspai)
	{
		Chunk *chunk;
		newStruct(&chunk);
		chunk->color = sspai->sp->mahj->color;
		chunk->si = sspai->sp->mahj->value;
		chunk->ei = chunk->si; 
		chunk->sp = sspai;
		chunk->ep = sspai;
		chunk->next = NULL;
		chunk->prev = g_chunkEnd;
		chunk->num = sspai->ep - sspai->sp + 1;
		if(g_chunk==NULL)
		{
			g_chunk = chunk;
		}
		else{
			g_chunkEnd ->next = chunk;
		}
		g_chunkEnd = chunk;
		sspai = sspai->next;
		chunkNum++;
	}
	int i = 0;
	for(i=0;i<3;i++)
	{
		sspai = g_sspai[i];
		int value0 = -2;
		int num = 0;
		while(sspai)
		{
			int value = sspai->sp->mahj->value;
			printf("value0:%d,value:%d\n",value0,value);
			num += sspai->ep - sspai->sp +1;
			if(value - value0 > 1)
			{
				if(g_chunkEnd!=NULL)
					g_chunkEnd->num = num;
				num=0;
				Chunk *chunk;
				newStruct(&chunk);
				chunk->color = sspai->sp->mahj->color;
				chunk->si = sspai->sp->mahj->value;
				chunk->ei = chunk->si; 
				chunk->sp = sspai;
				chunk->ep = sspai;
				chunk->next = NULL;
				chunk->prev = g_chunkEnd;
				if(g_chunk==NULL)
				{
					g_chunk = chunk;
				}
				else{
					g_chunkEnd ->next = chunk;
				}
				g_chunkEnd = chunk;
				chunkNum++;
			}
			else{
				g_chunkEnd->ep = sspai;
				g_chunkEnd->ei = sspai->sp->mahj->value;
			}
			value0 = value;
			sspai = sspai->next;
		}
		g_chunkEnd->num = num;
	}
	return chunkNum;
}

int build(Card cards[4][9],int group[],int normalN,int huNum,int hasJiang)
{
	int i;
	int guN = 0;//孤
	int guDN = 0;//孤对
	int guLN = 0;//孤连(两个相连的牌)
	int huNumJ = huNum+1;//假定多加一个赖子,把将变成一个刻
	int keN = 0;//刻的数
	Chunk *pChunk = g_chunk;
	while(pChunk)
	{
		if(pChunk->ei == pChunk->si)
		{
			if(pChunk->sp->sp == pChunk->sp->ep)
				guN++;
			if(pChunk->sp->sp == pChunk->sp->ep - 1)
				guDN++;
		}
		else
		{
			if(pChunk->num==2)
				guLN++;
		}
		pChunk=pChunk->next;
	}
	if(guN>2)
		return -1;//孤的数目大于２个是不能胡的
	if(huNum==0)
	{
		if(guN>0)
			return -1;
		if(guLN>0)
			return -1;
	}
	else
	{
		if(guN==2)
			return -1;
		while(guN>0)
		{
			if(hasJiang == 0)
			{
				hasJiang = 1;
				huNum--;
			}
			else
				huNum-=2;
			guN--;
			if(huNum<0)
				return -1;
		}
		while(guDN>0)
		{
			if(hasJiang == 0)
			{
				hasJiang = 1;
			}
			else
				huNum--;
			guDN--;
			if(huNum<0)
				return -1;
		}
		while(guLN>0)
		{
			huNum--;
			guLN--;
			if(huNum<0)
				return -1;
		}
	}


	if(hasJiang == 0)
	{
	}
	pChunk = g_chunk;
	while(pChunk)
	{
		if(pChunk->num==4)
		{
		}
		pChunk=pChunk->next;
	}

	//孤用混配对完了
	pChunk = g_chunk;
	while(pChunk)
	{
		if(pChunk->ei == pChunk->si)
		{
			saveGu(cards,pChunk->sp->sp,group,normalN,huNum,hasJiang);
			deleteStruct(pChunk);
		}
		else
			pChunk=pChunk->next;
	}

	//剩下的是连顺分拆问题
	pChunk = g_chunk;
	while(pChunk)
	{
		if(pChunk->ei == pChunk->si)
		{
		}
		else
			pChunk=pChunk->next;
	}
	
}

int checkHu(int mah[],MAHJ mahj[],Card cards[4][9])
{
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
	
	pMahj->color = color;
	pMahj->value = value;
	
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
