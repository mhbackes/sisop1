CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
INC_DIR=../include

mdata.o: $(SRC_DIR)/mdata.c  $(INC_DIR)/mdata.h
	$(CC) -g -c $(SRC_DIR)/mdata.c

mthread.o: $(SRC_DIR)/mthread.c $(INC_DIR)/mthread.h $(SRC_DIR)/mdata.c  $(INC_DIR)/mdata.h
	$(CC) -g -c $(SRC_DIR)/mthread.c

queue: queue.c mdata.o
	$(CC) -o queue queue.c mdata.o

mcreate: mcreate.c mcreate.o mthread.o mdata.o
	$(CC) -o mcreate -g mcreate.c mthread.o mdata.o -Wall

clean:
	rm queue *.o
