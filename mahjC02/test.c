#include <stdio.h>
#include "test.h"
struct timeval timeVal[10];

struct timeval setTime(int i)
{
	gettimeofday(&timeVal[i],0);
}

int getRunTime(int start,int end)
{
	setTime(end);
	int time = 1000000 * (timeVal[end].tv_sec-timeVal[start].tv_sec)+ timeVal[end].tv_usec-timeVal[start].tv_usec;
	printf("time::%d\n",time);
	return time;
}

int main()
{
	int num = 10;
	int a[num];
	int i;
	for(i=0;i<num;i++)
		a[i] = i;
	for(i=0;i<num;i++)
		printf("%d\n",a[i]);

	MAHJ *mahj;
	newStruct(&mahj);
	mahj->value = 0;
	mahj->color = 1;
	printf("%d,%d",mahj->value,mahj->color);
	deleteStruct(mahj);


	int n = 10;
	int an[n] ;
	memset(an,-1,sizeof(an));
	printf("---------\n");
	for(i = 0;i<n;i++)
		printf("%d ",an[i]);
	printf("\n");


	int *p;
	newStruct(&p);

	printf("%d\n",*p);


	setTime(1);
	




	getRunTime(1,2);



}
