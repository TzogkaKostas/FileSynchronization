#define NUM_OF_ARGUMENTS 12
#define TRUE 0
#define FALSE 1
#define SUCCESS 0
#define FAILURE -1
#define E_NUM_ARGS -26
#define E_INPUT_DIR -2
#define E_MIRROR_DIR -3
#define E_FILE_OPEN -4
#define E_CREATE_DIR -5
#define E_CREATE_FILE -6
#define E_FILE_EXISTS -7
#define E_FORK -8
#define E_OPEN_DIR -9
#define E_MKFIFO -10
#define E_OPEN_FD -11
#define E_EXECL -12
#define E_PIPE_WRITE -13
#define E_PIPE_READ -14
#define E_NAME_LENGTH -15
#define E_FILE_READ -16
#define E_FILE_WRITE -17
#define E_FSTAT -18
#define E_STAT -19
#define E_FILE_CREAT -20
#define E_MALLOC -21
#define E_REMOVE -22
#define E_POLL -23
#define E_TIMEOUT -24
#define E_SIGNAL -25

	//output error functions
void num_arguments_error(int argc);
void input_dir_error(char *file_name);
void mirror_dir_error(char *file_name);
void file_exists_error(char *file_name);
void create_dir_error(char *directory_name);
void create_file_error(char *file_name);
void file_open_error(char *file_name);
void fork_error();
void opendir_error(char *directory);
void mkfifo_error(char *fifo_name);
void open_fd_error(char *file_name);
void execl_error(char *executable);
void pipe_write_error(char *string);
void pipe_read_error();
void name_length_error(char *entry);
void file_read_error();
void file_write_error();
void stat_error(char *name);
void fstat_error();
void file_creat_error();
void malloc_error();
void remove_error();
void poll_error();
void signal_error();

	//error checking functions
int arguments_checking(Arguments *arguments);
int directory_exists(char *directory_name);
int file_exists(char *filename);