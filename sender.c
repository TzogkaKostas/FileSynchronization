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
#include "sender.h"

int main(int argc, char  **argv) {
	char fifo_name[STRING_SIZE*10], file_name[STRING_SIZE], common_dir[STRING_SIZE];
	char mirror_dir[STRING_SIZE],input_dir[STRING_SIZE], log_file[STRING_SIZE];
	int fifo_fd, i, receiver_id, my_id, pipe_size, fifo_was_created, parent_pid;

	//int x = 40;
	//reading of arguments
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i],"-n") == 0){
			my_id = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i],"-o") == 0) {
			receiver_id = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-c") == 0) {
			strcpy(common_dir, argv[i + 1]);
		}
		else if (strcmp(argv[i], "-i") == 0) {
			strcpy(input_dir, argv[i + 1]);
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
		else if (strcmp(argv[i], "-t") == 0) {
			//x = atoi(argv[i + 1]);
		}	
	}
	printf("Sender(pid:%d): %d to %d.\n",getpid(), my_id, receiver_id);
	sprintf(fifo_name,"%s/%d_to_%d.fifo",common_dir, my_id, receiver_id);
	fifo_was_created = mkfifo(fifo_name, 0666);
	if ((fifo_was_created == -1) && ( errno != EEXIST )) {
		mkfifo_error(fifo_name);
		exit(E_MKFIFO);
	}
	fifo_fd = open(fifo_name, O_WRONLY);		
	if (fifo_fd == -1 ) {
		open_fd_error(fifo_name);
		exit(E_OPEN_FD);
	}

	//sleep(x);

	send_input_dir_and_its_content(fifo_fd, log_file, input_dir, pipe_size);
	send_termination_message(fifo_fd);

	close(fifo_fd);
	exit(0);
}