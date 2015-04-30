#
# Makefile ESQUELETO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "mthread"
# 

CC=gcc
AR=ar
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: libmthread.a

libmthread.a: mthread.o
	$(AR) crs $(LIB_DIR)/libmthread.a $(BIN_DIR)/mthread.o $(BIN_DIR)/mdata.o

mthread.o: mdata.o
	$(CC) -o $(BIN_DIR)/mthread.o -c $(SRC_DIR)/mthread.c -Wall

mdata.o:
	$(CC) -o $(BIN_DIR)/mdata.o -c $(SRC_DIR)/mdata.c -Wall

clean:
	rm -rf $(LIB_DIR)/libmthread.a $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


