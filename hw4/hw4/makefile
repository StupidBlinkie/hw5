CC=g++

SRC= hw4_model.cpp hw4_view.cpp hw4_controller.cpp
HEADERS= hw4_model.h hw4_view.h hw4_controller.h hw4.h

LIBS=../hw2/libhw2.a
CFLAGS = -Wall -std=c++11 
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L../hw2 -L../jansson/lib -lhw2 -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw4

hw4: $(SRC) $(HEADERS) $(LIBS)
	$(CC) $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) $(SRC) $(GTKLDFLAGS) $(LDFLAGS) -o hw4

clean: rm -f *.o *~ hw4 makegame hw4-sample-solution
