#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/file.h>
#include <errno.h>

#include "main_header.h"
#include "error_handle.h"

void read_command_line_arguments(int argc, char **argv, Arguments *args) {
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i],"-n") == 0) {
			args->id = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i],"-c") == 0) {
			strcpy(args->common_dir,argv[i+1]);
			remove_slash(args->common_dir);
		}
		else if (strcmp(argv[i],"-i") == 0) {
			strcpy(args->input_dir,argv[i+1]);
			remove_slash(args->input_dir);
		}
		else if (strcmp(argv[i],"-m") == 0) {
			strcpy(args->mirror_dir,argv[i+1]);
			remove_slash(args->mirror_dir);
		}
		else if (strcmp(argv[i],"-b") == 0) {
			args->buffer_size = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i],"-l") == 0) {
			strcpy(args->log_file,argv[i+1]);
		}
	}
}

int write_my_pid_to_common_dir(int id, char *common_dir) {
	int fd;
	pid_t pid;
	char path[STRING_SIZE], id_as_string[STRING_SIZE];

	sprintf(path, "%s/%d.id", common_dir, id);
	if (file_exists(path) == TRUE) {
		file_exists_error(path);
		exit(E_FILE_EXISTS);
	}

	fd = open(path, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);		
	if (fd == -1 ) {
		open_fd_error(path);
		exit(E_OPEN_FD);
	}
	pid = getpid();
	int bytes = write(fd, &pid, sizeof(pid_t));
	close(fd);
	return 0;
}

void append_to_log_file(char *log_file, char *buffer) {
	int bytes, fd;
	fd = open(log_file, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1 ) {
		file_open_error(log_file);
		exit(E_FILE_OPEN);
	}
	//safely, append to to the log file
	flock(fd, LOCK_EX);
	bytes = write(fd, buffer, strlen(buffer));
	fsync(fd);
	flock(fd, LOCK_UN);
	if (bytes == -1){
		file_write_error();
		exit(E_FILE_WRITE); 
	}
	return;
}

int handle_new_clients(Arguments *arguments, Id_List *id_list) {
	DIR *dir;
	struct dirent *entry;
	char new_file[STRING_SIZE];
	int new_id;
	pid_t new_pid;

	dir = opendir(get_common_dir(arguments));
	if (dir == NULL) {
		opendir_error(get_common_dir(arguments));
		exit(E_OPEN_DIR);
	}
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_type == DT_DIR 
				|| strstr(entry->d_name, ".id") == NULL ){
			continue;
		}
		new_id = atoi(entry->d_name);
		//check if file is new
		if (exists(id_list, new_id) == FALSE) {
			sprintf(new_file,"%s/%s",get_common_dir(arguments),entry->d_name);
			new_pid = read_new_client_pid(new_file);
			insert(id_list, new_id, new_pid);
			handle_client(arguments, id_list, new_id);
		}
	}
	closedir(dir);
	return 0;
}

pid_t read_new_client_pid(char *file_name) {
	int bytes, fd;
	pid_t pid;
	fd = open(file_name, O_RDONLY);		
	if (fd == -1 ) {
		open_fd_error(file_name);
		exit(E_OPEN_FD);
	}
	bytes = read(fd, &pid, sizeof(pid_t));
	if (bytes == -1){
		file_read_error();
		exit(E_FILE_READ);
	}
	close(fd);
	return pid;
}

int handle_client(Arguments *arguments, Id_List *id_list, int new_id) {
	pid_t sender_child,receiver_child,sender_status,receiver_status;
	char new_id_str[STRING_SIZE], my_id_str[STRING_SIZE], buffer_size_str[STRING_SIZE];
	char pid_str[STRING_SIZE];

	sprintf(new_id_str, "%d", new_id);
	sprintf(my_id_str, "%d", get_arg_id(arguments));
	sprintf(buffer_size_str, "%d", get_buffer_size(arguments));
	sprintf(pid_str, "%d", getpid());
	sender_child = fork();
	if (sender_child == -1) {
		fork_error();
		exit(E_FORK);
	}
	if (sender_child == 0) {
		execl(SENDER_CHILD, SENDER_CHILD, "-o",new_id_str, "-n",my_id_str,
			"-c", get_common_dir(arguments), "-i", get_input_dir(arguments),
			"-b", buffer_size_str, "-l", get_log_file(arguments), "-p", 
				pid_str, NULL);
		execl_error(SENDER_CHILD);
		exit(E_EXECL);
	}
	else {
		receiver_child = fork();
		if (receiver_child == -1) {
			fork_error();
			exit(E_FORK);
		}
		if (receiver_child == 0) {
			execl(RECEIVER_CHILD, RECEIVER_CHILD, "-o",new_id_str, "-n",my_id_str,
				"-c", get_common_dir(arguments), "-m", get_mirror_dir(arguments),
				"-b", buffer_size_str, "-l", get_log_file(arguments), "-p", 
				pid_str, NULL);
			execl_error(RECEIVER_CHILD);
			exit(E_EXECL);
		}
	}
	//insert sender's child and receiver's child pid into the list
	insert_info(id_list, new_id, sender_child, receiver_child);
}

int handle_gone_clients(Arguments *arguments, Id_List *id_list) {
	Id_Node *temp;
	int id;
	char id_str[STRING_SIZE];

	temp = get_head(id_list);
	//check if some client has been disconnected
	while(temp != NULL) {
		id = get_id(temp);
		if (client_exists(arguments, id) == FALSE && get_arg_id(arguments) != id) {
			sprintf(id_str, "%s/%d", get_mirror_dir(arguments), id);
			printf("Client %d has been disconnected.\n", id);
			remove_entry(id_str);
			delete(id_list, id);
		}
		temp = get_next(temp);
	}
	return 0;
}

int handle_failed_receiver_child(Arguments *arguments, Id_List *id_list, pid_t child_pid) {
	pid_t other_pid, sender_child_pid, receiver_child_pid, pid1, pid;
	int sender_failed_counter, receiver_failed_counter, other_id;
	char other_id_str[STRING_SIZE], my_id_str[STRING_SIZE], buffer_size_str[STRING_SIZE];
	char pid_str[STRING_SIZE];

	//get the other clients's id, pid and my children's pid.
	get_all_info(id_list, child_pid, &other_id, &other_pid, &sender_child_pid, &receiver_child_pid,
		&sender_failed_counter, &receiver_failed_counter);

	sprintf(other_id_str, "%d", other_id);
	sprintf(my_id_str, "%d", get_arg_id(arguments));
	sprintf(buffer_size_str, "%d", get_buffer_size(arguments));
	sprintf(pid_str, "%d", getpid());
 	if (receiver_failed_counter + 1 <= MAX_FAIL_TIMES) {
		pid = fork();
		if (pid == -1) {
			fork_error();
			exit(E_FORK);
		}
		if (pid == 0) {
			execl(RECEIVER_CHILD, RECEIVER_CHILD, "-o",other_id_str, "-n",my_id_str,
				"-c", get_common_dir(arguments), "-m", get_mirror_dir(arguments),
				"-b", buffer_size_str, "-l", get_log_file(arguments), "-p", 
				pid_str, NULL);
			execl_error(RECEIVER_CHILD);
			exit(E_EXECL);
		}
		insert_receiver_child_pid(id_list, other_id, pid);
		increase_fail_counter(id_list, other_id, pid);
		//send signal to the other client, so he creates a sender child process
		kill(other_pid, SIGUSR2);
	}	
	else {
		printf("Receiver for %d has failed %d times.No more trying.\n", other_id, receiver_failed_counter + 1);
	}
}

int handle_failed_sender_child(Arguments *arguments, Id_List *id_list, pid_t other_pid) {
	pid_t pid, cur_sender_pid;
	int sender_failed_counter, other_id;
	char other_id_str[STRING_SIZE], my_id_str[STRING_SIZE], buffer_size_str[STRING_SIZE];
	char pid_str[STRING_SIZE];

	//get others client's id
	other_id = get_id_by_pid(id_list, other_pid);
	//get the number of failed trials with this client
	sender_failed_counter = get_failed_sender_counter(id_list, other_id);
	//get others client's pid
	cur_sender_pid = get_sender_pid(id_list, other_id);
	if (sender_failed_counter + 1 <= MAX_FAIL_TIMES) {
		pid = fork();
		if (pid == -1) {
			fork_error();
			exit(E_FORK);
		}
		if (pid == 0) {
			sprintf(other_id_str, "%d", other_id);
			sprintf(my_id_str, "%d", get_arg_id(arguments));
			sprintf(buffer_size_str, "%d", get_buffer_size(arguments));
			sprintf(pid_str, "%d", getpid());
			
			execl(SENDER_CHILD, SENDER_CHILD, "-o",other_id_str, "-n",my_id_str,
				"-c", get_common_dir(arguments), "-i", get_input_dir(arguments),
				"-b", buffer_size_str, "-l", get_log_file(arguments), "-p", 
				pid_str, "-t", "1", NULL);
			execl_error(SENDER_CHILD);
			exit(E_EXECL);
		}
		insert_sender_child_pid(id_list, other_id, pid);
		increase_fail_counter(id_list, other_id, pid);
		//kill my sender child, because i created a new one
		kill(cur_sender_pid, SIGINT);
	}
	else {
		printf("Sender for %d  has failed %d times.No more trying.\n", other_id, sender_failed_counter + 1);
	}
}

int client_exists(Arguments *arguments, int client_id) {
	DIR *dir;
	struct dirent *entry;
	char new_file[STRING_SIZE], client_id_str[STRING_SIZE];

	dir = opendir(get_common_dir(arguments));
	if (dir == NULL) {
		opendir_error(get_common_dir(arguments));
		exit(E_OPEN_DIR);
	}
	sprintf(client_id_str,"%d.id", client_id);
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_type == DT_DIR){
			continue;
		}
		if (strcmp(entry->d_name, client_id_str) == 0) {
			closedir(dir);
			return TRUE;
		}
	}
	closedir(dir);
	return FALSE;
}

void remove_entry(char *entry) {
	pid_t pid;
	pid = fork();
	if (pid == -1 ) {
		fork_error();
		exit(E_FORK);
	}
	if (pid == 0) {
		execlp("rm", "rm", "-rf", entry, NULL);
		execl_error("rm -rf error:");
		exit(E_EXECL);
	}
	else {
	}
}

FILE* create_file(char *file,char *mode) {
	FILE *fp;
	fp = fopen(file,mode); //filename
	if (fp == NULL) {
		create_file_error(file);
		exit(E_CREATE_FILE);
	}
	return fp;
}

int create_directory(char *directory) {
	if (mkdir(directory,0777) == -1) {
		return E_CREATE_DIR;
	}
	else {
		return SUCCESS;
	}
}

void remove_file(char *file_name) {
	int ret_val = remove(file_name);
	if (ret_val == -1) {
		remove_error(file_name);
		exit(E_REMOVE);
	}
}

void merge_with_slash(char *left_path, char *right_path, char *result) {
	char source[STRING_SIZE],destination[STRING_SIZE];
	//e.g. left_path = "d1/d2",right_path = f1, result = d1/d2/f1
	strcpy(result,left_path);
	strcat(result,"/");
	strcat(result,right_path);
}

int cut_first_part_of_the_path(char *path, char *new_path) {
	//e.g. 1_input/dir1/dir2/dir3/file1 ---> dir1/dir2/dir3/file1
	char *ptr = strchr(path,'/');
	strcpy(new_path, ptr + 1);
}

int remove_slash(char *string) {
	char *ptr;
	ptr = strstr(string, "/");
	if (ptr != NULL) {
		*ptr = '\0';
	}
}

void set_sig_int_signal(struct sigaction *sig_action, void (*sig_int_handler)(int)) {
	int ret_val;
	sig_action->sa_handler = sig_int_handler;
	sig_action->sa_flags = SA_RESTART;
	ret_val = sigfillset(&sig_action->sa_mask);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}
	ret_val = sigaction(SIGINT, sig_action, NULL);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}	
}

void set_sig_chld_signal(struct sigaction *sa, void (*sig_chld_handler)(int, siginfo_t*, void*)) {
	int ret_val;
	sa->sa_sigaction = sig_chld_handler;
	sa->sa_flags = SA_RESTART | SA_SIGINFO | SA_NOCLDSTOP;
	ret_val = sigfillset(&sa->sa_mask);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}
	ret_val = sigaction(SIGCHLD, sa, NULL);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}	
}

void set_sig_usr1_signal(struct sigaction *sa, void (*sig_usr1_handler)(int, siginfo_t*, void*)) {
	int ret_val;
	sa->sa_sigaction = sig_usr1_handler;
	sa->sa_flags = SA_RESTART | SA_SIGINFO;
	ret_val = sigfillset(&sa->sa_mask);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}
	ret_val = sigaction(SIGUSR1, sa, NULL);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}	
}

void set_sig_usr2_signal(struct sigaction *sa, void (*sig_usr2_handler)(int, siginfo_t*, void*)) {
	int ret_val;
	sa->sa_sigaction = sig_usr2_handler;
	sa->sa_flags = SA_RESTART | SA_SIGINFO;
	ret_val = sigfillset(&sa->sa_mask);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}
	ret_val = sigaction(SIGUSR2, sa, NULL);
	if (ret_val == -1) {
		signal_error();
		exit(E_SIGNAL);
	}	
}

void create_success_msg(Id_List *id_list, Arguments *arguments, pid_t child_pid, char *msg) {
	print(id_list);


	int other_id = get_other_id(id_list, child_pid);
	pid_t sender_child_pid = get_sender_pid(id_list, other_id);
	printf("other id:%d sender_pid: %d child_pid:%d\n", other_id, sender_child_pid, child_pid);
	if (sender_child_pid == child_pid) {
		sprintf(msg, "Sender: %d_to_%d completed\n", get_arg_id(arguments), other_id);
	}
	else {
		sprintf(msg, "Receiver: %d_to_%d completed\n", other_id, get_arg_id(arguments));
	}

	//if (child_pid == receiver_child_pid) {
	//	sprintf(msg, "Receiver: %d_to_%d completed\n", other_id, get_arg_id(arguments));
	//}
	//else if (child_pid == sender_child_pid) {
	//	sprintf(msg, "Sender: %d_to_%d completed\n", get_arg_id(arguments), other_id);
	//}
	//else {
	//	printf("--- %d %d %d ---\n", child_pid, sender_child_pid, receiver_child_pid );
	//}
}