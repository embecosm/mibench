
CC=gcc
CFLAGS=-Wall -g

all: mpglib


*.o: mpg123.h mpglib.h

mpglib: common.o dct64_i386.o decode_i386.o layer3.o tabinit.o interface.o main.o
	$(CC) -o mpglib common.o dct64_i386.o decode_i386.o layer3.o \
		tabinit.o interface.o main.o -lm

lib:    common.o dct64_i386.o decode_i386.o layer3.o tabinit.o interface.o main.o
	ar -svru libmpg123.a main.o common.o dct64_i386.o decode_i386.o layer3.o \
		tabinit.o interface.o 


clean:
	rm *.o mpglib


