#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>

#include "main_header.h"
#include "error_handle.h"

Arguments arguments;
Id_List *id_list;
sigset_t mask;

volatile sig_atomic_t keep_running = 1;

void sig_int_handler(int signum) {
	keep_running = 0;
}

void sig_chld_handler(int signum, siginfo_t *info, void *ucontext) {
	sigprocmask(SIG_BLOCK, &mask, NULL); //block signals	
	pid_t pid;
	int status;
	char msg[STRING_SIZE];
	while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		sprintf(msg,"Child with pid %d terminated with exit code %d\n", pid, WEXITSTATUS(status));
		write(1, msg, strlen(msg));	
	}
	sigprocmask(SIG_UNBLOCK, &mask, NULL); //unblock signals
}

void sig_usr1_handler(int signum, siginfo_t *info, void *ucontext) {
	sigprocmask(SIG_BLOCK, &mask, NULL); //block signals	
	char msg[STRING_SIZE];
	sprintf(msg,"Error occured with receiver child with pid %d "
		"in receiving with client %d \n", info->si_pid, get_other_id(id_list, info->si_pid));
	write(1, msg, strlen(msg));
	handle_failed_receiver_child(&arguments, id_list, info->si_pid);
	sigprocmask(SIG_UNBLOCK, &mask, NULL); //unblock signals
}

void sig_usr2_handler(int signum, siginfo_t *info, void *ucontext) {
	sigprocmask(SIG_BLOCK, &mask, NULL); //block signals
	char msg[STRING_SIZE];
	sprintf(msg, "Client %d with pid %d needs me to resend files.\n", get_id_by_pid(id_list, info->si_pid), info->si_pid);
	write(1, msg, strlen(msg));
	handle_failed_sender_child(&arguments, id_list, info->si_pid);
	sigprocmask(SIG_UNBLOCK, &mask, NULL); //unblock signals
}

int main(int argc, char **argv) {
	char my_id_file[STRING_SIZE], log_file_buffer[STRING_SIZE];
	struct sigaction sig_int_action, sig_chld_action, sig_usr1_action, sig_usr2_action;

	if (argc - 1 != NUM_OF_ARGUMENTS) {
		num_arguments_error(argc);
		exit(E_NUM_ARGS);
	}
	//initialize all sigaction structures
	memset(&sig_int_action, 0, sizeof(struct sigaction));
	memset(&sig_chld_action, 0, sizeof(struct sigaction));
	memset(&sig_usr1_action, 0, sizeof(struct sigaction));
	memset(&sig_usr2_action, 0, sizeof(struct sigaction));
	set_sig_int_signal(&sig_int_action, sig_int_handler);
	set_sig_chld_signal(&sig_chld_action, sig_chld_handler);
	set_sig_usr1_signal(&sig_usr1_action, sig_usr1_handler);
	set_sig_usr2_signal(&sig_usr2_action, sig_usr2_handler);

	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigaddset(&mask, SIGUSR1);
	sigaddset(&mask, SIGUSR2);
	read_command_line_arguments(argc, argv, &arguments);

	arguments_checking(&arguments);

	initialize_list(&id_list);
	insert(id_list, get_arg_id(&arguments), (int)getpid());

	write_my_pid_to_common_dir(get_arg_id(&arguments), get_common_dir(&arguments));
	sprintf(log_file_buffer, "client_in %d\n", get_arg_id(&arguments));
	append_to_log_file(get_log_file(&arguments), log_file_buffer);

	printf("client pid:%d - id: %d\n", getpid(), get_arg_id(&arguments));

	while(keep_running == 1) {
		printf("loop\n");
		handle_new_clients(&arguments, id_list);
		handle_gone_clients(&arguments, id_list);
		sleep(SLEEP_PERIOD);
	}
	sprintf(my_id_file, "%s/%d.id", get_common_dir(&arguments), get_arg_id(&arguments));
	remove_file(my_id_file);
	remove_entry(get_mirror_dir(&arguments));
	sprintf(log_file_buffer, "client_out %d\n", get_arg_id(&arguments));
	append_to_log_file(get_log_file(&arguments), log_file_buffer);
	while (waitpid(-1,NULL,0) != -1); //wait for all children to terminate.
	destroy(&id_list);
	printf("Client Termination\n");
	exit(0);
}