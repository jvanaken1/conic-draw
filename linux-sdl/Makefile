# Build demo1 and demo2 programs to run on SDL2 in Linux
# This makefile uses the GNU C/C++ compiler and linker
# Run the GNU make utility from the command line in this directory

CC = g++

OBJS = conic.o bounce.o

all : .PHONY demo1 demo2

demo1 : demo1.o $(OBJS)
	$(CC) -o demo1 demo1.o $(OBJS) -lSDL2

demo2 : demo2.o $(OBJS)
	$(CC) -o demo2 demo2.o $(OBJS) -lSDL2

demo1.o : demo1.cpp demo.h conic.h
	$(CC) -w -c demo1.cpp

demo2.o : demo2.cpp demo.h conic.h
	$(CC) -w -c demo2.cpp

conic.o : conic.cpp conic.h
	$(CC) -w -c conic.cpp

bounce.o : bounce.cpp demo.h
	$(CC) -w -c bounce.cpp

.PHONY :
	cp -u ../*.cpp .
	cp -u ../*.h .

clean:
	rm *.o
	rm demo

