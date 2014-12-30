#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
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

#define N 0x7FFFFFFU 
int main()
{
    unsigned int i;
    double a,b,c;
    unsigned long long t;
    clock_t c_start, c_end;
    printf("%d\n", sizeof(clock_t));

    a = 2.3928318;
    b = 83.328321;

	setTime(1);
    for(i = 0; i < N; i++)
        ; /* null loop */
	getRunTime(1,2);

	setTime(1);
    for(i = 0; i < N; i++)
        c = a * b; /* float multi */
	getRunTime(1,2);

	setTime(1);
	int an[100];
    for(i = 0; i < N; i++)
	{
		int j = i%100;
		an[j] = j;
	}
	getRunTime(1,2);

	setTime(1);
	int *p = an;
	p = (int *)malloc(4*100);
    for(i = 0; i < N; i++)
	{
		int j = i%100;
		*(p+j) = j;
	}
	getRunTime(1,2);

	setTime(1);
    for(i = 0; i < N; i++)
	{
		int j = 1;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
		if(j<11)j++;
	}
	getRunTime(1,2);

	setTime(1);
    for(i = 0; i < N; i++)
	{
		int j = 1;
		while(j<11)
			j++;
	}
	getRunTime(1,2);

    return 0;
}
