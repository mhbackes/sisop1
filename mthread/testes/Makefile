CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
INC_DIR=../include

all: queue mcreate mutex myield mmutex_init mlock

queue: queue.c 
	$(CC) -o queue queue.c -g -L$(LIB_DIR) -lmthread -Wall

mcreate: test_mcreate.c $(LIB_DIR)/libmthread.a
	$(CC) -o mcreate -g test_mcreate.c -L$(LIB_DIR) -lmthread  -Wall

mutex: mutex.c $(LIB_DIR)/libmthread.a
	$(CC) -o mutex -g mutex.c -L$(LIB_DIR) -lmthread -Wall

mwait: mwait.c $(LIB_DIR)/libmthread.a
	$(CC) -o mwait -g mwait.c -L$(LIB_DIR) -lmthread -Wall

myield: myield.c $(LIB_DIR)/libmthread.a
	$(CC) -o myield -g myield.c -L$(LIB_DIR) -lmthread -Wall
	
mmutex_init: mmutex_init.c $(LIB_DIR)/libmthread.a
	$(CC) -o mmutex_init -g mmutex_init.c -L$(LIB_DIR) -lmthread -Wall
	
mlock: mlock.c $(LIB_DIR)/libmthread.a
	$(CC) -o mlock -g mlock.c -L$(LIB_DIR) -lmthread -Wall
	
clean:
	rm queue myield mcreate mutex mwait *.o
