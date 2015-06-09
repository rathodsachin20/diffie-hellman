CC=gcc -O2
CFLAGS=-Wall
LIBS=-lssl -lcrypto -lrt

SRC=main.c
OBJ=bn_utils.o montgomery.o diffie_hellman.o global.o

all: bn_utils montgomery diffie_hellman global
	$(CC) $(OBJ) -o main main.c $(CFLAGS) $(LIBS)

bn_utils: bn_utils.c
	$(CC) -c bn_utils.c

montgomery: montgomery.c
	$(CC) -c montgomery.c

diffie_hellman: diffie_hellman.c
	$(CC) -c diffie_hellman.c

global: global.c
	$(CC) -c global.c

clean:
	rm -rf *.o
