#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "usage: ./run_all <first id> <last id>"
	exit 1
fi

start=$1
end=$2
for ((i=$start; i<=$end; i++))
do
	input_dir="${i}_input"
	mirror_dir="${i}_mirror"
	log_file="log_file${i}"
	#gnome-terminal -e "valgrind --leak-check=full ./mirror_client -n $i -c common -i $input_dir -m $mirror_dir -b 10 -l $log_file"
	gnome-terminal -e "./mirror_client -n $i -c common -i $input_dir -m $mirror_dir -b 10 -l $log_file"

	sleep 3
done