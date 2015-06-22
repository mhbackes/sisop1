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
int init();
int read_super_block();
int block_to_sector(DWORD block);
int read_block(BYTE* data, DWORD block);
int inode_block(DWORD inode);
int inode_offset(DWORD inode);

void record_init(struct t2fs_record *record);
int record_block(DWORD record);
int record_offset(DWORD record);
int read_record(struct t2fs_record *record_data, DWORD inode, int position);
int write_record(struct t2fs_record *record_data, DWORD inode, int position);
int append_record(DWORD inode_ptr, struct t2fs_record *record);
int append_record_block(DWORD block, struct t2fs_record *record);
int append_record_single_ind(DWORD block, struct t2fs_record *record, DWORD inode);
int append_record_double_ind(DWORD block, struct t2fs_record *record, DWORD inode);
int remove_record(DWORD inode, int position);
void add_size_bytes(DWORD inode, DWORD bytes);
void add_size_blocks(DWORD inode, DWORD blocks);

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

int last_record_pos(DWORD inode);

DWORD find_dir_inode(DWORD curr_inode_ptr, char *path);
int dir_is_empty(DWORD inode);

DWORD inode_size_bytes(DWORD inode);
DWORD inode_size_blocks(DWORD inode);
int read_inode(struct t2fs_inode *inode_data, DWORD inode);
int write_inode(struct t2fs_inode *inode_data, DWORD inode);
void inode_init(struct t2fs_inode *inode);
DWORD alloc_inode();
int free_inode(DWORD inode);
int deep_free_inode(DWORD inode);

int free_single_ind(DWORD block);
int free_double_ind(DWORD block);

DWORD create_record_block(struct t2fs_record *record);
DWORD create_single_ind_block(DWORD first_ptr);
DWORD create_double_ind_block(DWORD first_ptr);

DWORD alloc_block();
int free_block(DWORD block);

int last_occurrence(char* str, int ch);

int create_dir(DWORD parent_inode, char *file_name);
int remove_dir(DWORD parent_inode, char *file_name);
#endif /* T2FS_AUX_H_ */
