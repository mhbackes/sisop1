#include "../include/t2fs_aux.h"
#include "../include/apidisk.h"
#include <string.h>


int init() {
	if (read_super_block() != 0)
		return -1;
	_inode_size_ = sizeof(struct t2fs_inode);
	_record_size_ = sizeof(struct t2fs_record);
	_dwords_per_block_ = _super_block_.BlockSize / sizeof(DWORD);
	_sectors_per_block_ = _super_block_.BlockSize / SECTOR_SIZE;
	_inodes_per_block_ = _super_block_.BlockSize / _inode_size_;
	_records_per_block_ = _super_block_.BlockSize / _record_size_;
	_current_dir_inode_ = 0;
	// other initializations go here
	_initialized_ = 1;
	
	printf("block size size %u",_super_block_.BlockSize);
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

DWORD inode_size_bytes(DWORD inode) {
	struct t2fs_record dot_record;
	if (find_record(&dot_record, inode, ".") >= 0) {
		if (inode == 0) // se é o root, deve subtrair o tamanho de um bloco
			return dot_record.bytesFileSize - _super_block_.BlockSize
					+ 2 * _record_size_;
		else
			return dot_record.bytesFileSize;
	}
	return -1;
}

DWORD inode_size_blocks(DWORD inode) {
	struct t2fs_record dot_record;
	if (find_record(&dot_record, inode, ".") >= 0) {
		return dot_record.blocksFileSize;
	}
	return -1;
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

int read_record(struct t2fs_record *record_data, DWORD inode, int position) {
	int size = inode_size_bytes(inode) / _record_size_;
	if (position >= size)
		return -1;
	DWORD record_ptr = record_data_ptr(position);
	DWORD record_off = record_offset(position);
	struct t2fs_inode inode_data;
	read_inode(&inode_data, inode);
	if (record_ptr < 10) {
		struct t2fs_record dirent_buff[_records_per_block_];
		int record_block = inode_data.dataPtr[record_ptr];
		read_block((BYTE*) dirent_buff, record_block);
		memcpy(record_data, &(dirent_buff[record_off]), _record_size_);
		return 0;
	}
	if (record_ptr < (10 + _dwords_per_block_)) {
		return read_record_single_ind(record_data, inode_data.singleIndPtr,
				record_ptr - 10, record_off);
	}
	if (record_ptr
			< (10 + _dwords_per_block_ + _dwords_per_block_ * _dwords_per_block_)) {
		return read_record_double_ind(record_data, inode_data.doubleIndPtr,
				record_ptr - 10 - _dwords_per_block_);
	}
	return 0;
}

int read_record_single_ind(struct t2fs_record *record_data, DWORD block,
		DWORD record_ptr, DWORD record_off) {
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	struct t2fs_record records[_records_per_block_];
	read_block((BYTE*) records, ptrs[record_ptr]);
	memcpy(record_data, &(records[record_off]), _record_size_);
	return 0;
}

int read_record_double_ind(struct t2fs_record *record_data, DWORD block,
		DWORD record_ptr, DWORD record_off) {
	DWORD single_ind_ptr = record_ptr / _dwords_per_block_;
	DWORD single_ind_off = record_ptr % _dwords_per_block_;
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	return read_record_single_ind(record_data, ptrs[single_ind_ptr],
			single_ind_off, record_off);
}

//apenas escreve na posição indicada, não aloca memória
int write_record(struct t2fs_record *record_data, DWORD inode, int position) {
	DWORD record_ptr = record_data_ptr(position);
	DWORD record_off = record_offset(position);
	struct t2fs_inode inode_data;
	read_inode(&inode_data, inode);
	if (record_ptr < 10) {
		struct t2fs_record records[_records_per_block_];
		int record_block = inode_data.dataPtr[record_ptr];
		read_block((BYTE*) records, record_block);
		memcpy(&(records[record_off]), record_data, _record_size_);
		write_block((BYTE*) records, record_block);
		return 0;
	}
	if (record_ptr < (10 + _dwords_per_block_)) {
		return write_record_single_ind(record_data, inode_data.singleIndPtr,
				record_ptr - 10, record_off);
	}
	if (record_ptr
			< (10 + _dwords_per_block_ + _dwords_per_block_ * _dwords_per_block_)) {
		return write_record_double_ind(record_data, inode_data.doubleIndPtr,
				record_ptr - 10 - _dwords_per_block_);
	}
	return -1;
}

int write_record_single_ind(struct t2fs_record *record_data, DWORD block,
		DWORD record_ptr, DWORD record_off) {
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	struct t2fs_record records[_records_per_block_];
	read_block((BYTE*) records, ptrs[record_ptr]);
	memcpy(&(records[record_off]), record_data, _record_size_);
	write_block((BYTE*) records, ptrs[record_ptr]);
	return 0;
}

int write_record_double_ind(struct t2fs_record *record_data, DWORD block,
		DWORD record_ptr, DWORD record_off) {
	DWORD single_ind_ptr = record_ptr / _dwords_per_block_;
	DWORD single_ind_off = record_ptr % _dwords_per_block_;
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	return write_record_single_ind(record_data, ptrs[single_ind_ptr],
			single_ind_off, record_off);
}

//retorna a posição do record se achou, ou -1 se não achou
int find_record(struct t2fs_record *record_data, DWORD inode, char *record_name) {
	struct t2fs_inode inode_data;
	int position = 0;
	read_inode(&inode_data, inode);
	if (find_record_data_ptr(record_data, &position, inode_data.dataPtr, 10,
			record_name) == 0)
		return position;
	if (find_record_single_ind(record_data, &position, inode_data.singleIndPtr,
			record_name) == 0)
		return position;
	if (find_record_double_ind(record_data, &position, inode_data.doubleIndPtr,
			record_name) == 0)
		return position;
	return -1;
}

int find_record_data_ptr(struct t2fs_record *record_data, int *position,
		DWORD *data_ptr, int data_size, char *record_name) {
	int i;
	for (i = 0; i < data_size; i++) {
		DWORD block = data_ptr[i];
		if (block == NULL_BLOCK)
			return -1;
		struct t2fs_record record_array[_records_per_block_];
		read_block((BYTE*) record_array, block);
		if (find_record_in_array(record_data, position, record_array,
				_records_per_block_, record_name) == 0) {
			return 0;
		}
	}
	return -1;
}

int find_record_in_array(struct t2fs_record *record_data, int *position,
		struct t2fs_record* record_array, int array_size, char *record_name) {
	int i;
	for (i = 0; i < array_size; i++) {
		if (record_array[i].TypeVal != TYPEVAL_INVALIDO
				&& strcmp(record_name, record_array[i].name) == 0) {
			memcpy(record_data, &(record_array[i]), _record_size_);
			return 0;
		}
		*position += 1;
	}
	return -1;
}

int find_record_single_ind(struct t2fs_record *record_data, int *position,
		DWORD block, char *record_name) {
	if (block == NULL_BLOCK)
		return -1;
	DWORD data_ptr[_dwords_per_block_];
	read_block((BYTE*) data_ptr, block);
	if (find_record_data_ptr(record_data, position, data_ptr,
			_dwords_per_block_, record_name) == 0)
		return 0;
	return -1;
}

int find_record_double_ind(struct t2fs_record *record_data, int *position,
		DWORD block, char *record_name) {
	if (block == NULL_BLOCK)
		return -1;
	DWORD block_ptr[_dwords_per_block_];
	read_block((BYTE*) block_ptr, block);
	int i;
	for (i = 0; i < _dwords_per_block_; i++) {
		if (block_ptr[i] == NULL_BLOCK)
			return -1;
		if (find_record_single_ind(record_data, position, block_ptr[i],
				record_name) == 0)
			return 0;
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

//apenas marca o bitmap do inode como ocupado e retorna o endereço
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

// essa função só libera o inode, não libera os blocos associados
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

// essa função libera o inode e todos os blocos associados
int deep_free_inode(DWORD inode) {
	struct t2fs_inode inode_data;
	if (read_inode(&inode_data, inode) != 0)
		return -1;
	int i;
	for (i = 0; i < 10; i++) {
		if (inode_data.dataPtr[i] != NULL_BLOCK)
			free_block(inode_data.dataPtr[i]);
	}
	if (inode_data.singleIndPtr != NULL_BLOCK)
		deep_free_single_ind(inode_data.singleIndPtr);
	if (inode_data.doubleIndPtr != NULL_BLOCK)
		deep_free_double_ind(inode_data.doubleIndPtr);
	free_inode(inode);
	return 0;
}

// libera um bloco de indireção simples e todos os blocos associados
int deep_free_single_ind(DWORD block) {
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	int i;
	for (i = 0; i < _dwords_per_block_; i++) {
		if (ptrs[i] != NULL_BLOCK)
			free_block(ptrs[i]);
	}
	free_block(block);
	return 0;
}

// libera um bloco de indireção dupla e todos os blocos associados
int deep_free_double_ind(DWORD block) {
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	int i;
	for (i = 0; i < _dwords_per_block_; i++) {
		if (ptrs[i] != NULL_BLOCK)
			deep_free_single_ind(ptrs[i]);
	}
	free_block(block);
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

void add_size_bytes(DWORD inode, DWORD bytes) {
	struct t2fs_record dot_record;
	int pos = find_record(&dot_record, inode, ".");
	dot_record.bytesFileSize += bytes;
	write_record(&dot_record, inode, pos);
}

void add_size_blocks(DWORD inode, DWORD blocks) {
	struct t2fs_record dot_record;
	int pos = find_record(&dot_record, inode, ".");
	dot_record.blocksFileSize += blocks;
	write_record(&dot_record, inode, pos);
}

int append_record(DWORD inode_ptr, struct t2fs_record *record) {
	struct t2fs_inode inode;
	read_inode(&inode, inode_ptr);
	struct t2fs_record r;
	if (find_record(&r, inode_ptr, record->name) >= 0)
		return -1;
	int i = 0;
	for (i = 0; i < 10; i++) {
		if (inode.dataPtr[i] == NULL_BLOCK) {
			inode.dataPtr[i] = create_record_block(record);
			write_inode(&inode, inode_ptr);
			add_size_blocks(inode_ptr, 1);
			add_size_bytes(inode_ptr, _record_size_);
			return 0;
		}
		if (append_record_block(inode.dataPtr[i], record) == 0) {
			add_size_bytes(inode_ptr, _record_size_);
			return 0;
		}
	}
	if (inode.singleIndPtr == NULL_BLOCK) {
		DWORD block = create_record_block(record);
		inode.singleIndPtr = create_single_ind_block(block);
		write_inode(&inode, inode_ptr);
		add_size_blocks(inode_ptr, 1);
		add_size_bytes(inode_ptr, _record_size_);
		return 0;
	}
	if (append_record_single_ind(inode.singleIndPtr, record, inode_ptr) == 0) {
		add_size_bytes(inode_ptr, _record_size_);
		return 0;
	}
	if (inode.doubleIndPtr == NULL_BLOCK) {
		DWORD block = create_record_block(record);
		inode.doubleIndPtr = create_double_ind_block(block);
		write_inode(&inode, inode_ptr);
		add_size_blocks(inode_ptr, 1);
		add_size_bytes(inode_ptr, _record_size_);
		return 0;
	}
	if (append_record_double_ind(inode.doubleIndPtr, record, inode_ptr) == 0) {
		add_size_bytes(inode_ptr, _record_size_);
		return 0;
	}
	return -1;
}

// lê o último registro, escreve um registro vazio no último
// e escreve o último registro por cima do que é pra ser removido
int remove_record(DWORD inode, int position) {
	int last_record_pos = inode_size_bytes(inode) / _record_size_ - 1;
	struct t2fs_record empty_record, last_record;
	record_init(&empty_record);
	if (last_record_pos != position) {
		read_record(&last_record, inode, last_record_pos);
		write_record(&last_record, inode, position);
	}
	write_record(&empty_record, inode, last_record_pos);
	add_size_bytes(inode, -_record_size_);
	return 0;
}

// aloca memória e insere como último record
int append_record_block(DWORD block, struct t2fs_record *record) {
	struct t2fs_record records[_records_per_block_];
	read_block((BYTE*) records, block);
	int i;
	for (i = 0; i < _records_per_block_; i++) {
		if (records[i].TypeVal == TYPEVAL_INVALIDO) {
			memcpy(&(records[i]), record, _record_size_);
			write_block((BYTE*) records, block);
			return 0;
		}
	}
	return -1;
}

int append_record_single_ind(DWORD block, struct t2fs_record *record,
		DWORD inode) {
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	int i;
	for (i = 0; i < _dwords_per_block_; i++) {
		if (ptrs[i] == NULL_BLOCK) {
			ptrs[i] = create_record_block(record);
			write_block((BYTE*) ptrs, block);
			add_size_blocks(inode, 1);
			return 0;
		}
		if (append_record_block(ptrs[i], record) == 0)
			return 0;
	}
	return -1;
}

int append_record_double_ind(DWORD block, struct t2fs_record *record,
		DWORD inode) {
	DWORD ptrs[_dwords_per_block_];
	read_block((BYTE*) ptrs, block);
	int i;
	for (i = 0; i < _dwords_per_block_; i++) {
		if (ptrs[i] == NULL_BLOCK) {
			DWORD block_ptr = create_record_block(record);
			ptrs[i] = create_single_ind_block(block_ptr);
			write_block((BYTE*) ptrs, block);
			add_size_blocks(inode, 1);
			return 0;
		}
		if (append_record_single_ind(ptrs[i], record, inode) == 0)
			return 0;
	}
	return -1;
}

// cria um bloco lógico de records contendo um record
DWORD create_record_block(struct t2fs_record *record) {
	DWORD block = alloc_block();
	if (block == NULL_BLOCK)
		return NULL_BLOCK;
	struct t2fs_record records[_records_per_block_];
	memcpy(&(records[0]), record, _record_size_);
	int i;
	for (i = 1; i < _records_per_block_; i++) {
		record_init(&(records[i]));
	}
	write_block((BYTE*) records, block);
	return block;
}

// cria um bloco de indireção simples contendu um ponteiro já
DWORD create_single_ind_block(DWORD first_ptr) {
	DWORD block = alloc_block();
	if (block == NULL_BLOCK)
		return NULL_BLOCK;
	DWORD ptrs[_dwords_per_block_];
	ptrs[0] = first_ptr;
	int i;
	for (i = 1; i < _dwords_per_block_; i++) {
		ptrs[i] = NULL_BLOCK;
	}
	write_block((BYTE*) ptrs, block);
	return block;
}

// cria um bloco de indireção dupla contendu um ponteiro já
// ou seja, cria um bloco com o primeiro ponteiro apontando para
// outro bloco que tem first_ptr como primeiro ponteiro
DWORD create_double_ind_block(DWORD first_ptr) {
	DWORD block = alloc_block();
	if (block == NULL_BLOCK)
		return NULL_BLOCK;
	DWORD ptrs[_dwords_per_block_];
	ptrs[0] = create_single_ind_block(first_ptr);
	int i;
	for (i = 1; i < _dwords_per_block_; i++) {
		ptrs[i] = NULL_BLOCK;
	}
	write_block((BYTE*) ptrs, block);
	return block;
}

void record_init(struct t2fs_record *record) {
	record->TypeVal = TYPEVAL_INVALIDO;
	record->blocksFileSize = 0;
	record->bytesFileSize = 0;
	record->name[0] = '\0';
	record->i_node = 0;
}

// retorna o inode correspondente ao curr_inode_ptr + path
// observação: path deve ser relativo (não pode começar com '/')
// para caminhos absolutos usar curr_inode_ptr = 0 e remover o '/' do início do path
DWORD find_dir_inode(DWORD curr_inode_ptr, char *path) {
	if (path[0] == '\0')
		return curr_inode_ptr;
	char* first_bar = strchr(path, '/');
	char* next_path = strtok(path, "/") + strlen(path) + 1;
	struct t2fs_inode curr_inode;
	read_inode(&curr_inode, curr_inode_ptr);
	struct t2fs_record record;
	if (find_record(&record, curr_inode_ptr, path) == -1)
		return NULL_BLOCK;
	if (record.TypeVal != TYPEVAL_DIRETORIO)
		return NULL_BLOCK;
	if (first_bar == NULL)
		return record.i_node;
	return find_dir_inode(record.i_node, next_path);
}

int last_occurrence(char* str, int ch) {
	char* pch = strchr(str, ch);
	char* last_occ = str;
	while (pch != NULL) {
		last_occ = pch;
		pch = strchr(pch + 1, ch);
	}
	int result = last_occ - str;
	return result;
}

int create_dir(DWORD parent_inode, char *dir_name) {
	if (dir_name[0] == '\0')
		return -1;
	struct t2fs_record child_dir, dot, dotdot;
	if (find_record(&child_dir, parent_inode, dir_name) >= 0)
		return -1;

	DWORD child_inode = alloc_inode();
	if (child_inode == NULL_BLOCK)
		return -1;
	struct t2fs_inode child_inode_data;
	inode_init(&child_inode_data);
	write_inode(&child_inode_data, child_inode);

	record_init(&child_dir);

	child_dir.TypeVal = TYPEVAL_DIRETORIO;
	strncpy(child_dir.name, dir_name, 30);
	child_dir.name[31] = '\0';
	child_dir.i_node = child_inode;

	record_init(&dot);
	dot.TypeVal = TYPEVAL_DIRETORIO;
	strcpy(dot.name, ".");
	dot.i_node = child_inode;

	record_init(&dotdot);
	dotdot.TypeVal = TYPEVAL_DIRETORIO;
	strcpy(dotdot.name, "..");
	dotdot.i_node = parent_inode;

	if (append_record(parent_inode, &child_dir) != 0)
		return -1;
	if (append_record(child_inode, &dot) != 0)
		return -1;
	return append_record(child_inode, &dotdot);
}

int remove_dir(DWORD parent_inode, char *file_name) {
	if (file_name == NULL || file_name[0] == '\0' || strcmp(file_name, ".") == 0
			|| strcmp(file_name, "..") == 0 || strcmp(file_name, "/") == 0) {
		return -1;
	}
	struct t2fs_record record;
	int pos = find_record(&record, parent_inode, file_name);
	if (!dir_is_empty(record.i_node))
		return -1;
	deep_free_inode(record.i_node);
	return remove_record(parent_inode, pos);
}

int dir_is_empty(DWORD inode) {
	struct t2fs_inode inode_data;
	read_inode(&inode_data, inode);
	struct t2fs_record records[_records_per_block_];
	read_block((BYTE*) records, inode_data.dataPtr[0]);
	return records[2].TypeVal == TYPEVAL_INVALIDO;
}
