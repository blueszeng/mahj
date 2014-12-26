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
CUnitsSet *g_unitsSetS = NULL;
CUnitsSet *g_unitsSetE = NULL;
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
		int oldValue = -2;
		while(sspai)
		{
			int value = sspai ->sp ->mahj ->value;
			if(value - oldValue > 1)
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


int addJKN(int &jkN)
{
	jkN ++;
	g_unitsSetE ->cue ->jkN ++;
	g_unitsSetE ->jkN ++;
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


int build(Chunk *chunkS,int huNumJ,int jkN)
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


	int addGu = 0;
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
			if(addGu == 0)
			{
				addGu = 1;
				addStruct(g_unitsSetS,g_unitsSetE);
				addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
			}
			CUnits *cu = g_unitsSetE ->cue;
			Unit *u = addStruct(cu ->us,cu ->ue);
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
				addStruct(g_unitsSetS,g_unitsSetE);
				{
					addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
					{
						Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
						u ->type = 0;
						u ->sp = pChunk ->sp ->sp;
						addJKN(jkN);
					}
					{
						Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
						u ->type = s_value[2] - s_value[1] + 1;
						u ->sp = pChunk ->sp ->sp ->next;
						if(u ->type <2)
							addJKN(jkN);
					}
				}
				{
					addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
					{
						Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
						u ->type = 0;
						u ->sp = pChunk ->ep ->sp;
						addJKN(jkN);
					}
					{
						Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
						u ->type = s_value[1] - s_value[0] + 1;
						u ->sp = pChunk ->sp ->sp;
						if(u ->type <2)
							addJKN(jkN);
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
			addStruct(g_unitsSetS,g_unitsSetE);
			{
				addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
				{
					Unit *uL = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
					uL ->type = value[1] + 1;
					if(uL ->type < 2)
						addJKN(jkN);
					uL ->sp = pChunk ->sp ->sp;
				}
				{
					Unit *uL = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
					uL ->type = value[3] - value[2] + 1;
					if(uL ->type < 2)
						addJKN(jkN);
					uL ->sp = pChunk ->sp ->sp;
				}
			}
			if(s_num[0] == 3 || s_num[0] == 4)
			{
				addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
				{
					Unit *uG = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
					uG ->type = 0;
					uG ->sp = pChunk ->ep ->sp;
					addJKN(jkN);
				}
			}
			else if(s_num[1] == 3)
			{
				addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
				{
					Unit *uG = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
					uG ->type = 0;
					uG ->sp = pChunk ->sp ->sp;
					addJKN(jkN);
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
						addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = 0;
							u ->sp = ssp ->sp;
							addJKN(jkN);
						}
					}
				}

				if(sspN==2)
				{
					addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
					{
						Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
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
			addStruct(g_unitsSetS,g_unitsSetE);
			if(s_num[0] == 3 || s_num[0] == 4)
			{
				addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
				Unit *u= addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
				u ->type = value[4] - value[3]+1;
				u ->sp = pChunk ->ep ->sp ->prev;
				if(u ->type < 2)
					addJKN(jkN);
			}
			else if(s_num[2] == 3 || s_num[1] == 4)
			{
				addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
				Unit *u= addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
				u ->type = value[1]+1;
				u ->sp = pChunk ->sp ->sp;
				if(u ->type < 2)
					addJKN(jkN);
			}
			else if(s_num[1] == 3)
			{
				if(s_value[2] > 2)
				{
					huNumJ -= 3;
					if(huNumJ < 0)
						return -1;
					addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
					{
						SSPai *ssp = pChunk ->sp;
						Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
						u ->type = 0;
						u ->sp = ssp ->sp;
						addJKN(jkN);
					}
					{
						SSPai *ssp = pChunk ->sp;
						int gi = 2;
						while(gi-- > 0)
							ssp = ssp ->next; 
						Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
						u ->type = 0;
						u ->sp = ssp ->sp;
						addJKN(jkN);
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
							addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = s_value[guIndex[1]]- s_value[guIndex[0]] + 1;
								u ->sp = ssp ->sp;
								if(u ->type<2)
									addJKN(jkN);
							}
						}
						else
						{
							huNumJ -= 3;
							if(huNumJ < 0)
								return -1;
							addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
							{
								SSPai *ssp = pChunk ->sp;
								int gi = guIndex[0];
								while(gi-- > 0)
									ssp = ssp ->next; 
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = 0;
								u ->sp = ssp ->sp;
								addJKN(jkN);
							}
							{
								SSPai *ssp = pChunk ->sp;
								int gi = guIndex[1];
								while(gi-- > 0)
									ssp = ssp ->next; 
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = 0;
								u ->sp = ssp ->sp;
								addJKN(jkN);
							}
						}
					}
				}
				if(hasShun == 0)
				{
					SPai *sp = pChunk ->sp ->sp;
					//中间为孤
					{
						addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = 0;
							u ->sp = sp ->next ->next;
							addJKN(jkN);
						}
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = value[1] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN);
						}
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = value[4] - value[3] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN);
						}
					}
					//右边为孤
					{
						addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = 0;
							u ->sp = sp ->next ->next ->next ->next;
							addJKN(jkN);
						}
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = value[1] - value[0] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN);
						}
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = value[3] - value[2] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN);
						}
						if(value[3] - value[1] < 3 && value[2] - value[0] < 3)
						{
							addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = 0;
								u ->sp = sp ->next ->next ->next ->next;
								addJKN(jkN);
							}
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = value[3] - value[1] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN);
							}
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = value[2] - value[0] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN);
							}
						}
					}
					//左边为孤
					{
						addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = 0;
							u ->sp = sp;
							addJKN(jkN);
						}
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = value[2] - value[1] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN);
						}
						{
							Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
							u ->type = value[4] - value[3] + 1;
							u ->sp = sp;
							if(u ->type <2)
								addJKN(jkN);
						}
						if(value[2] - value[1] < 3 && value[4] - value[3] < 3)
						{
							addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = 0;
								u ->sp = sp;
								addJKN(jkN);
							}
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = value[2] - value[1] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN);
							}
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = value[4] - value[3] + 1;
								u ->sp = sp;
								if(u ->type <2)
									addJKN(jkN);
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
			if(s_num[0] == 3 && s_num[1] == 3)
			{
			}
			else
			{
				int twoShun = 0;
				if(dsv[0] == 1 && ddsv[0] == 0)
				{
					int snc[9];
					diff(snc,s_num,0);
					int i = 0;
					for(;i<3;i++)
						snc[i]--;
					for(i=0;i<4;i++)
					{
						if(dsv[i] == 1 && ddsv[i] == 0 && snc[i] > 0 && snc[i+1] > 0 && snc[i+2] > 0)
						{
							//两顺
							twoShun = 1;
						}
					}
				}
				if(twoShun == 0)
				{
					int oneKe = 0;
					huNumJ -= 3;
					if(huNumJ < 0)
					{
						return -1;
					}
					int i ;
					for(i=0;i<4;i++)
					{
						if(s_num[i] == 4)
						{
							oneKe = 1;
							addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = 0;
								u ->sp = pChunk ->sp ->sp;
								addJKN(jkN);
							}
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = value[2] - value[1] + 1;
								u ->sp = pChunk ->sp ->next ->sp;
								if(u ->type <2)
									addJKN(jkN);
							}
							addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = 0;
								u ->sp = pChunk ->ep ->sp;
								addJKN(jkN);
							}
							{
								Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
								u ->type = s_value[1] - s_value[0] + 1;
								u ->sp = pChunk ->sp ->sp;
								if(u ->type <2)
									addJKN(jkN);
							}
						}
						else if(s_num[i] == 3)
						{
							oneKe = 1;
							if(i==0)
							{
								addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = 0;
									u ->sp = pChunk ->sp ->next ->sp;
									addJKN(jkN);
								}
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = s_value[3] - s_value[2] + 1;
									u ->sp = pChunk ->ep ->prev ->sp;
									if(u ->type <2)
										addJKN(jkN);
								}
								addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = 0;
									u ->sp = pChunk ->ep ->sp;
									addJKN(jkN);
								}
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = s_value[2] - s_value[1] + 1;
									u ->sp = pChunk ->sp ->next ->sp;
									if(u ->type <2)
										addJKN(jkN);
								}
							}
							else if(i==1)
							{
								addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = 0;
									u ->sp = pChunk ->sp ->sp;
									addJKN(jkN);
								}
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = s_value[3] - s_value[1] + 1;
									u ->sp = pChunk ->ep ->prev ->sp;
									if(u ->type <2)
										addJKN(jkN);
								}
							}
							else if(i==2)
							{
								addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = 0;
									u ->sp = pChunk ->ep ->sp;
									addJKN(jkN);
								}
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = s_value[1] - s_value[0] + 1;
									u ->sp = pChunk ->sp ->sp;
									if(u ->type <2)
										addJKN(jkN);
								}
							}
							else if(i==3)
							{
								addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = 0;
									u ->sp = pChunk ->sp ->sp;
									addJKN(jkN);
								}
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = s_value[2] - s_value[1] + 1;
									u ->sp = pChunk ->sp ->next ->sp;
									if(u ->type <2)
										addJKN(jkN);
								}
								addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = 0;
									u ->sp = pChunk ->ep ->prev ->sp;
									addJKN(jkN);
								}
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = s_value[1] - s_value[0] + 1;
									u ->sp = pChunk ->sp ->sp;
									if(u ->type <2)
										addJKN(jkN);
								}
							}
						}
					}
					int oneShun = 0;
					if(oneKe == 0)
					{
						int i;
						int snc[9];
						for(i=0;i<4;i++)
						{
							if(dsv[i] == 1 && ddsv[i] == 0)
							{
								oneShun = 1;
								diff(snc,s_num,0);
								snc[i] --;
								snc[i+1] --;
								snc[i+2] --;
							}
						}
						if(oneShun == 1)
						{
							SPNCL *spncl = NULL;
							newStruct(spncl);
							SSPai *ssp = pChunk ->sp;
							int cn = 0;
							for(i=0;i<sspN;i++)
							{
								if(snc[i] > 0)
								{
									if(i==0 || s_value[i] - s_value[i-1] > 2)
									{
										addStruct(spncl ->cs,spncl ->ce);
										cn++;
									}
									SPaiNum *spn = addStruct(spncl ->ce ->ns,spncl ->ce ->ne);
									spn ->sp = ssp ->sp;
									spn ->num = snc[i]; 
									ssp = ssp ->next;
								}
							}
							if(cn > 2)
								return -1;
							SPNC *spnc = spncl ->cs;
							if(cn==1)
							{
								addStruct(g_unitsSetE ->cus,g_unitsSetE ->cue);
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = 0;
									u ->sp = spnc ->ns ->sp;
									addJKN(jkN);
								}
								{
									Unit *u = addStruct(g_unitsSetE ->cue ->us,g_unitsSetE ->cue ->ue);
									u ->type = s_value[1] - s_value[0] + 1;
									u ->sp = spnc ->ns ->sp;
									if(u ->type <2)
										addJKN(jkN);
								}
							}
							else//cn==2
							{
							}
						}
					}
				}
			}
		}
		else
		{
			pChunk=pChunk ->next;
		}
	}
}

int checkHu(Card cards[4][9],int group[],int normalN,int huNum)
{
	int i;
	int huNumJ = huNum+1;//假定多加一个赖子,把将变成一个刻
	build(g_chunkS,huNumJ,0);
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
