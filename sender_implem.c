#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "main_header.h"
#include "error_handle.h"
#include "sender.h"

int send_input_dir_and_its_content(int fifo_fd, char *log_file, char *directory, int pipe_size) {
	DIR *dir;
	struct dirent *entry;
	char entry_name[STRING_SIZE], path[STRING_SIZE], log_file_buffer[STRING_SIZE*10];
	int file_size;

	dir = opendir(directory);
	if (dir == NULL) {
		opendir_error(directory);
		exit(E_OPEN_DIR);
	}
	//DFS on input_dir
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			continue;
		}
		sprintf(path,"%s/%s", directory, entry->d_name);
		//e.g. path:1_input1/dir1/dir2/f1 ,entry_name:dir1/dir2/f1 
		cut_first_part_of_the_path(path, entry_name);		
		send_name_length(fifo_fd, entry_name);
		send_name(fifo_fd, entry_name);		
		send_entry_size(fifo_fd, path);
		if (entry->d_type == DT_REG) {
			send_indication_bit(fifo_fd, FILE_BIT);
			send_file(fifo_fd, path, pipe_size);
			sprintf(log_file_buffer, "sent: %s %d\n", entry_name, get_entry_size(path));	
			append_to_log_file(log_file, log_file_buffer);
		}
		else {
			send_indication_bit(fifo_fd, DIRECTORY_BIT);
			send_input_dir_and_its_content(fifo_fd, log_file, path, pipe_size);
		}
	}
	closedir(dir);
	return 0;
}

int send_name_length(int fifo_fd, char *entry_name) {
	int bytes, entry_name_length;
	entry_name_length = strlen(entry_name) + 1;
	if (fits_in_2_bytes(entry_name_length) == TRUE) {
		write_to_pipe(fifo_fd, &entry_name_length, BYTES_OF_ENTRY_NAME_LENGTH);
	}
	else {
		name_length_error(entry_name);
		exit(E_NAME_LENGTH);
	}
	return bytes;
}

int send_name(int fifo_fd, char *name) {
	return write_to_pipe(fifo_fd, name, strlen(name) + 1);
}

int send_entry_size(int fifo_fd, char *entry_name) {
	int size = get_entry_size(entry_name);
	return write_to_pipe(fifo_fd, &size, BYTES_OF_FILE_SIZE);
}

int send_indication_bit(int fifo_fd, int indication_bit) {
	return write_to_pipe(fifo_fd, &indication_bit, 1);
}

void send_file(int fifo_fd, char *file_name, int pipe_size) {
	char pipe_buffer[pipe_size];
	int bytes, file_size, fd, written_bytes;

	fd = open(file_name, O_RDONLY);
	if (fd == -1 ) {
		file_open_error(file_name);
		exit(E_FILE_OPEN);
	}
	file_size = get_entry_size_by_fd(fd);
	written_bytes = 0;
	while(written_bytes < file_size) {
		bytes = read_from_file(fd, pipe_buffer, pipe_size);
		write_to_pipe(fifo_fd, pipe_buffer, bytes);
		written_bytes += bytes;
	}
	close(fd);
}

void send_termination_message(int fifo_fd) {
	int termination_msg = TERMINATION_MESSAGE;
	write_to_pipe(fifo_fd, &termination_msg, BYTES_OF_TERMINATION_MSG);
}

int write_to_pipe(int fifo_fd, void *buffer,int length) {
	int bytes;
	bytes = write(fifo_fd, buffer, length);
	if (bytes == -1){
		pipe_write_error(buffer);
		exit(E_PIPE_WRITE); 
	}
	return bytes;
}

int read_from_file(int fd, void *buffer,int length) {
	int bytes;
	bytes = read(fd, buffer, length);
	if (bytes == -1){
		file_read_error();
		exit(E_FILE_READ); 
	}
	return bytes;
}

int fits_in_2_bytes(int x) {
	return x <= MAX_INT_OF_2_BYTE ? TRUE:FALSE; 
}

int get_entry_size(char *entry_name) {
	struct stat st;
	int ret_val;
	ret_val = stat(entry_name, &st);
	if (ret_val == -1) {
		stat_error(entry_name);
		exit(E_STAT);
	}
	return st.st_size;
}

int get_entry_size_by_fd(int fd) {
	struct stat st;
	int ret_val;
	ret_val = fstat(fd, &st);
	if (ret_val == -1) {
		fstat_error();
		exit(E_FSTAT);
	}
	return st.st_size;
}