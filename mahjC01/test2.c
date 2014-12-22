#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_NUM 10
int main(int argc,char **argv)
{
char*str;
str = (char*)malloc(sizeof(char)*MAX_NUM);
if(!str)
exit(-1);

int i;
for(i=0;i<MAX_NUM+2;i++){
str[i] = 'a';
}

free(str);
free(str);
return 0;
}
