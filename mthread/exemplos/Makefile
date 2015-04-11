#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
INC_DIR=../include

all: exemplo mandel teste_vetor teste_prio

exemplo: exemplo.c libmthread.a
	$(CC) -o exemplo exemplo.c -L$(LIB_DIR) -lmthread -Wall

teste_vetor: teste_vetor.c $(LIB_DIR)/libmthread.a
	$(CC) -o teste_vetor teste_vetor.c -L$(LIB_DIR) -lmthread -Wall

teste_prio: teste_prio.c $(LIB_DIR)/libmthread.a
	$(CC) -o teste_prio teste_prio.c -L$(LIB_DIR) -lmthread -Wall

mandel: mandel.c $(LIB_DIR)/libmthread.a
	$(CC) -o mandel mandel.c -L$(LIB_DIR) -lmthread -lm -Wall

libmthread.a: mthread.o mdata.o
	ar crs $(LIB_DIR)/libmthread.a mthread.o mdata.o

mthread.o: $(SRC_DIR)/mthread.c
	$(CC) -c $(SRC_DIR)/mthread.c

mdata.o: $(SRC_DIR)/mdata.c
	$(CC) -c $(SRC_DIR)/mdata.c

clean:
	rm -rf exemplo mandel teste_vetor teste_prio *~ *.o




