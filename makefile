all: mirror_client sender receiver

mirror_client: mirror_client.o functions.o structs_implem.o error_handle.o main_header.h structs.h error_handle.h
	gcc mirror_client.o functions.o error_handle.o structs_implem.o -o mirror_client
mirror_client.o : mirror_client.c main_header.h error_handle.h
	gcc -c mirror_client.c -o mirror_client.o
sender:sender.o sender_implem.o functions.o error_handle.o structs_implem.o main_header.h
	gcc sender.o sender_implem.o functions.o error_handle.o structs_implem.o -o sender
receiver:receiver.o receiver_implem.o functions.o error_handle.o structs_implem.o main_header.h
	gcc receiver.o receiver_implem.o functions.o error_handle.o structs_implem.o -o receiver
sender.o:sender.c sender_implem.c sender.h main_header.h error_handle.h
	gcc -c sender.c -o sender.o
receiver.o:receiver.c receiver.h main_header.h error_handle.h
	gcc -c receiver.c -o receiver.o
sender_implem.o: sender_implem.c sender.h main_header.h error_handle.h
	gcc -c sender_implem.c -o sender_implem.o
receiver_implem.o: receiver_implem.c receiver.h main_header.h error_handle.h
	gcc -c receiver_implem.c -o receiver_implem.o
functions.o:functions.c main_header.h error_handle.h
	gcc -c  functions.c -o functions.o
structs_implem.o: structs_implem.c structs.h error_handle.h
	gcc -c structs_implem.c -o structs_implem.o
error_handle.o: error_handle.c main_header.h error_handle.h
	gcc -c error_handle.c -o error_handle.o
clean :
	rm *.o mirror_client sender receiver