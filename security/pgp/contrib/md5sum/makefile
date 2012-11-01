#
# Makefile for md5sum
#
# created by Peter Simons <simons@peti.rhein.de>
#

#
# Labels:
# =======
#
CC = gcc
LD = gcc
PGPSRC = ../../src
OBJS = md5sum.o $(PGPSRC)/md5.o

# On a big-endian system (e.g. Motorola), uncomment the following line:
#BYTEORDER= -DHIGHFIRST


# Compiler flags:
# ===============
#
# Define one of the following symbols to archive the described effect:
#
#
CFLAGS = $(BYTEORDER) -Wall -O2 -I ../../src


# Linker flags:
# =============
#
LDFLAGS  =


#
# Targets
#
md5sum:
	$(LD) $(OBJS) -o md5sum $(LDFLAGS)

$(PGPSRC)/md5.o:
	$(CC) -c $(PGPSRC)/md5.c -o $(PGPSRC)/md5.o $(CFLAGS)

clean:
	rm -f md5sum md5sum.o


#
# Dependencies
#
md5sum: $(OBJS)
md5sum.o: md5sum.c $(PGPSRC)/md5.h
