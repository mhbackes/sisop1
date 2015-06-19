/*
 * t2fs_aux.h
 *
 *  Created on: Jun 18, 2015
 *      Author: mhbackes
 */

#ifndef __T2FS_AUX__
#define __T2FS_AUX__

#define NULL_BLOCK 0x0FFFFFFFF

/** global variables */
struct t2fs_superbloco _super_block_;
int _inode_size_;
int _record_size_;
int _sectors_per_block_;
int _inodes_per_block_;
int _records_per_block_;
int _current_dir_inode_;

/** internal functions */
int init();
int read_super_block();
int block_to_sector(DWORD block);
int read_block(BYTE* data, DWORD block);
int inode_block(DWORD inode);
int inode_offset(DWORD inode);
int read_inode(struct t2fs_inode *inode_data, DWORD inode);
int record_data_ptr(DWORD record);
int record_offset(DWORD record);
int read_record(struct t2fs_record *dirent_data, struct t2fs_inode *inode, DWORD record);
int find_record(struct t2fs_record *record_data, struct t2fs_inode *inode, char *record_name);
int find_record_data_ptr(struct t2fs_record *record_data, DWORD *data_ptr, int data_size, char *record_name);
int find_record_in_array(struct t2fs_record *record_data,
		struct t2fs_record* record_array, int array_size, char *record_name);

#endif /* T2FS_AUX_H_ */
