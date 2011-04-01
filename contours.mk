CC=g++
CFLAGS=-Wall -fkeep-inline-functions -g -O3
TRACKINGLFLAGS=-lpthread
IDLLFLAGS=-L /usr/local/idl/idl706/bin/bin.linux.x86_64 -lpthread -lidl -lXp -lXpm -lXmu -lXext -lXt -lSM -lICE  -lXinerama -lX11 -ldl -ltermcap -lrt -lm /usr/lib/libXm.a
MAGICKLFLAGS=`Magick++-config --cppflags --ldflags --libs`
MAGICKCFLAGS=`Magick++-config --cppflags`
DFLAGS= $(MAGICKCFLAGS)
LFLAGS=-lcfitsio $(MAGICKLFLAGS)

all:bin/contours.x
clean: rm bin/contours.x objects/contours.o objects/gradient.o objects/ArgumentHelper.o objects/ColorMap.o objects/SunImage.o objects/FitsFile.o objects/Coordinate.o objects/Image.o objects/Header.o objects/tools.o


bin/contours.x : contours.mk objects/contours.o objects/gradient.o objects/ArgumentHelper.o objects/ColorMap.o objects/SunImage.o objects/FitsFile.o objects/Coordinate.o objects/Image.o objects/Header.o objects/tools.o
	$(CC) $(CFLAGS) $(DFLAGS) objects/contours.o objects/gradient.o objects/ArgumentHelper.o objects/ColorMap.o objects/SunImage.o objects/FitsFile.o objects/Coordinate.o objects/Image.o objects/Header.o objects/tools.o $(LFLAGS) -o bin/contours.x

objects/contours.o : contours.mk utilities/contours.cpp classes/tools.h classes/constants.h classes/ColorMap.h classes/ArgumentHelper.h classes/gradient.h
	$(CC) -c $(CFLAGS) $(DFLAGS) utilities/contours.cpp -o objects/contours.o

objects/gradient.o : contours.mk classes/gradient.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/gradient.cpp -o objects/gradient.o

objects/ArgumentHelper.o : contours.mk classes/ArgumentHelper.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/ArgumentHelper.cpp -o objects/ArgumentHelper.o

objects/ColorMap.o : contours.mk classes/ColorMap.cpp classes/Header.h classes/SunImage.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/ColorMap.cpp -o objects/ColorMap.o

objects/SunImage.o : contours.mk classes/SunImage.cpp classes/Image.h classes/Coordinate.h classes/Header.h classes/FitsFile.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/SunImage.cpp -o objects/SunImage.o

objects/FitsFile.o : contours.mk classes/FitsFile.cpp classes/fitsio.h classes/longnam.h classes/tools.h classes/constants.h classes/Header.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/FitsFile.cpp -o objects/FitsFile.o

objects/Coordinate.o : contours.mk classes/Coordinate.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Coordinate.cpp -o objects/Coordinate.o

objects/Image.o : contours.mk classes/Image.cpp classes/tools.h classes/constants.h classes/Coordinate.h classes/FitsFile.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Image.cpp -o objects/Image.o

objects/Header.o : contours.mk classes/Header.cpp 
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/Header.cpp -o objects/Header.o

objects/tools.o : contours.mk classes/tools.cpp classes/constants.h
	$(CC) -c $(CFLAGS) $(DFLAGS) classes/tools.cpp -o objects/tools.o

