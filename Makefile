CC=gcc
CFLAGS=-Wall
LIBS=-lssl -lcrypto

SRC=main.c
OBJ=bn_utils.o montgomery.o

all: bn_utils montgomery
	$(CC) $(OBJ) -o main main.c $(CFLAGS) $(LIBS)

bn_utils: bn_utils.c
	$(CC) -c bn_utils.c

montgomery: montgomery.c
	$(CC) -c montgomery.c

clean:
	rm -rf *.o
