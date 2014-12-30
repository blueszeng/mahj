#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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


int main()
{
	int *front[10];
	int *back[10];
	int num[10];
	char str[] = "0:1,2:3,4:5;1:2,0:3,4:5";
	int n = splitSetStr(str,front,back,num);
	int i;
	for(i=0;i<n;i++)
	{
		int j;
		for(j=0;j<num[i];j++)
		{
		}
	}

	int f[10];
	int b[10];
	char string[] = "0:-1";
	int number = splitLinkStr(string,f,b);
	for(i=0;i<number;i++)
	{
		printf("front:%d\n",f[i]);
		printf("back:%d\n",b[i]);
	}
}

