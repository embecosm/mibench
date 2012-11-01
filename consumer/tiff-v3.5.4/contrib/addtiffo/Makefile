#
# If libtiff.a is installed in /usr/lib or /usr/local/lib just point
# LIBTIFF_DIR there.  It doesn't need a full libtiff tree.
#
LIBTIFF_DIR =	../../libtiff
#
CC	=	gcc
CXX	=	gcc
CFLAGS	=	-O -I$(LIBTIFF_DIR)
LIBS	=	$(LIBTIFF_DIR)/libtiff.a -lm

addtiffo:	addtiffo.o tif_overview.o rawblockedimage.o
	$(CXX) $(CFLAGS) addtiffo.o tif_overview.o rawblockedimage.o \
		$(LIBS) -o addtiffo 


addtiffo.o:	addtiffo.c
	$(CC) -c $(CFLAGS) addtiffo.c

tif_overview.o:	tif_overview.cpp
	$(CXX) -c $(CFLAGS) tif_overview.cpp

rawblockedimage.o:	rawblockedimage.cpp
	$(CXX) -c $(CFLAGS) rawblockedimage.cpp

clean:
	rm *.o addtiffo
