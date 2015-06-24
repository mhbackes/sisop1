#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
# 

AR=ar crs
CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TST_DIR=./teste

all:

test: test.o t2fs.a
	$(CC) -o $(BIN_DIR)/test $(BIN_DIR)/test.o -L$(LIB_DIR) -lt2fs -Wall -m32

shell: t2fs.a
	$(CC) -o $(BIN_DIR)/shell $(TST_DIR)/shell.c -L$(LIB_DIR) -lt2fs -Wall -m32

test.o: $(TST_DIR)/test.c
	$(CC) -c $(TST_DIR)/test.c -o $(BIN_DIR)/test.o -Wall

t2fs.a: t2fs.o t2fs_aux.o $(LIB_DIR)/apidisk.o
	$(AR) $(LIB_DIR)/libt2fs.a $(BIN_DIR)/t2fs.o $(BIN_DIR)/t2fs_aux.o $(LIB_DIR)/apidisk.o

t2fs.o: $(SRC_DIR)/t2fs.c
	$(CC) -I. -c $(SRC_DIR)/t2fs.c -o $(BIN_DIR)/t2fs.o -Wall

t2fs_aux.o: $(SRC_DIR)/t2fs_aux.c
	$(CC) -I. -c $(SRC_DIR)/t2fs_aux.c -o $(BIN_DIR)/t2fs_aux.o -Wall -m32

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ $(TST_DIR)/*~ *~


