#include "../include/t2fs.h"
#include "../include/t2fs_aux.h"
#include "../include/apidisk.h"
#include <string.h>

int identify2(char *name, int size) {
	strncpy(name,
			"Luis Pedro Silvestrin 228528, Marcos Henrique Backes 228483, Thiago Bell Felix de Oliveira 228527",
			size);
	name[size - 1] = '\0';
	return 0;
}

int mkdir2 (char *pathname) {
	if(pathname == NULL)
		return -1;
	char cpypathname[strlen(pathname) + 1];
	strcpy(cpypathname, pathname);
	DWORD curr_inode;
	char* path;
	if(cpypathname[0] == '/'){
		curr_inode = 0;
		path = cpypathname + 1;
	} else {
		curr_inode = _current_dir_inode_;
		path = cpypathname;
	}
	int last_occ = last_occurrence(path, '/');
	DWORD parent_inode;
	char* file_name;
	if(last_occ == 0){
		file_name = path;
		parent_inode = curr_inode;
	} else {
		path[last_occ] = '\0';
		file_name = path + last_occ + 1;
		parent_inode = find_dir_inode(curr_inode, path);
	}
	if(parent_inode == NULL_BLOCK)
		return -1;
	return create_dir(parent_inode, file_name);
}
