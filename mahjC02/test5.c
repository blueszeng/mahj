#include <stdio.h>


int func(int *&p)
{
	p = (int *)1;
}

int main()
{
	int a = 0;
	int *b = &a;
	int c = a;
	printf("%p,%p,%p\n",&a,&(*b),&c);



	int g[5] = {1,2,3,4,5};

	int *pg[5];

	int i ;
	for(i=0;i<5;i++)
	{
		pg[i] = &g[i];
	}


	int **ppg[5];
	for(i=0;i<5;i++)
	{
		ppg[i] = &pg[i];
	}

	int *p = *ppg[0];
	func(*ppg[0]);
	func(p);
	printf("pg:%d,%d\n",pg[0],*ppg[0]);
	printf("%d,%d,%d\n",&p,&(*ppg[0]),&(pg[0]));


	int *pa = &a;
	
	int **pp = &pa;

	int *&p1 = pa;

	p1 = &c;


	int a1 = 1;
	int &b1 = a1;
	a1 =  2;
	printf("b:%d\n",b1);


}
