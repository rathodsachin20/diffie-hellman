CC=gcc
CFLAGS=-Wall
LIBS=-lssl -lcrypto

SRC=main.c
OBJ=bn_utils.o

all: bn_utils
	$(CC) $(OBJ) -o main main.c $(CFLAGS) $(LIBS)

bn_utils: bn_utils.c
	$(CC) -c bn_utils.c

clean:
	rm -rf *.o
