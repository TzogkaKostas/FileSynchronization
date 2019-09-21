#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "main_header.h"
#include "error_handle.h"

	//error functions
void num_arguments_error(int argc) {
	printf("Wrong number of aguments.%d were given.\n", argc);
	printf("Usage: ./mirror_client -n id -c common_dir -i input_dir -m mirror_dir "
		"-b buffer_size -l log_file\n");
}


void input_dir_error(char *file_name) {
	printf("Input_dir:%s directory doesn't exist.\n", file_name);
}

void mirror_dir_error(char *file_name) {
	printf("Mirror_dir:%s directory already exists.\n", file_name);
}

void create_dir_error(char *directory_name) {
	printf("mkdir:%s failed.%s.\n",directory_name, strerror(errno));
}

void file_exists_error(char *file_name) {
	printf("%s file already exists.\n", file_name);
}

void create_file_error(char *file_name) {
	printf("fopen:%s file open error\n.%s.",file_name, strerror(errno));
}

void file_open_error(char *file_name) {
	printf("File open error:%s.%s.\n", file_name, strerror(errno));
}

void fork_error() {
	printf("Fork error:%s.\n",strerror(errno));
}

void opendir_error(char *directory) {
	printf("opendir error: %s.%s.\n", directory, strerror(errno));
}

void mkfifo_error(char *fifo_name) {
	printf("mkfifo error: %s.%s.\n", fifo_name, strerror(errno));
}

void open_fd_error(char *file_name) {
	printf("open fd error:%s.%s\n", file_name, strerror(errno));
}

void execl_error(char *executable) {
	printf("execl error:%s.%s.\n", executable, strerror(errno));
}

void pipe_write_error(char *string) {
	printf("pipe write error:%s.%s.\n", string, strerror(errno));
}

void pipe_read_error() {
	printf("pipe read error: %s.\n", strerror(errno));
}

void name_length_error(char *entry) {
	printf("name length error:%s.%s.\n",entry, strerror(errno));
}
	
void file_read_error() {
	printf("file read error:%s.\n",strerror(errno));
}

void file_write_error() {
	printf("file write error:%s.\n", strerror(errno));
}

void stat_error(char *name) {
	printf("stat error:%s.%s\n", name, strerror(errno));
}

void fstat_error() {
	printf("fstat error:%s.\n", strerror(errno));
}

void file_creat_error(char *name) {
	printf("file creat error:%s.%s.\n", name, strerror(errno));
}

void malloc_error() {
	printf("malloc error:%s.\n", strerror(errno));
}

void remove_error(char *name) {
	printf("remove error:%s.%s.\n", name, strerror(errno));
}

void poll_error() {
	printf("poll error:%s.\n", strerror(errno));
}

void signal_error() {
	printf("signal error:%s.\n", strerror(errno));
}


	//error checking functions
int arguments_checking(Arguments *arguments) {
	if (directory_exists(get_input_dir(arguments)) == FALSE) {
		input_dir_error(get_input_dir(arguments));
		exit(E_INPUT_DIR);
	}

	if (directory_exists(get_mirror_dir(arguments)) == TRUE) {
		mirror_dir_error(get_mirror_dir(arguments));
		exit(E_MIRROR_DIR);
	}
	else {
		if (create_directory(get_mirror_dir(arguments)) != SUCCESS) {
			create_dir_error(get_mirror_dir(arguments));
			exit(E_CREATE_DIR);
		}
	}

	if (directory_exists(get_common_dir(arguments)) == FALSE) {
		if (create_directory(get_common_dir(arguments)) != SUCCESS) {
			create_dir_error(get_common_dir(arguments));
			exit(E_CREATE_DIR);
		}
	}
	return SUCCESS;
}

int directory_exists(char *directory_name) {
	DIR *dir = opendir(directory_name);
	int exists;
	if (dir == NULL)  {
		exists = FALSE;
	}
	else {
		exists = TRUE;
	}
	closedir(dir);
	return exists;
}

int file_exists(char *filename) {
	if(access(filename,F_OK) == -1 ) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}