#ifndef __MAH__HEAD__H__
#define __MAH__HEAD__H__

#define pNum 14 
#define pNum_1 (pNum-1) 

#include "../mahjC02/common.h"

typedef struct mahj{
	int color;
	int value;
	char isHu;
}MAHJ;

typedef struct hand{
	int type;//0,刻，１，顺
	MAHJ *mahj[3];
}HAND;

typedef struct jiang{
	MAHJ *mahj[2];
}JIANG;

typedef struct treeNode{
	HAND *hand;
	struct treeNode *child[4];//shun1,shun2,shun3,ke
	struct treeNode *parent;
}TreeNode;

typedef struct nodeRes{
	struct treeNode *node;
	struct nodeRes *next;
	struct nodeRes *prev;
}NodeRes;

typedef struct pai
{
	int laizi_flag;//等于1为赖子
	int amount;//该牌的张数
}pai;

typedef struct pihu
{
	int type;//1:将；2：刻；3：顺;4:七对的对
	int laizi_flag;//等于一为赖子
	int mahj;//牌值
}pihu;


int sortP(int mah[pNum],MAHJ mahj[pNum]);
int getHuNum(int mah[]);
int initRes();
int freeRes();
int initJiang();
int initHu();
int freeTreeNode(TreeNode *node);
int freeHu();
int freeJiang();
int geneMahjsRes(TreeNode *node,MAHJ *mahjs[12]);
int printRes();
int saveRes(TreeNode *node,int huDui);
int printHu();
int checkHu(int mah[],MAHJ mahj[]);
TreeNode *newTreeNode();
HAND *newHand();
MAHJ *newMahj();
int build(int mah[pNum],MAHJ mahj[pNum],int huNum);
int buildChild(TreeNode *node,int mah[pNum],MAHJ mahj[],int huNum,int startIndex,int type);
int str_to_value(char Str_Hex[],MAHJ *pMahj);
MAHJ value_to_mahj(int value);
int mahj_to_value(MAHJ mahj);

#endif
