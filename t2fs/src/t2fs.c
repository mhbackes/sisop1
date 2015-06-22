#include "../include/t2fs.h"
#include "../include/t2fs_aux.h"
#include <string.h>

#define MAX_CWD 1024
char _cwd_[MAX_CWD] = "/";

#define MAX_DIR 20
typedef struct t2fs_dir_data {
	int busy;
	int curr_entry;
	DWORD inode;
} DIR_DATA;

DIR_DATA _opened_dir_[MAX_DIR];

int identify2(char *name, int size) {
	strncpy(name,
			"Luis Pedro Silvestrin 228528, Marcos Henrique Backes 228483, Thiago Bell Felix de Oliveira 228527",
			size);
	name[size - 1] = '\0';
	return 0;
}

FILE2 create2(char *filename) {
	if (!_initialized_)
		init();
	return -1;
}
int delete2(char *filename) {
	if (!_initialized_)
		init();
	return -1;
}
FILE2 open2(char *filename) {
	if (!_initialized_)
		init();
	return -1;
}
int close2(FILE2 handle) {
	if (!_initialized_)
		init();
}
int read2(FILE2 handle, char *buffer, int size) {
	if (!_initialized_)
		init();
	return -1;
}
int write2(FILE2 handle, char *buffer, int size) {
	if (!_initialized_)
		init();
	return -1;
}
int seek2(FILE2 handle, unsigned int offset) {
	if (!_initialized_)
		init();
	return -1;
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
	dentry->fileSize = inode_size_blocks(record.i_node) * _super_block_.BlockSize;
	switch(record.TypeVal){
	case TYPEVAL_DIRETORIO:
		dentry->fileType = 1;
		break;
	case TYPEVAL_REGULAR:
		dentry->fileType = 0;
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
