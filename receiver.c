#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include "main_header.h"
#include "error_handle.h"
#include "receiver.h"


int main(int argc, char  **argv) {
	char fifo_name[STRING_SIZE*10], file_name[STRING_SIZE], common_dir[STRING_SIZE];
	char mirror_dir[STRING_SIZE],mirrod_id_dir[STRING_SIZE*10], log_file[STRING_SIZE];
	int fifo_fd, i, sender_id, my_id, pipe_size, ret_val, fifo_was_created, parent_pid;

	//reading of arguments
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i],"-n") == 0){
			my_id = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i],"-o") == 0) {
			sender_id = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-c") == 0) {
			strcpy(common_dir, argv[i + 1]);
		}
		else if (strcmp(argv[i], "-m") == 0) {
			strcpy(mirror_dir, argv[i + 1]);
		}
		else if (strcmp(argv[i], "-b") == 0) {
			pipe_size = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-l") == 0) {
			strcpy(log_file, argv[i + 1]);
		}
		else if (strcmp(argv[i], "-p") == 0) {
			parent_pid = atoi(argv[i + 1]);
		}
	}
	printf("Receiver(pid:%d): %d to %d.\n",getpid(), sender_id, my_id);

	sprintf(fifo_name,"%s/%d_to_%d.fifo",common_dir, sender_id, my_id);
	fifo_was_created = mkfifo(fifo_name, 0666);
	if ((fifo_was_created == -1) && ( errno != EEXIST )) {
		mkfifo_error(fifo_name);
		kill(parent_pid, SIGUSR1);
		exit(E_MKFIFO);
	}
	fifo_fd = open(fifo_name, O_RDONLY);		
	if (fifo_fd == -1 ) {
		open_fd_error(fifo_name);
		kill(parent_pid, SIGUSR1);
		exit(E_OPEN_FD);
	}
	sprintf(mirrod_id_dir,"%s/%d", mirror_dir, sender_id);
	ret_val = receive_input_dir_and_its_content(fifo_fd, pipe_size, mirrod_id_dir, log_file);
	if (ret_val == E_TIMEOUT) {
		timeout_error(sender_id, my_id);
	}

	close(fifo_fd);
	remove_file(fifo_name);
	if (ret_val == SUCCESS) {
		exit(EXIT_SUCCESS);
	}
	else {
		kill(parent_pid, SIGUSR1);
		exit(EXIT_FAILURE);
	}
}