#################################
#
#  Simple Makefile
#  by bighead  2008.2.14
#  Email:jsrenyw@gmail.com
#  
#  NO warranty NO liability
#  You use this at your own risk
#  
#  Haha,Go and Use it.
#
#  Have A Good DAY ........
#
#  游戏编译文件  2009
#  根据项目需要请修改编译的主文件  MAIN 和 编译后的执行文件 TARGET 即可
#
##################################
#FLAGS =  -g -W -Wall  -lnsl -lm -lz  -O2
FLAGS =  -g 
#LIB = -L/usr/lib/mysql/ -lpthread
#INCLUDE =   -I/usr/include/mysql  -I./
CC = g++ 

SOURCE := $(wildcard *.c) $(wildcard *.h)

#SRC_SOURCE := $(wildcard ../src/*.c) $(wildcard ../include/*.h)

MAIN = mahj3.c
TARGET = a 

ALL:$(TARGET)

two:
	gcc -g mahj2.c -o a2

$(TARGET):$(SOURCE) $(SRC_SOURCE)
	$(CC)  $(MAIN) $(FLAGS) $(INCLUDE) $(LIB)   -o $(TARGET)


rebuilt:clean $(TARGET)


.PHONY:clean allclean del
clean:
	-rm -f $(TARGET)
	-rm -f test

del allclean:
	-rm -f $(TARGET)
	-rm -f tags

.PHONY:tag tags ctag ctags
### useage  vi -t myFunc
tag tags ctag ctags:
	ctags -R *

.PHONY:md5 md
md md5:
	date >> game.md5
	md5sum *.c *.h >> game.md5
	date >> game.md5

