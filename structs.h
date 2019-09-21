#define STRING_SIZE 1024

typedef struct {
	int id,buffer_size;
	char common_dir[STRING_SIZE], input_dir[STRING_SIZE], mirror_dir[STRING_SIZE], log_file[STRING_SIZE];		
}Arguments;


typedef struct Id_Struct {
	int id;
	pid_t pid;
	pid_t sender_pid;
	pid_t receiver_pid;
	int failed_sender_counter;
	int failed_receiver_counter;
	struct Id_Struct *next;
}Id_Node;

typedef struct {
	Id_Node *head;
}Id_List;


	//struct Arguments accessors
int get_arg_id(Arguments *args);
int get_buffer_size(Arguments *args);
char* get_common_dir(Arguments *args);
char* get_input_dir(Arguments *args);
char* get_mirror_dir(Arguments *args);
char* get_log_file(Arguments *args);
void print_arguments(Arguments *args);

	//Id_List functions
void initialize_list(Id_List **id_list);
Id_Node* get_head(Id_List *id_list);
void set_head(Id_List *id_list,Id_Node *string_node);
Id_Node* insert(Id_List *id_list, int id, pid_t pid);
Id_Node* insert_info(Id_List *id_list, int id, pid_t sender_pid, pid_t receiver_pid);
Id_Node* insert_sender_child_pid(Id_List *id_list, int id, pid_t sender_pid);
Id_Node* insert_receiver_child_pid(Id_List *id_list, int id, pid_t receiver_pid);
int delete(Id_List *id_list,int id);
int exists(Id_List *id_list,int id);
int increase_fail_counter(Id_List *id_list, int id, pid_t pid);
Id_Node* search_node(Id_List *id_list,int id);
Id_Node* search_child(Id_List *id_list, pid_t pid);
int get_failed_sender_counter(Id_List *id_list, int id);
int get_other_id(Id_List *id_list, pid_t pid);
int get_id_by_pid(Id_List *id_list, pid_t pid);
pid_t get_sender_pid(Id_List *id_list, int pid);
void get_all_info(Id_List *id_list, pid_t child_pid, int *other_id, pid_t *other_pid, pid_t *sender_pid,
	pid_t *receiver_pid, int *failed_sender_counter, int *failed_receiver_counter);
void print(Id_List *id_list);
void destroy_all_nodes(Id_List *id_list);
void destroy(Id_List **id_list);


	//Id_Node functions
int get_id(Id_Node *id_node);
Id_Node* get_next(Id_Node *id_node);
pid_t get_pid(Id_Node *id_node);
pid_t get_sender_pid2(Id_Node *id_node);
pid_t get_receiver_pid(Id_Node *id_node);
int get_failed_sender_counter2(Id_Node *id_node);
int get_failed_receiver_counter2(Id_Node *id_node);
void set_next(Id_Node *id_node, Id_Node *next);
void set_id(Id_Node *id_node, int id);
void set_sender_pid(Id_Node *id_node, pid_t sender_pid);
void set_reicever_pid(Id_Node *id_node, pid_t receiver_pid);
void increase_sender_fail_counter(Id_Node *id_node);
void increase_receiver_fail_counter(Id_Node *id_node);
void print_info(Id_Node *id_node);