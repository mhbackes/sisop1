CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
INC_DIR=../include

all: queue mcreate mutex

mdata.o: $(SRC_DIR)/mdata.c  $(INC_DIR)/mdata.h
	$(CC) -g -c $(SRC_DIR)/mdata.c

mthread.o: $(SRC_DIR)/mthread.c $(INC_DIR)/mthread.h $(SRC_DIR)/mdata.c  $(INC_DIR)/mdata.h
	$(CC) -g -c $(SRC_DIR)/mthread.c

queue: queue.c mdata.o
	$(CC) -o queue queue.c -g -L$(LIB_DIR) -lmthread -Wall

mcreate: mcreate.c mcreate.o mthread.o mdata.o
	$(CC) -o mcreate -g mcreate.c -L$(LIB_DIR) -lmthread  -Wall

mutex: mutex.c mthread.o mdata.o
	$(CC) -o mutex -g mutex.c -L$(LIB_DIR) -lmthread -Wall

mwait: mwait.c mthread.o mdata.o
	$(CC) -o mwait -g mwait.c -L$(LIB_DIR) -lmthread -Wall

myield: myield.c mthread.o mdata.o
	$(CC) -o myield -g myield.c -L$(LIB_DIR) -lmthread -Wall
	
clean:
	rm queue myield mcreate mutex mwait *.o
