AR=ar crs
CC=gcc
LIB_DIR=../lib
INC_DIR=../include
BIN_DIR=../bin
SRC_DIR=../src
TST_DIR=../teste

all: cat2 cp2 ls2 mkdir2 rm2 rmdir2

cat2: $(LIB_DIR)/libt2fs.a
	$(CC) -g -o cat2 $(TST_DIR)/cat2.c -L$(LIB_DIR) -lt2fs -Wall

cp2: $(LIB_DIR)/libt2fs.a
	$(CC) -g -o cp2 $(TST_DIR)/cp2.c -L$(LIB_DIR) -lt2fs -Wall

ls2: $(LIB_DIR)/libt2fs.a
	$(CC) -g -o ls2 $(TST_DIR)/ls2.c -L$(LIB_DIR) -lt2fs -Wall

mkdir2: $(LIB_DIR)/libt2fs.a
	$(CC) -g -o mkdir2 $(TST_DIR)/mkdir2.c -L$(LIB_DIR) -lt2fs -Wall

rm2: $(LIB_DIR)/libt2fs.a
	$(CC) -g -o rm2 $(TST_DIR)/rm2.c -L$(LIB_DIR) -lt2fs -Wall

rmdir2: $(LIB_DIR)/libt2fs.a
	$(CC) -g -o rmdir2 $(TST_DIR)/rmdir2.c -L$(LIB_DIR) -lt2fs -Wall

clean:
	rm -rf cat2 cp2 ls2 mkdir2 rm2 rmdir2 *.~
