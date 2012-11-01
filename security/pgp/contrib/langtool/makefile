# Makefile for language tools
#


# E= .exe

CFLAGS= -O

all: langtool$E charconv$E pickpstr$E killdups$E


langtool$E: language.c crc.c
	$(CC) $(CFLAGS) -DLANGTOOL language.c crc.c -o $@

charconv$E: charconv.c
	$(CC) $(CFLAGS) charconv.c -o $@

pickpstr$E: pickpstr.c
	$(CC) $(CFLAGS) pickpstr.c -o $@

killdups$E: killdups.c
	$(CC) $(CFLAGS) killdups.c -o $@

langtool.c: usuals.h
