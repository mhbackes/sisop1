CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
INC_DIR=../include

all: mcreate myield mmutex_init mlock mwait munlock primos

mcreate: mcreate.c $(LIB_DIR)/libmthread.a
	$(CC) -o mcreate -g mcreate.c -L$(LIB_DIR) -lmthread  -Wall

mwait: mwait.c $(LIB_DIR)/libmthread.a
	$(CC) -o mwait -g mwait.c -L$(LIB_DIR) -lmthread -Wall

myield: myield.c $(LIB_DIR)/libmthread.a
	$(CC) -o myield -g myield.c -L$(LIB_DIR) -lmthread -Wall
	
mmutex_init: mmutex_init.c $(LIB_DIR)/libmthread.a
	$(CC) -o mmutex_init -g mmutex_init.c -L$(LIB_DIR) -lmthread -Wall
	
mlock: mlock.c $(LIB_DIR)/libmthread.a
	$(CC) -o mlock -g mlock.c -L$(LIB_DIR) -lmthread -Wall

munlock: munlock.c $(LIB_DIR)/libmthread.a
	$(CC) -o munlock -g munlock.c -L$(LIB_DIR) -lmthread -Wall

primos: primos.c $(LIB_DIR)/libmthread.a
	$(CC) -o primos -g primos.c -L$(LIB_DIR) -lmthread -Wall
	
clean:
	rm -rf myield mcreate mwait mlock mmutex_init munlock primos *.o *~
