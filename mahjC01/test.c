#include <stdio.h>

int main()
{
	int num = 10;
	int a[num];
	int i;
	for(i=0;i<num;i++)
		a[i] = i;
	for(i=0;i<num;i++)
		printf("%d\n",a[i]);

}
