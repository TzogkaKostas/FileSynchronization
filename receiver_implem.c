#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <sys/file.h>

#include "main_header.h"
#include "error_handle.h"
#include "receiver.h"

int receive_input_dir_and_its_content(int fifo_fd, int pipe_size, char *mirrod_id_dir,
		char *log_file) {
	char entry_name[STRING_SIZE], log_file_buffer[STRING_SIZE*10];
	int entry_name_length, entry_size, indication_bit, ret_val;
	entry_name_length = 0;

	ret_val = try_receive_name_length(fifo_fd, &entry_name_length);
	if (ret_val < 0) {return ret_val;}

	while(entry_name_length != TERMINATION_MESSAGE) {
		ret_val = try_receive_name(fifo_fd, entry_name, entry_name_length);
		if (ret_val < 0) {return ret_val;}

		ret_val = try_receive_entry_size(fifo_fd, &entry_size);
		if (ret_val < 0) {return ret_val;}
		
		ret_val = try_receive_indication_bit(fifo_fd, &indication_bit);
		if (ret_val < 0) {return ret_val;}

		if (indication_bit == FILE_BIT) {
			ret_val = try_receive_file(fifo_fd, mirrod_id_dir, entry_name, entry_size, pipe_size);
			if (ret_val < 0) {return ret_val;}

			sprintf(log_file_buffer, "received: %s/%s %d\n", mirrod_id_dir, entry_name, entry_size);	
			ret_val = try_append_to_log_file(log_file, log_file_buffer);
			if (ret_val < 0) {return ret_val;}
		}
		else {
			ret_val = receive_directory(mirrod_id_dir, entry_name);
			if (ret_val < 0) {return ret_val;}
		}
		entry_name_length = 0;
		ret_val = try_receive_name_length(fifo_fd, &entry_name_length);
		if (ret_val < 0) {return ret_val;}
	}
	return SUCCESS;
}

int receive_name_length(int fifo_fd, int *entry_name_length) {
	return read_from_pipe(fifo_fd, entry_name_length, BYTES_OF_ENTRY_NAME_LENGTH);
}

int try_receive_name_length(int fifo_fd, int *entry_name_length) {
	return try_read_from_pipe(fifo_fd, entry_name_length, BYTES_OF_ENTRY_NAME_LENGTH);
}

int receive_name(int fifo_fd, char *entry_name, int entry_name_length) {
	return read_from_pipe(fifo_fd, entry_name, entry_name_length);
}

int try_receive_name(int fifo_fd, char *entry_name, int entry_name_length) {
	return try_read_from_pipe(fifo_fd, entry_name, entry_name_length);
}

int receive_entry_size(int fifo_fd, int *entry_size) {
	return read_from_pipe(fifo_fd, entry_size, BYTES_OF_FILE_SIZE);
}

int try_receive_entry_size(int fifo_fd, int *entry_size) {
	return try_read_from_pipe(fifo_fd, entry_size, BYTES_OF_FILE_SIZE);
}

int receive_indication_bit(int fifo_fd, int *indication_bit) {
	return read_from_pipe(fifo_fd, indication_bit, 1);
}

int try_receive_indication_bit(int fifo_fd, int *indication_bit) {
	return try_read_from_pipe(fifo_fd, indication_bit, 1);
}

int receive_file(int fifo_fd, char *mirrod_id_dir, char *file_name, int file_size, int pipe_size) {
	char pipe_buffer[pipe_size];
	char received_file_path[STRING_SIZE];
	int bytes, read_bytes, fd, ret_val;

	if (directory_exists(mirrod_id_dir) == FALSE) {
		ret_val = create_directory(mirrod_id_dir);
		if (ret_val != SUCCESS) {return E_CREATE_DIR;}
	}
	sprintf(received_file_path, "%s/%s", mirrod_id_dir, file_name);
	fd = creat(received_file_path, S_IRWXU);
	if (fd == -1 ) {
		file_creat_error(received_file_path);
		return E_CREATE_FILE;
	}

	read_bytes = 0;
	while(read_bytes < file_size) {
		if (file_size - read_bytes >= pipe_size) {
			bytes = read_from_pipe(fifo_fd, pipe_buffer, pipe_size);
			if (bytes < 0) {return bytes;}
		}
		else {
			bytes = read_from_pipe(fifo_fd, pipe_buffer, file_size - read_bytes);
			if (bytes < 0) {return bytes;}
		}
		ret_val = write_to_file(fd, pipe_buffer, bytes);
		if (ret_val < 0) {return ret_val;}
		read_bytes += bytes;
	}
	close(fd);
	return SUCCESS;
}

int try_receive_file(int fifo_fd, char *mirrod_id_dir, char *file_name, int file_size, int pipe_size) {
	char pipe_buffer[pipe_size];
	char received_file_path[STRING_SIZE];
	int bytes, read_bytes, fd, ret_val;

	if (directory_exists(mirrod_id_dir) == FALSE) {
		ret_val = create_directory(mirrod_id_dir);
		if (ret_val != SUCCESS) {return E_CREATE_DIR;}
	}
	sprintf(received_file_path, "%s/%s", mirrod_id_dir, file_name);
	fd = creat(received_file_path, S_IRWXU);
	if (fd == -1 ) {
		file_creat_error(received_file_path);
		return E_CREATE_FILE;
	}

	read_bytes = 0;
	while(read_bytes < file_size) {
		if (file_size - read_bytes >= pipe_size) {
			bytes = try_read_from_pipe(fifo_fd, pipe_buffer, pipe_size);
			if (bytes < 0) {return bytes;}
		}
		else {
			bytes = try_read_from_pipe(fifo_fd, pipe_buffer, file_size - read_bytes);
			if (bytes < 0) {return bytes;}
		}
		ret_val = write_to_file(fd, pipe_buffer, bytes);
		if (ret_val < 0) {return ret_val;}
		read_bytes += bytes;
	}
	close(fd);
	return SUCCESS;
}

int receive_directory(char *mirrod_id_dir, char *file_name) {
	int ret_val;
	char received_dir_path[STRING_SIZE*2];
	if (directory_exists(mirrod_id_dir) == FALSE) {
		ret_val = create_directory(mirrod_id_dir);
		if (ret_val != SUCCESS) {return E_CREATE_DIR;}
	}
	sprintf(received_dir_path, "%s/%s", mirrod_id_dir, file_name);
	ret_val = create_directory(received_dir_path);
	if (ret_val != SUCCESS) {return E_CREATE_DIR;}
	return SUCCESS;
}

int read_from_pipe(int fifo_fd, void *buffer,int length) {
	int bytes;
	bytes = read(fifo_fd, buffer, length);
	if (bytes == -1){
		pipe_read_error();
		return E_PIPE_READ;
	}
	return bytes;
}

int try_read_from_pipe(int fifo_fd, void *buffer,int length) {
	struct pollfd poll_fd[1];
	int ret_val, bytes, i;

	poll_fd[0].fd = fifo_fd;
	poll_fd[0].events = POLLIN;
	while(1) {
		ret_val = poll(poll_fd ,1 ,PIPE_READ_TIMEOUT);
		if (ret_val == -1) {
			poll_error();
			return E_POLL;
		}
		else if (ret_val == 0) {
			return E_TIMEOUT;
		}
		else if ((ret_val == 1) && (poll_fd[0].revents & POLLIN)){
			if (poll_fd[0].fd == fifo_fd) {
				bytes = read_from_pipe(fifo_fd, buffer, length);
				return bytes;
			}
		}
		else {
			poll_fd[0].revents = 0;
		}
	}
}

int write_to_file(int fd, void *buffer, int length) {
	int bytes;
	bytes = write(fd, buffer, length);
	if (bytes == -1){
		file_write_error();
		return E_FILE_WRITE;
	}
	return bytes;
}

int try_append_to_log_file(char *log_file, char *buffer) {
	int bytes, fd;
	fd = open(log_file, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
	if (fd == -1 ) {
		file_open_error(log_file);
		return E_FILE_OPEN;
	}
	//safely, append to to the log file
	flock(fd, LOCK_EX);
	bytes = write(fd, buffer, strlen(buffer));
	fsync(fd);
	flock(fd, LOCK_UN);
	if (bytes == -1){
		file_write_error();
		return E_FILE_WRITE;
	}
	return SUCCESS;
}

void timeout_error(int sender, int receiver) {
	printf("%d to %d : TIMEOUT after %d seconds.\n", sender, receiver, (int)(PIPE_READ_TIMEOUT/1000.0));
}