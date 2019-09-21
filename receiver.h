int try_receive_name_length(int fifo_fd, int *entry_name_length);
int try_receive_name(int fifo_fd, char *entry_name, int entry_name_length);
int try_receive_entry_size(int fifo_fd, int *entry_size);
int try_receive_indication_bit(int fifo_fd, int *indication_bit);
int try_receive_file(int fifo_fd, char *dir_for_copying, char *file_name, int file_size, int pipe_size);
int receive_directory(char *dir_for_copying, char *file_name);
int read_from_pipe(int fd, void *buffer,int length);
int try_read_from_pipe(int fifo_fd, void *buffer,int length);
int write_to_file(int fd, void *buffer,int length);
int try_append_to_log_file(char *log_file, char *buffer);
void timeout_error(int sender, int receiver);

int receive_input_dir_and_its_content(int fifo_fd, int pipe_size, char *mirrod_id_dir, char *log_file);
int receive_name_length(int fifo_fd, int *entry_name_length);
int receive_name(int fifo_fd, char *entry_name, int entry_name_length);
int receive_entry_size(int fifo_fd, int *entry_size);
int receive_indication_bit(int fifo_fd, int *indication_bit);
int receive_file(int fifo_fd, char *dir_for_copying, char *file_name, int file_size, int pipe_size);
