#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
INC_DIR=../include
BIN_DIR=../bin

all: exemplo mandel teste_vetor teste_prio primos

exemplo: exemplo.c $(LIB_DIR)/libmthread.a
	$(CC) -g -o exemplo exemplo.c -L$(LIB_DIR) -lmthread -Wall

teste_vetor: teste_vetor.c $(LIB_DIR)/libmthread.a
	$(CC) -g -o teste_vetor teste_vetor.c -L$(LIB_DIR) -lmthread -Wall

teste_prio: teste_prio.c $(LIB_DIR)/libmthread.a
	$(CC) -g -o teste_prio teste_prio.c -L$(LIB_DIR) -lmthread -Wall

mandel: mandel.c $(LIB_DIR)/libmthread.a
	$(CC) -g -o mandel mandel.c -L$(LIB_DIR) -lmthread -lm -Wall

primos: primos.c $(LIB_DIR)/libmthread.a
	$(CC) -g -o primos primos.c -L$(LIB_DIR) -lmthread -lm -Wall

clean:
	rm -rf exemplo mandel teste_vetor teste_prio primos *~ *.o




