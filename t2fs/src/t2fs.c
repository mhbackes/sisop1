#include "../include/t2fs.h"
#include "../include/t2fs_aux.h"
#include <string.h>
#include <stdio.h>


#define MAX_CWD 1024
char _cwd_[MAX_CWD] = "/";

#define NULL_BLOCK 0x0FFFFFFFF

/*opened directories handles*/
#define MAX_DIR 20
typedef struct t2fs_dir_data {
	int busy; //indicates if this entry in being used
	int curr_entry;
	DWORD inode;
} DIR_DATA;

DIR_DATA _opened_dir_[MAX_DIR];

/*opened files handles*/
#define MAX_FILE 20
typedef struct t2fs_file_data {
	int busy;//indicates if this entry in being used
	DWORD curr_pointer;
	DWORD inode;
	DWORD parent_inode;
	struct t2fs_record record;
} FILE_DATA;

FILE_DATA _opened_file_[MAX_FILE];

int identify2(char *name, int size) {
	strncpy(name,
			"Luis Pedro Silvestrin 228528, Marcos Henrique Backes 228483, Thiago Bell Felix de Oliveira 228527",
			size);
	name[size - 1] = '\0';
	return 0;
}

FILE2 create2(char *filename) {
	if (!_initialized_){
		init();		
	}
	char *c;
	c = filename;
	int namesize = 0;
	//checks if filename is valid
	while(*c!='\0'){
		namesize++;
		if(*c<33 || *c>122 || namesize > 30){
			printf("hi2 %c size:%d", *c,namesize);
			printf("invalid filename");
			return -1;
		}
		c++;
	}
	struct t2fs_record record;
	
	//pega inode do diretorio atual
	DWORD parent_inode_addr;
	parent_inode_addr = find_dir_inode(0, _cwd_+1);
	
	
	//checks if file already exists
	
	if(find_record(&record, parent_inode_addr, filename)!=-1){
		//arquivo já existe
		printf("file already exists");
		return -1;
	}

	FILE2 handle = get_empty_file_handle();
	
	if(handle == -1){
		printf("too many files opened");
		//no space in memory structure
		return -1;
	}
	
	record.TypeVal = TYPEVAL_REGULAR; //arquivo
	strncpy(record.name, filename, namesize+1);
	record.blocksFileSize = 0;
	record.bytesFileSize = 0;
	//creates inode for file
	DWORD file_inode_addr = alloc_inode();
	inode_init(file_inode_addr);
	record.i_node = file_inode_addr;
	append_record(parent_inode_addr, &record);
	
	struct t2fs_inode file_inode;
	inode_init(&file_inode); 
	write_inode(&file_inode, file_inode_addr);
	
	_opened_file_[handle].busy = 1;
	_opened_file_[handle].curr_pointer = 0;
	_opened_file_[handle].inode = file_inode_addr;
	_opened_file_[handle].parent_inode = parent_inode_addr;
	_opened_file_[handle].record = record;
	return handle;
}

//return an valid empty file handle. If none is free, returns -1
FILE2 get_empty_file_handle(){
	int ind=0;
	while(ind<MAX_FILE) {
		if(_opened_file_[ind].busy==0){
			return ind;
		}
		ind++;
	}
	return -1;	
}

int delete2(char *filename) {
	if (!_initialized_)
		init();
	//pega inode do diretorio atual
	DWORD parent_inode_addr;
	parent_inode_addr = find_dir_inode(0, _cwd_+1);
	
	//checks if file  exists
	struct t2fs_record record;
	int position=find_record(&record, parent_inode_addr, filename);
	if(position!=-1){
	  
		deep_free_inode(record.i_node);
		//remove registro
		remove_record(parent_inode_addr,position);
		//remove da tabelas de arquivos abertos
		int i =0;
		for(i=0;i<MAX_FILE; i++){
			if(_opened_file_[i].busy==1){
				if(_opened_file_[i].inode == record.i_node){
					_opened_file_[i].busy=0;
					break;
				}
			}
		}
		return 0;
	}
	return -1;

}

FILE2 open2(char *filename) {
	if (!_initialized_)
		init();
	DWORD curr_dir_inode_addr = find_dir_inode(0, _cwd_+1);

	struct t2fs_record rec;
	int pos = find_record(&rec, _current_dir_inode_, filename);
	if (pos == -1) {
		//printf("file doesn't exists");
		return -1;	// file doesn't exists		
	}
	int i = 0;
	for (i=0; i<MAX_FILE; i++)
		if (_opened_file_[i].inode == rec.i_node)
			break;
		
	if (i != MAX_FILE && _opened_file_[i].busy) {
		//printf("file already opened");
		return i; 	// file already opened
	}
	FILE2 handle = get_empty_file_handle();
	if (handle == -1) {
		//printf("no space");
		return -1;	// no space in memory structure
	}
	_opened_file_[handle].busy = 1;
	_opened_file_[handle].inode = rec.i_node;
	_opened_file_[handle].curr_pointer = 0;
	_opened_file_[handle].parent_inode = _current_dir_inode_;
	_opened_file_[handle].record = rec;
	
	//printf("the size of the file is: %d bytes \n",_opened_file_[handle].record.bytesFileSize );

	return handle;	
}

int close2(FILE2 handle) {
	if (!_initialized_)
		init();
	if (handle >= MAX_FILE || handle < 0)
		return -1;
	
	_opened_file_[handle].busy = 0;
	return 0;
}

int read2(FILE2 handle, char *buffer, int size) {
	if (!_initialized_)
		init();
	
	//checks if handle is valid
	if(_opened_file_[handle].busy!=1 || size<0){
		return -1;
	}
	
	//checks how much of file will be read
	int bytes_to_read = size;
	if(_opened_file_[handle].curr_pointer+size > _opened_file_[handle].record.bytesFileSize){
		bytes_to_read = _opened_file_[handle].record.bytesFileSize - _opened_file_[handle].curr_pointer;
	}
	//printf("file has size: %d\n", _opened_file_[handle].record.bytesFileSize);
	//printf("will read %d bytes\n", bytes_to_read);
	

	//if region starts on middle of a block must read it first
	int bytes_left = bytes_to_read;
	
	while(bytes_left>0){
		DWORD current_block = _opened_file_[handle].curr_pointer / _super_block_.BlockSize;
		DWORD current_offset = _opened_file_[handle].curr_pointer % _super_block_.BlockSize;
		BYTE block[_super_block_.BlockSize];
		//reads block from disk
		if(read_file_block(_opened_file_[handle].inode,current_block,block, _super_block_.BlockSize)<0)
			   return -1;

		//amount that can be read from actual block
		int readable_bytes = _super_block_.BlockSize - current_offset; 
		int amount_to_read_in_block;
		BYTE *start; //address from which to start reading
		start = block+current_offset;
		if(readable_bytes>bytes_left)
		{
			//must not read entire block
			amount_to_read_in_block = bytes_left;
		} else{
			//read until the end
			amount_to_read_in_block = readable_bytes;
		}	
		memcpy(buffer, start, amount_to_read_in_block);
		//printf("first value in buffer %d\n ",(int)(*buffer));
		bytes_left -= amount_to_read_in_block;
		buffer = buffer + amount_to_read_in_block;
		_opened_file_[handle].curr_pointer += amount_to_read_in_block;
	}

	return bytes_to_read;
}
int write2(FILE2 handle, char *buffer, int size) {
	if (!_initialized_)
		init();
	
	if(handle<0 || handle >= MAX_FILE){
		return -1;
	}

	DWORD file_inode_addr = _opened_file_[handle].inode;
	int increase_in_size_bytes =0;
	int increase_in_size_blocks = 0;
	if(_opened_file_[handle].record.bytesFileSize< size + _opened_file_[handle].curr_pointer) {
		//blocks need to be allocated
		increase_in_size_bytes = size + _opened_file_[handle].curr_pointer - _opened_file_[handle].record.bytesFileSize; 
	}
	
	
	BYTE block_buffer[_super_block_.BlockSize];
	int bytes_to_write = size; //number of bytes that still have to be written
	
	//block to which current pointer points
	DWORD curr_pointer_block = _opened_file_[handle].curr_pointer / _super_block_.BlockSize;
	//ofset from the start of the block
	DWORD curr_pointer_offset = _opened_file_[handle].curr_pointer % _super_block_.BlockSize;

	//current pointer may be in the middle of a block. in such case this block must
	//be read, modified and written
	//never increases number of blocks
	if(curr_pointer_offset>0){
		int bytes_to_copy;
		if(bytes_to_write< _super_block_.BlockSize-curr_pointer_offset) // in case of not writing until the end
		{								//of the block
			bytes_to_copy = bytes_to_write;
		} else {
			bytes_to_copy = _super_block_.BlockSize-curr_pointer_offset;
		}
		
		if(read_file_block(file_inode_addr, curr_pointer_block,block_buffer,_super_block_.BlockSize)!=0){
			return -1;
		}
		memcpy(block_buffer+curr_pointer_offset,buffer,bytes_to_copy);
		buffer = buffer+bytes_to_copy;
		bytes_to_write = bytes_to_write - bytes_to_copy;
		//writes block back
		if(write_file_block(file_inode_addr,curr_pointer_block,block_buffer)!=0)
		{ 
			return -1;
		}
		_opened_file_[handle].curr_pointer += bytes_to_copy;

	}
	
	//writes rest of bytes
	while(bytes_to_write>0){		
		//recalculates pointers
		curr_pointer_block = _opened_file_[handle].curr_pointer / _super_block_.BlockSize;
		curr_pointer_offset = _opened_file_[handle].curr_pointer % _super_block_.BlockSize;
		int bytes_to_copy;
		if(bytes_to_write< _super_block_.BlockSize-curr_pointer_offset) // in case of not writing until the end
		{//of the block
			bytes_to_copy = bytes_to_write;
		} else {
			bytes_to_copy = _super_block_.BlockSize-curr_pointer_offset;
		}
		
		if(curr_pointer_block+1 > _opened_file_[handle].record.blocksFileSize){
			//needs to create block in file
			increase_in_size_blocks++;
			create_next_file_block(file_inode_addr);
			memcpy(block_buffer,buffer,bytes_to_copy);
			if(write_file_block(file_inode_addr,curr_pointer_block,block_buffer)!=0)
			{ 
				return -1;
			}
			
		} else {
			//no need for new block
			if(read_file_block(file_inode_addr, curr_pointer_block,block_buffer,_super_block_.BlockSize)!=0){
				return -1;
			}
			memcpy(block_buffer,buffer,bytes_to_copy);
			if(write_file_block(file_inode_addr,curr_pointer_block,block_buffer)!=0)
			{ 
				return -1;
			}
		}
		buffer = buffer + bytes_to_copy;
		bytes_to_write -= bytes_to_copy;
		_opened_file_[handle].curr_pointer += bytes_to_copy;
		
	}
	//saves new size
	_opened_file_[handle].record.blocksFileSize += increase_in_size_blocks;
	_opened_file_[handle].record.bytesFileSize += increase_in_size_bytes;

	struct t2fs_record dummy;
	DWORD parent_inode = _opened_file_[handle].parent_inode;
	int record_position = find_record(&dummy,parent_inode, _opened_file_[handle].record.name);
	int result = write_record(&_opened_file_[handle].record, parent_inode,record_position);

	return size;
}
int seek2(FILE2 handle, unsigned int offset) {
	if (!_initialized_)
		init();
	if (handle >= MAX_FILE || handle < 0)
		return -1;
	if(offset == -1){
		_opened_file_[handle].curr_pointer = _opened_file_[handle].record.bytesFileSize;
	} else if (offset >= 0) {
		_opened_file_[handle].curr_pointer = offset;
	} else {
		return -1;	
	}
	return 0;
}

int mkdir2(char *pathname) {
	if (!_initialized_)
		init();
	if (pathname == NULL || pathname[0] == '\0')
		return -1;
	char cpypathname[strlen(pathname) + 1];
	strcpy(cpypathname, pathname);
	DWORD curr_inode;
	char* path;
	if (cpypathname[0] == '/') {
		curr_inode = 0;
		path = cpypathname + 1;
	} else {
		curr_inode = _current_dir_inode_;
		path = cpypathname;
	}
	int last_occ = last_occurrence(path, '/');
	DWORD parent_inode;
	char* file_name;
	if (last_occ == 0) {
		file_name = path;
		parent_inode = curr_inode;
	} else {
		path[last_occ] = '\0';
		file_name = path + last_occ + 1;
		parent_inode = find_dir_inode(curr_inode, path);
	}
	if (parent_inode == NULL_BLOCK)
		return -1;
	return create_dir(parent_inode, file_name);
}

DIR2 opendir2(char *pathname) {
	if (!_initialized_)
		init();
	char cpypathname[strlen(pathname) + 1], *path;
	strcpy(cpypathname, pathname);
	int curr_inode, dir_inode;
	if (cpypathname[0] == '/') {
		curr_inode = 0;
		path = cpypathname + 1;
	} else {
		curr_inode = _current_dir_inode_;
		path = cpypathname;
	}
	dir_inode = find_dir_inode(curr_inode, path);
	if(dir_inode == NULL_BLOCK)
		return -1;
	int i;
	for (i = 0; i < MAX_DIR; i++) {
		if (!_opened_dir_[i].busy) {
			_opened_dir_[i].inode = dir_inode;
			_opened_dir_[i].busy = 1;
			_opened_dir_[i].curr_entry = 0;
			return (DIR2) i;
		}
	}
	return -1;
}

int readdir2(DIR2 handle, DIRENT2 *dentry) {
	if (!_initialized_)
		init();
	if (handle < 0 || handle >= MAX_DIR)
		return -1;
	if (!_opened_dir_[handle].busy)
		return -1;
	DWORD inode = _opened_dir_[handle].inode;
	int entry = _opened_dir_[handle].curr_entry;
	struct t2fs_record record;
	if(read_record(&record, inode, entry) != 0)
		return -1;
	switch(record.TypeVal){
	case TYPEVAL_DIRETORIO:
		dentry->fileType = 1;
		dentry->fileSize = inode_dir_size_blocks(record.i_node) * _super_block_.BlockSize;
		break;
	case TYPEVAL_REGULAR:
		dentry->fileType = 0;
		dentry->fileSize = record.blocksFileSize * _super_block_.BlockSize;
		break;
	default:
		return -1;
	}
	strcpy(dentry->name, record.name);
	_opened_dir_[handle].curr_entry++;
	return 0;
}

int closedir2(DIR2 handle) {
	if (!_initialized_)
		init();
	if (handle < 0 || handle >= MAX_DIR)
		return -1;
	if (!_opened_dir_[handle].busy)
		return -1;
	_opened_dir_[handle].busy = 0;
	return 0;
}

int rmdir2(char *pathname) {
	if (!_initialized_)
		init();
	if (pathname == NULL || pathname[0] == '\0')
		return -1;
	char cpypathname[strlen(pathname) + 1];
	strcpy(cpypathname, pathname);
	DWORD curr_inode;
	char* path;
	if (cpypathname[0] == '/') {
		curr_inode = 0;
		path = cpypathname + 1;
	} else {
		curr_inode = _current_dir_inode_;
		path = cpypathname;
	}
	int last_occ = last_occurrence(path, '/');
	DWORD parent_inode;
	char* file_name;
	if (last_occ == 0) {
		file_name = path;
		parent_inode = curr_inode;
	} else {
		path[last_occ] = '\0';
		file_name = path + last_occ + 1;
		parent_inode = find_dir_inode(curr_inode, path);
	}
	if (parent_inode == NULL_BLOCK)
		return -1;
	return remove_dir(parent_inode, file_name);
}

int chdir2(char *pathname) {
	if (!_initialized_)
		init();
	if (pathname == NULL || pathname[0] == '\0')
		return -1;
	char cwd_cpy[MAX_CWD];
	strcpy(cwd_cpy, _cwd_);
	char pathname_cpy[strlen(pathname) + 1];
	strcpy(pathname_cpy, pathname);
	DWORD curr_inode;
	char* path;
	if (pathname_cpy[0] == '/') {
		curr_inode = 0;
		path = pathname_cpy + 1;
	} else {
		curr_inode = _current_dir_inode_;
		path = pathname_cpy;
	}
	char *first_bar, *next_path;
	do {
		first_bar = strchr(path, '/');
		next_path = strtok(path, "/") + strlen(path) + 1;
		if ((curr_inode = find_dir_inode(curr_inode, path)) == NULL_BLOCK)
			return -1;
		if (strcmp(path, "..") == 0) {
			if (strcmp(cwd_cpy, "/") != 0)
				cwd_cpy[last_occurrence(cwd_cpy, '/')] = '\0';
			if (cwd_cpy[0] == '\0')
				strcpy(cwd_cpy, "/");
		} else if (strcmp(path, ".") != 0) {
			if (strcmp(cwd_cpy, "/") != 0)
				strncat(cwd_cpy, "/", MAX_CWD);
			strncat(cwd_cpy, path, MAX_CWD);
		}
		path = next_path;
	} while (first_bar != NULL);
	strcpy(_cwd_, cwd_cpy);
	_current_dir_inode_ = curr_inode;
	return 0;
}

int getcwd2(char *pathname, int size) {
	if (!_initialized_)
		init();
	strncpy(pathname, _cwd_, size);
	pathname[size - 1] = '\0';
	if (size < strlen(_cwd_))
		return -1;
	return 0;
}
