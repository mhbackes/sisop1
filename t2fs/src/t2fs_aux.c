#include "../include/t2fs_aux.h"
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include <string.h>

int init() {
	read_super_block();
	_inode_size_ = sizeof(struct t2fs_inode);
	_record_size_ = sizeof(struct t2fs_record);
	_sectors_per_block_ = _super_block_.BlockSize / SECTOR_SIZE;
	_inodes_per_block_ = _super_block_.BlockSize / _inode_size_;
	_records_per_block_ = _super_block_.BlockSize / _record_size_;

	_current_dir_inode_ = 0;
	// other initializations go here
	return 0;
}

int read_super_block() {
	char buff[SECTOR_SIZE];
	if (read_sector(0, buff) != 0)
		return -1;
	memcpy(&_super_block_, buff, sizeof(_super_block_));
	return 0;
}

int block_to_sector(DWORD block) {
	return block * _sectors_per_block_;
}

int read_block(BYTE* data, DWORD block) {
	int i, sector = block_to_sector(block);
	BYTE* data_aux = data;
	for (i = 0; i < _sectors_per_block_; i++) {
		if (read_sector(sector++, (char*) data_aux) != 0)
			return -1;
		data_aux += SECTOR_SIZE;
	}
	return 0;
}

int write_block(BYTE* data, DWORD block) {
	int i, sector = block_to_sector(block);
	BYTE* data_aux = data;
	for (i = 0; i < _sectors_per_block_; i++) {
		if (write_sector(sector++, (char*) data_aux) != 0)
			return -1;
		data_aux += SECTOR_SIZE;
	}
	return 0;
}

int inode_block(DWORD inode) {
	return _super_block_.InodeBlock + inode / _inodes_per_block_;
}

int inode_offset(DWORD inode) {
	return inode % _inodes_per_block_;
}

int read_inode(struct t2fs_inode *inode_data, DWORD inode) {
	BYTE buff[_super_block_.BlockSize];
	DWORD inode_blk = inode_block(inode);
	DWORD inode_off = inode_offset(inode);
	if (read_block(buff, inode_blk) != 0)
		return -1;
	BYTE *inode_in_buffer = buff + inode_off * _inode_size_;
	memcpy(inode_data, inode_in_buffer, _inode_size_);
	return 0;
}

int write_inode(struct t2fs_inode *inode_data, DWORD inode) {
	BYTE buff[_super_block_.BlockSize];
	DWORD inode_blk = inode_block(inode);
	DWORD inode_off = inode_offset(inode);
	if (read_block(buff, inode_blk) != 0)
		return -1;
	BYTE *inode_in_buffer = buff + inode_off * _inode_size_;
	memcpy(inode_in_buffer, inode_data, _inode_size_);
	if (write_block(buff, inode_blk) != 0)
		return -1;
	return 0;
}

int record_data_ptr(DWORD dirent) {
	return dirent / _records_per_block_;
}

int record_offset(DWORD dirent) {
	return dirent % _records_per_block_;
}

int read_record(struct t2fs_record *record_data, struct t2fs_inode *inode,
		DWORD dirent) {
	DWORD record_ptr = record_data_ptr(dirent);
	DWORD record_off = record_offset(dirent);
	if (record_ptr < 10) {
		struct t2fs_record dirent_buff[_records_per_block_];
		int record_block = inode->dataPtr[record_ptr];
		read_block((BYTE*) dirent_buff, record_block);
		memcpy(record_data, &(dirent_buff[record_off]), _record_size_);
	}
	//TODO single and double indirections
	return 0;
}
int find_record(struct t2fs_record *record_data, struct t2fs_inode *inode,
		char *record_name) {
	if (find_record_data_ptr(record_data, inode->dataPtr, 10, record_name) == 0)
		return 0;
	//TODO single and double indirections
	return -1;
}

int find_record_data_ptr(struct t2fs_record *record_data, DWORD *data_ptr,
		int data_size, char *record_name) {
	int i;
	for (i = 0; i < data_size; i++) {
		DWORD block = data_ptr[i];
		if (block == NULL_BLOCK)
			continue;
		struct t2fs_record record_array[_records_per_block_];
		read_block((BYTE*) record_array, block);
		if (find_record_in_array(record_data, record_array, _records_per_block_,
				record_name) == 0) {
			return 0;
		}
	}
	return -1;
}

int find_record_in_array(struct t2fs_record *record_data,
		struct t2fs_record* record_array, int array_size, char *record_name) {
	int i;
	for (i = 0; i < array_size; i++) {
		if (record_array[i].TypeVal != TYPEVAL_INVALIDO
				&& strcmp(record_name, record_array[i].name) == 0) {
			memcpy(record_data, &(record_array[i]), _record_size_);
			return 0;
		}
	}
	return -1;
}

void inode_init(struct t2fs_inode *inode) {
	int i;
	for (i = 0; i < 10; i++) {
		inode->dataPtr[i] = NULL_BLOCK;
	}
	inode->singleIndPtr = NULL_BLOCK;
	inode->doubleIndPtr = NULL_BLOCK;
}

DWORD alloc_inode() {
	DWORD i, j, inode_address = 0;
	for (i = _super_block_.BitmapInodes; i < _super_block_.InodeBlock; i++) {
		BYTE buff[_super_block_.BlockSize];
		read_block(buff, i);
		j = 0;
		while (j < _super_block_.BlockSize && buff[j] == 0xFF) {
			inode_address += 8;
			j++;
		}
		if (j < _super_block_.BlockSize) {
			int k;
			BYTE x = 1;
			BYTE b = buff[j];
			for (k = 0; k < 8; k++) {
				if (!(b & x)) {
					b = b | x;
					buff[j] = b;
					write_block(buff, i);
					return inode_address;
				}
				inode_address++;
				x = x << 1;
			}
		}
	}
	return NULL_BLOCK;
}

int free_inode(DWORD inode) {
	int inode_map_byte = (inode / 8) % _super_block_.BlockSize;
	int inode_map_byte_off = inode % 8;
	int inode_map_block = _super_block_.BitmapInodes
			+ (inode / 8) / _super_block_.BlockSize;
	BYTE buff[_super_block_.BlockSize];
	if (read_block(buff, inode_map_block))
		return -1;
	BYTE b = buff[inode_map_byte];
	BYTE x = 0;
	int i;
	for (i = 7; i >= 0; i--) {
		x = x << 1;
		if (i != inode_map_byte_off) {
			x = x | 1;
		}
	}
	b = x & b;
	buff[inode_map_byte] = b;
	if (write_block(buff, inode_map_block))
		return -1;
	return 0;
}

DWORD alloc_block() {
	DWORD i, j, block_address = 0;
	for (i = _super_block_.BitmapBlocks; i < _super_block_.BitmapInodes; i++) {
		BYTE buff[_super_block_.BlockSize];
		read_block(buff, i);
		j = 0;
		while (j < _super_block_.BlockSize && buff[j] == 0xFF) {
			block_address += 8;
			j++;
		}
		if (j < _super_block_.BlockSize) {
			int k;
			BYTE x = 1;
			BYTE b = buff[j];
			for (k = 0; k < 8; k++) {
				if (!(b & x)) {
					b = b | x;
					buff[j] = b;
					write_block(buff, i);
					return block_address;
				}
				block_address++;
				x = x << 1;
			}
		}
	}
	return NULL_BLOCK;
}

int free_block(DWORD block) {
	int block_map_byte = (block / 8) % _super_block_.BlockSize;
	int block_map_byte_off = block % 8;
	int block_map_block = _super_block_.BitmapBlocks
			+ (block / 8) / _super_block_.BlockSize;
	BYTE buff[_super_block_.BlockSize];
	if (read_block(buff, block_map_block))
		return -1;
	BYTE b = buff[block_map_byte];
	BYTE x = 0;
	int i;
	for (i = 7; i >= 0; i--) {
		x = x << 1;
		if (i != block_map_byte_off) {
			x = x | 1;
		}
	}
	b = x & b;
	buff[block_map_byte] = b;
	if (write_block(buff, block_map_block))
		return -1;
	return 0;
}
