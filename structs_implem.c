#include <stdio.h>
#include <stdlib.h>

#include "structs.h"	
#include "error_handle.h"

	//Arguments struct accessors

int get_arg_id(Arguments *args) {
	return args->id;
}

int get_buffer_size(Arguments *args) {
	return args->buffer_size;
}

char* get_common_dir(Arguments *args) {
	return args->common_dir;
}

char* get_input_dir(Arguments *args) {
	return args->input_dir;
}

char* get_mirror_dir(Arguments *args) {
	return args->mirror_dir;
}

char* get_log_file(Arguments *args) {
	return args->log_file;
}

void print_arguments(Arguments *args) {
	printf("id (-n ) = %d\n", args->id);
	printf("common_dir (-c) = %s\n", args->common_dir);
	printf("input_dir (-i) = %s\n", args->input_dir);
	printf("mirror_dir (-m) = %s\n", args->mirror_dir);
	printf("buffer_size (-b) = %d\n", args->buffer_size);
	printf("log_file (-l) = %s\n", args->log_file);
}

	//Id_List functions
void initialize_list(Id_List **id_list) {
	*id_list = (Id_List*)malloc(sizeof(Id_List));
	set_head(*id_list,NULL);
}

Id_Node* get_head(Id_List *id_list) {
	return id_list->head;
}

void set_head(Id_List *id_list,Id_Node *id_node) {
	id_list->head = id_node;
}

Id_Node* insert(Id_List *id_list,int id, pid_t pid) {
	Id_Node *temp;
	temp = (Id_Node*)malloc(sizeof(Id_Node));
	set_next(temp,get_head(id_list));
	set_id(temp, id);
	temp->pid = pid;
	temp->failed_sender_counter = 0;
	temp->failed_receiver_counter = 0;
	set_head(id_list,temp);
}

Id_Node* insert_info(Id_List *id_list, int id, pid_t sender_pid, pid_t receiver_pid) {
	Id_Node *temp;
	temp = search_node(id_list, id);
	set_sender_pid(temp, sender_pid);
	set_reicever_pid(temp, receiver_pid);
}

Id_Node* insert_sender_child_pid(Id_List *id_list, int id, pid_t sender_pid) {
	Id_Node *temp;
	temp = search_node(id_list, id);
	temp->sender_pid = sender_pid;
}

Id_Node* insert_receiver_child_pid(Id_List *id_list, int id, pid_t receiver_pid) {
	Id_Node *temp;
	temp = search_node(id_list, id);
	temp->receiver_pid = receiver_pid;
}

int delete(Id_List *id_list,int id) {
	Id_Node *cur,*prev;
	cur = get_head(id_list);
	if (cur == NULL) return FAILURE;

	if (get_id(cur) == id) {
		set_head(id_list,get_next(cur));
		free(cur);
		return SUCCESS;
	}

	while (cur != NULL) {
		if (get_id(cur) == id) {
			prev->next = cur->next;
			set_next(prev,get_next(cur));
			free(cur);
			return SUCCESS;
		}
		else {
			prev = cur;
			cur = get_next(cur);
		}
	}
	return FAILURE;
}

int exists(Id_List *id_list,int id) {
	if (search_node(id_list,id) != NULL){
		return TRUE;
	}
	else {
		return FALSE;
	}
}

int increase_fail_counter(Id_List *id_list, int id, pid_t pid) {
	Id_Node *temp;
	temp = search_node(id_list, id);
	if (pid == get_sender_pid2(temp)) {
		increase_sender_fail_counter(temp);
	}
	else if (pid == get_receiver_pid(temp)) {
		increase_receiver_fail_counter(temp);
	}
}

Id_Node* search_node(Id_List *id_list,int id) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		if (get_id(temp) == id) {
			return temp;
		}
		temp = get_next(temp);
	}
	return NULL;
}

Id_Node* search_child(Id_List *id_list, pid_t pid) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		if (get_sender_pid2(temp) == pid || get_receiver_pid(temp) == pid) {
			return temp;
		}
		temp = get_next(temp);
	}
	return NULL;	
}

int get_failed_sender_counter(Id_List *id_list, int id) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		if (get_id(temp) == id) {
			return get_failed_sender_counter2(temp);
		}
		temp = get_next(temp);
	}
	return -1;
}

int get_other_id(Id_List *id_list, pid_t pid) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		if (get_sender_pid2(temp) == pid || get_receiver_pid(temp) == pid) {
			return get_id(temp);
		}
		temp = get_next(temp);
	}
	return -1;	
}

int get_id_by_pid(Id_List *id_list, pid_t pid) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		if (get_pid(temp) == pid) {
			return get_id(temp);
		}
		temp = get_next(temp);
	}
	return -1;	
}

pid_t get_sender_pid(Id_List *id_list, int id) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		if (get_id(temp) == id) {
			return get_sender_pid2(temp);
		}
		temp = get_next(temp);
	}
	return -1;	
}

void get_all_info(Id_List *id_list, pid_t child_pid, int *other_id, pid_t *other_pid, pid_t *sender_pid,
		pid_t *receiver_pid, int *failed_sender_counter, int *failed_receiver_counter) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		if (get_sender_pid2(temp) == child_pid || get_receiver_pid(temp) == child_pid) {
			*other_id = get_id(temp);
			*other_pid = get_pid(temp);
			*sender_pid = get_sender_pid2(temp);
			*receiver_pid = get_receiver_pid(temp);
			*failed_sender_counter = get_failed_sender_counter2(temp);
			*failed_receiver_counter = get_failed_receiver_counter2(temp);
			return;
		}
		temp = get_next(temp);
	}
}

void print(Id_List *id_list) {
	Id_Node *temp = get_head(id_list);
	while(temp != NULL) {
		print_info(temp);
		temp = get_next(temp);	
	}
	printf("\n");
}

void destroy_all_nodes(Id_List *id_list) {
	Id_Node *temp;
	while(get_head(id_list) != NULL) {
		temp = get_head(id_list);
		set_head(id_list, get_next(get_head(id_list)));
		free(temp);
	}
}

void destroy(Id_List **id_list) {
	destroy_all_nodes(*id_list);
	free(*id_list);
}

	//Id_Node functions
int get_id(Id_Node *id_node) {
	return id_node->id;
}

Id_Node* get_next(Id_Node *id_node) {
	return id_node->next;
}

pid_t get_pid(Id_Node *id_node) {
	return id_node->pid;
}

pid_t get_sender_pid2(Id_Node *id_node) {
	return id_node->sender_pid;
}

pid_t get_receiver_pid(Id_Node *id_node) {
	return id_node->receiver_pid;
}

int get_failed_sender_counter2(Id_Node *id_node) {
	return id_node->failed_sender_counter;
}

int get_failed_receiver_counter2(Id_Node *id_node) {
	return id_node->failed_receiver_counter;
}

void set_next(Id_Node *id_node, Id_Node *next) {
	id_node->next = next;
}

void set_id(Id_Node *id_node, int id) {
	id_node->id = id;
}

void set_sender_pid(Id_Node *id_node, pid_t sender_pid) {
	id_node->sender_pid = sender_pid;
}

void set_reicever_pid(Id_Node *id_node, pid_t receiver_pid) {
	id_node->receiver_pid = receiver_pid;
}

void increase_sender_fail_counter(Id_Node *id_node) {
	id_node->failed_sender_counter += 1;
}

void increase_receiver_fail_counter(Id_Node *id_node) {
	id_node->failed_receiver_counter += 1;
}

void print_info(Id_Node *id_node) {
	printf("(id: %d ",id_node->id);
	printf("pid: %d ",id_node->pid);
	printf("sender_pid: %d ",id_node->sender_pid);
	printf("receiver_pid: %d ",id_node->receiver_pid);
	printf("sender fail: %d ",id_node->failed_sender_counter);
	printf("receiver fail: %d )",id_node->failed_receiver_counter);

}