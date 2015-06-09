CC=gcc -O2
CFLAGS=-Wall
LIBS=-lssl -lcrypto -lrt

SRC=main.c
OBJ=bn_utils.o montgomery.o diffie_hellman.o global.o

all: bn_utils montgomery diffie_hellman global alice bob
	$(CC) $(OBJ) -o main main.c $(CFLAGS) $(LIBS)

bn_utils: bn_utils.c
	$(CC) -c bn_utils.c

montgomery: montgomery.c
	$(CC) -c montgomery.c

diffie_hellman: diffie_hellman.c
	$(CC) -c diffie_hellman.c

global: global.c
	$(CC) -c global.c

alice: alice.c all
	$(CC) $(OBJ) -o alice alice.c -lpthread -lcrypto -lssl

bob: bob.c all
	$(CC) $(OBJ) -o bob bob.c -lpthread -lcrypto -lssl

clean:
	rm -rf *.o, main, alice, bob
