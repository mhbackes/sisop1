#include "../include/t2fs.h"
#include "../include/t2fs_aux.h"
#include "../include/apidisk.h"
#include <string.h>

char _cwd_[1024] = "/";

int identify2(char *name, int size) {
	strncpy(name,
			"Luis Pedro Silvestrin 228528, Marcos Henrique Backes 228483, Thiago Bell Felix de Oliveira 228527",
			size);
	name[size - 1] = '\0';
	return 0;
}

FILE2 create2(char *filename) {
}
int delete2(char *filename) {
}
FILE2 open2(char *filename) {
}
int close2(FILE2 handle) {
}
int read2(FILE2 handle, char *buffer, int size) {
}
int write2(FILE2 handle, char *buffer, int size) {
}
int seek2(FILE2 handle, unsigned int offset) {
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

DIR2 opendir2(char *pathname){}
int readdir2(DIR2 handle, DIRENT2 *dentry){}
int closedir2(DIR2 handle){}

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
	char cwd_cpy[1024];
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
				strncat(cwd_cpy, "/", 1024);
			strncat(cwd_cpy, path, 1024);
		}
		path = next_path;
	} while (first_bar != NULL);
	strcpy(_cwd_, cwd_cpy);
	_current_dir_inode_ = curr_inode;
	return 0;
}

int getcwd2(char *pathname, int size) {
	strncpy(pathname, _cwd_, size);
	pathname[size - 1] = '\0';
	if (size < strlen(_cwd_))
		return -1;
	return 0;
}
