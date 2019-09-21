#include "structs.h"
#include <signal.h>

#define SLEEP_PERIOD 5
#define SENDER_CHILD "sender"
#define RECEIVER_CHILD "receiver"
#define MAX_INT_OF_2_BYTE 65535
#define BYTES_OF_ENTRY_NAME_LENGTH 2
#define BYTES_OF_FILE_SIZE 4
#define BYTES_OF_TERMINATION_MSG 2
#define TERMINATION_MESSAGE 0
#define FILE_BIT 1
#define DIRECTORY_BIT 0
#define PIPE_READ_TIMEOUT 30000
#define MAX_FAIL_TIMES 3

	//other functions
void read_command_line_arguments(int argc, char **argv, Arguments *args);
int write_my_pid_to_common_dir(pid_t id, char *common_dir);
void append_to_log_file(char *log_file, char *buffer);
int handle_new_clients(Arguments *args, Id_List *id_list);
pid_t read_new_client_pid(char *file_name);
int handle_client(Arguments *args, Id_List *id_list, int new_id);
int handle_gone_clients(Arguments *arguments, Id_List *id_list);
int handle_failed_receiver_child(Arguments *arguments, Id_List *id_list, pid_t child_pid);
int handle_failed_sender_child(Arguments *arguments, Id_List *id_list, pid_t child_pid);
int client_exists(Arguments *arguments, int client_id);
void remove_entry(char *directory);
FILE* create_file(char *file_name,char *mode);
int create_directory(char *directory_name);
void remove_file(char *file_name);
void merge_with_slash(char *left_path, char *right_path, char *result);
int cut_first_part_of_the_path(char *path, char *new_path);
int remove_slash(char *string);
void sig_int_handler(int signum);
void set_sig_int_signal(struct sigaction *sig_action, void (*sigint_handler)(int));
void sig_chld_handler(int signum, siginfo_t *info, void *ucontext);
void set_sig_chld_signal(struct sigaction *sa, void (*sig_chld_handler)(int, siginfo_t*, void*));
void sig_usr1_handler(int signum, siginfo_t *info, void *ucontext);
void set_sig_usr1_signal(struct sigaction *sa, void (*sig_usr1_handler)(int, siginfo_t*, void*));
void sig_usr2_handler(int signum, siginfo_t *info, void *ucontext);
void set_sig_usr2_signal(struct sigaction *sa, void (*sig_usr2_handler)(int, siginfo_t*, void*));
void create_success_msg(Id_List *id_list, Arguments *arguments, pid_t child_pid, char *msg);