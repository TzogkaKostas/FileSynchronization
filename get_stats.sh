#!/bin/bash

num_clients=0
min_id=9999999999999
max_id=-1
num_bytes_sent=0
num_bytes_read=0
num_files_sent=0
num_files_received=0
num_clients_left=0

echo "Clients id:"
#read the log_files line by line
while read line
do
	array=($(echo "$line"))
	if [ "${array[0]}" = "client_in" ];
	then
		echo -n "${array[1]} "
		if [ ${array[1]} -ge $max_id ];
		then
			max_id=${array[1]}
		fi
		if [ ${array[1]} -le $min_id ];
		then
			min_id=${array[1]}
		fi
		num_clients=$(($num_clients+1))
	elif [ "${array[0]}" = "sent:" ];
	then
		num_bytes_sent=$(($num_bytes_sent+${array[2]}))
		num_files_sent=$(($num_files_sent+1))
	elif [ "${array[0]}" = "received:" ];
	then
		num_bytes_read=$(($num_bytes_read+${array[2]}))
		num_files_received=$(($num_files_received+1))
	elif [ "${array[0]}" = "client_out" ];
	then
		num_clients_left=$(($num_clients_left+1))
	fi
done

echo ""
echo "Number of clients that have beed connected: ${num_clients}."
echo "Mininum id: ${min_id}."
echo "Maximum id: ${max_id}."
echo "Number of sent bytes: ${num_bytes_sent}."
echo "Number of read bytes: ${num_bytes_read}."
echo "Number of received files: ${num_files_received}."
echo "Number of sent files: ${num_files_sent}."
echo "Number of clients that have beed disconnected: ${num_clients_left}."