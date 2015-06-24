/*
 * t2fs_aux.h
 *
 *  Created on: Jun 18, 2015
 *      Author: mhbackes
 */

#ifndef __T2FS_AUX__
#define __T2FS_AUX__

#include "t2fs.h"

#define NULL_BLOCK 0x0FFFFFFFF

/** global variables */
int _initialized_;
struct t2fs_superbloco _super_block_;
int _inode_size_;
int _record_size_;
int _sectors_per_block_;
int _inodes_per_block_;
int _dwords_per_block_;
int _records_per_block_;
int _current_dir_inode_;

/** internal functions */

// funções básicas de leitura e escrita
int init();
int read_super_block();
DWORD alloc_block();
int free_block(DWORD block);
int block_to_sector(DWORD block);
int read_block(BYTE* data, DWORD block);
int write_block(BYTE* data, DWORD block);
int inode_block(DWORD inode);
int inode_offset(DWORD inode);
int read_inode(struct t2fs_inode *inode_data, DWORD inode);
int write_inode(struct t2fs_inode *inode_data, DWORD inode);

// funções de blocos lógicos em geral
DWORD create_single_ind_block(DWORD first_ptr); //write2 <- pode ser util para escrever no arquivo
DWORD create_double_ind_block(DWORD first_ptr); //write2 <- pode ser util para escrever no arquivo
int deep_free_single_ind(DWORD block); //delete2 <- pode ser util para deletar arquivo
int deep_free_double_ind(DWORD block); //delete2 <- pode ser util para deletar arquivo


// funções de inode em geral
void inode_init(struct t2fs_inode *inode);
DWORD alloc_inode();
int free_inode(DWORD inode);
int deep_free_inode(DWORD inode); //delete2 <- pode ser util para deletar arquivo
DWORD inode_size_bytes(DWORD inode);
DWORD inode_dir_size_blocks(DWORD inode);
void add_size_bytes(DWORD inode, DWORD bytes); // write2 <- usar quando o arquivo aumenta
void add_size_blocks(DWORD inode, DWORD blocks); // write2 <- usar quando o arquivo aumenta

// funções de blocos de records
DWORD create_record_block(struct t2fs_record *record); //write2 <- precisa de uma versão pra dados dessa função pra escrever no arquivo


// funções de inode de records (aka diretório)
void record_init(struct t2fs_record *record);
int record_block(DWORD record);
int record_offset(DWORD record);
int read_record(struct t2fs_record *record_data, DWORD inode, int position);
int write_record(struct t2fs_record *record_data, DWORD inode, int position);


//IO de dados de arquivos
//retorna -1 em caso de erro

int write_file_block(DWORD inode_ptr, DWORD logical_block, BYTE *data);
int read_file_block(DWORD inode_ptr, DWORD logical_block, BYTE *data,int size);
/*adiciona registro a um diretório*/
int append_record(DWORD inode_ptr, struct t2fs_record *record); //create2 <- usar essa função para criar arquivo
int append_record_block(DWORD block, struct t2fs_record *record);
int append_record_single_ind(DWORD block, struct t2fs_record *record, DWORD inode);
int append_record_double_ind(DWORD block, struct t2fs_record *record, DWORD inode);
int remove_record(DWORD inode, int position);
int find_record(struct t2fs_record *record_data, DWORD inode,
		char *record_name);
int find_record_single_ind(struct t2fs_record *record_data, int *position,
		DWORD block, char *record_name);
int find_record_double_ind(struct t2fs_record *record_data, int *position,
		DWORD block, char *record_name);
int find_record_data_ptr(struct t2fs_record *record_data, int *position,
		DWORD *data_ptr, int data_size, char *record_name);
int find_record_in_array(struct t2fs_record *record_data, int *position,
		struct t2fs_record* record_array, int array_size, char *record_name);
DWORD find_dir_inode(DWORD curr_inode_ptr, char *path); //create2 <- usar essa função para achar o diretorio pai
int dir_is_empty(DWORD inode);
int create_dir(DWORD parent_inode, char *file_name);
int remove_dir(DWORD parent_inode, char *file_name);



// util string
int last_occurrence(char* str, int ch);


#endif /* T2FS_AUX_H_ */
