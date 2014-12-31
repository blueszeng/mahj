#define _GNU_SOURCE
#include <stdio.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<ctype.h>
#include <stdlib.h>
#include "mah_head.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

int zong=0;
int buhu=0;
int normalNum = 0;
NodeRes *startRes = NULL;
NodeRes *endRes = NULL;
JIANG jiang;
TreeNode *root = NULL;

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
	char *delim=" ";
	char *p;
	int mah[pNum], mah_num = 0, j; 
	MAHJ mahj[pNum];
	int count = 0,flag = 0;
	int color, val, ture_color;
	struct pai cards[4][10]; 
	struct pai *laizi = NULL;
	 fp = fopen("../mahjC02/data3", "r");//打开文件
	 if (fp == NULL)
	 {
		 printf("打开文件失败！\n");
       exit(EXIT_FAILURE);
	 }

     while ((getline(&line, &len, fp)) != EOF) //读取一行
	{	
		laizi = NULL;
		memset(&mah,0,sizeof(mah));//二维数组初始化为0
		memset(&mahj,0,sizeof(mahj));
		mah_num = 0;
		p = strtok(line,delim);//分割一行中的第一个数
		if((p[0] == ' ') || (p[0] == 13) || (p[0] == 10))//屏蔽所有空格，换行。
		{
			continue;
		}
		if((mah[0] = str_to_value(p,&mahj[0])) == -1)//第0个都不合法//本行分割不成功，读取下一行
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
			if((mah[mah_num] = str_to_value(p,&mahj[mah_num])) == -1)//第i个不合法
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
			int color = mahj[j].color;	//取的花色真值：1、2、4、8
			int val = mahj[j].value;
			cards[color][val].amount++;		// 该花色该牌值加一
			cards[color][9].amount++;			  //该花色总数加一
			if(mah[j] == mah[pNum_1])//非万能牌
			{		
				cards[color][val].laizi_flag=1;		// 该花色该牌为赖子
				laizi = &cards[color][val];
			}
			else//万能牌
			{
				cards[color][val].laizi_flag=0;		// 该花色该牌不是赖子	
			}
		}
		///查看是否有四张以上的牌，有则不合法。读取下一行
		flag = 0;
		for(color = 0; color < 4; color++)
		{
			int valMax = 9;
			if(color == 3)valMax = 7;
			for(val = 0;val < valMax;val++)
			{
				if(cards[color][val].amount > 4)
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
		zong++; //合法的数据条数
		sortP(mah,mahj);

		checkHu(mah,mahj);
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

int initRes()
{
	startRes = NULL;
	endRes = startRes;
}

int freeRes()
{
	NodeRes *node = startRes;
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
	startRes = NULL;
	endRes = NULL;
}

int initJiang()
{
	jiang.mahj[0] = newMahj();
	jiang.mahj[1] = newMahj();
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
	free(jiang.mahj[0]);
	free(jiang.mahj[1]);
	jiang.mahj[0] = NULL;
	jiang.mahj[0] = NULL;
}



int geneMahjsRes(TreeNode *node,MAHJ *mahjs[12])
{
	int i = 12;
	while(node)
	{
		int j = 0;
		for(;j<3;j++)
		{
			i--;
			//if(i>=0)//不加上这句会出现地址越界,然后free的时候会出现free(): invalid next size (fast)
			{
				mahjs[i] = node->hand->mahj[2-j]; 
			}
			if(i==0){
				return 0;
			}
		}
		node = node->parent;
	}
	return i;
}

int printRes()
{
	NodeRes *node = startRes;
	printf("hupai:\n");
	MAHJ *mahjs[12];
	while(node){
		if(geneMahjsRes(node->node,mahjs)==0)
		{
			MAHJ *hus[5];
			int hi = 0;
			int i;
			for(i=0;i<12;i++)
			{
				printf("%d%d ",mahjs[i]->color,mahjs[i]->value);
				if(mahjs[i]->isHu==1)
				{
					hus[hi++] = mahjs[i];
				}
			}
			printf("hu: ");
			for(i=0;i<hi;i++)
			{
				printf("%d%d ",hus[i]->color,hus[i]->value);
			}
			printf("\n");
		}
		node = node->next;
	}
}

int saveRes(TreeNode *node,int huDui)
{
	huShu++;
	NodeRes *nodeRes = (NodeRes *)malloc(sizeof(NodeRes));
	if(nodeRes==NULL)
		return -1;
	TreeNode *pNode = node;
	while(huDui-->0)
	{
		TreeNode *node = newTreeNode();
		if(node==NULL)return -1;
		node->hand = newHand();
		int i;
		for(i=0;i<3;i++)
		{
			node->hand->mahj[i] = newMahj();
			node->hand->mahj[i]->value = 6;
			node->hand->mahj[i]->color = 3;
			node->hand->mahj[i]->isHu = 0;
		}
		pNode->child[3] = node;
		node->parent = pNode;
		pNode=node;
	}
	nodeRes->node = pNode;
	nodeRes->next = NULL;
	nodeRes->prev = endRes;
	if(startRes == NULL)
	{
		startRes = nodeRes;
		endRes = nodeRes;
	}
	else
	{
		endRes->next = nodeRes;
	}
	endRes = nodeRes;
}


int printHu()
{
	printf("jiang:");
	printf("%d%d ",jiang.mahj[0]->color,jiang.mahj[0]->value);
	printf("%d%d ",jiang.mahj[1]->color,jiang.mahj[1]->value);
	if(jiang.mahj[0]->isHu==1)
	{
		printf("jiangHu");
	}
	printf("\n");
	printRes();
}


int checkHu(int mah[],MAHJ mahj[])
{
	int huNum = getHuNum(mah);
	printf("huNum:%d\n",huNum);
	int norN = pNum - huNum;
	int i = 0;

	normalNum = norN-1;
	int mahN[normalNum];
	MAHJ mahjN[normalNum];
	initJiang();
	//用两个赖子作将
	if(huNum>1)
	{
		normalNum = norN;
		printf("normalNum:%d\n",normalNum);
		jiang.mahj[0]->isHu = 0;
		jiang.mahj[1]->isHu = 0;
		jiang.mahj[0]->value = 6;
		jiang.mahj[0]->color = 3;
		jiang.mahj[1]->value = 6;
		jiang.mahj[1]->color = 3;
		int j;
		for(j=0;j<norN;j++)
		{
			mahN[j] = mah[j];
			mahjN[j] = mahj[j];
		}
		initHu();
		build(mahN,mahjN,huNum);
		printHu();
		freeHu();
	}
	for(;i<norN;i++)
	{
		//不用赖子作将
		if(i<norN-1 && mah[i] == mah[i+1]){
			normalNum = norN-2;
			printf("normalNum:%d\n",normalNum);
			jiang.mahj[0]->isHu = 0;
			jiang.mahj[1]->isHu = 0;
			jiang.mahj[0]->value = mahj[i].value;
			jiang.mahj[0]->color = mahj[i].color;
			jiang.mahj[1]->value = mahj[i].value;
			jiang.mahj[1]->color = mahj[i].color;
			int j;
			for(j=0;j<i;j++)
			{
				mahN[j] = mah[j];
				mahjN[j] = mahj[j];
			}
			for(j=i+2;j<norN;j++)
			{
				mahN[j-2] = mah[j];
				mahjN[j-2] = mahj[j];
			}
			initHu();
			build(mahN,mahjN,huNum);
			printHu();
			freeHu();
			i++;
		}else if(i>0&&mah[i]==mah[i-1]){
			continue;
		//用一个赖子作将
		}else{
			normalNum = norN-1;
			if(huNum<1)
			{
				continue;
			}
			jiang.mahj[0]->isHu = 1;
			jiang.mahj[0]->value = mahj[i].value;
			jiang.mahj[0]->color = mahj[i].color;
			jiang.mahj[1]->value = mahj[i].value;
			jiang.mahj[1]->color = mahj[i].color;
			int j;
			for(j=0;j<i;j++)
			{
				mahN[j] = mah[j];
				mahjN[j] = mahj[j];
			}
			for(j=i+1;j<norN;j++)
			{
				mahN[j-1] = mah[j];
				mahjN[j-1] = mahj[j];
			}
			initHu();
			build(mahN,mahjN,huNum-1);
			printHu();
			freeHu();
		}
	}
	freeJiang();
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
		
int build(int mah[],MAHJ mahj[],int huNum)
{
	root  = newTreeNode();
	
	int type = 0;
	for(;type<4;type++) 
		buildChild(root,mah,mahj,huNum,0,type);

	return 1;
}

int buildChild(TreeNode *node,int mah[],MAHJ mahj[],int huNum,int startIndex,int type)
{
	MAHJ *mahj0 = &mahj[startIndex];
	TreeNode *childNode = NULL;
	if(type < 3)
	{
		if(mahj[startIndex].color == 3) return -1;
		if(mahj[startIndex].value > 6+type )return -1;
		if(mahj[startIndex].value < type )return -1;
		int num = normalNum - 1 - startIndex;
		if(num>2)num=2;
		int minT = type;
		if(type<2-num)
		{
			minT = 2-num;
		}
		huNum -= minT;
		if(huNum < 0) return -1;
		int i = 0;
		int j = 0;
		int k[2]={0};//是否需要混
		for(;i<2-minT;i++)
		{
			if(mahj[startIndex].color == mahj[startIndex+1].color && mahj[startIndex].value == mahj[startIndex+1].value - j - 1)
			{
				startIndex++;
			}
			else{
				k[i] = 1;
				j++;
				huNum --;
				if(huNum < 0)
					return -1;
			}
		}
		HAND *hand = newHand();
		hand->type = 1;
		for(i=0;i<3;i++)
		{
			MAHJ *mahj = newMahj();
			mahj->color = mahj0->color;
			mahj->value = mahj0->value+i-type;
			hand->mahj[i] = mahj;
			hand->mahj[i]->isHu = 0;
		}
		for(i=0;i<2-type;i++)
		{
			if((k[i]==1) || (i>=num))
			{
				hand->mahj[type+1+i]->isHu = 1;
			}
		}
		for(i=0;i<type;i++)
		{
			hand->mahj[i]->isHu = 1;
		}
		childNode = newTreeNode();
		childNode->parent = node;
		childNode->hand = hand;
		node->child[type] = childNode;
		if(startIndex == normalNum - 1) 
		{
			saveRes(childNode,huNum/3);
			return 0;
		}
		/*
		else if(startIndex > normalNum - 1)
		{
			return -1;
		}
		*/
	}
	else
	{
		int i = 0;
		int j = 0;
		//判断位置
		int num = normalNum-1 - startIndex;
		if(num>2)
			num=2;
		for(;i<num;i++)
		{
			if(mahj[startIndex].color == mahj[startIndex+1].color && mahj[startIndex].value == mahj[startIndex+1].value)
			{
				j++;
				startIndex++;
			}
			else{
				break;
			}
		}
		//判断混的数目
		huNum = huNum-2+j;
		if(huNum < 0)
			return -1;
		//新建配对
		HAND *hand = newHand();
		hand->type = 0;
		//复制麻将的数值
		i = 0;
		for(;i<3;i++)
		{
			MAHJ *mahj = newMahj();	
			mahj->color = mahj0->color;
			mahj->value = mahj0->value;
			hand->mahj[i] = mahj;
			if(i>j)
			{
				mahj->isHu = 1;
			}
			else{
				mahj->isHu = 0;
			}
		}
		childNode = newTreeNode();
		childNode->parent = node;
		childNode->hand = hand;
		node->child[type] = childNode;
		if(startIndex == normalNum - 1) 
		{
			saveRes(childNode,huNum/3);
			return 0;
		}
		/*
		else if(startIndex > normalNum - 1)//没有这个会段错误
		{
			return -1;
		}
		*/
	}
	if(childNode)
	{
		int type_ = 0;
		for(;type_<4;type_++) 
			buildChild(childNode,mah,mahj,huNum,startIndex+1,type_);
	}
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
