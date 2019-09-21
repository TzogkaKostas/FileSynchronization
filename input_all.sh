#!/bin/bash

if [ "$#" -ne 5 ]; then
	echo "usage: ./input_all <first id> <last id> <num_files> <num_of_dirs> <levels>"
	exit 1
fi

first=$1
last=$2
num_of_files=$3
num_of_dirs=$4
levels=$5
for ((i=$first; i<=$last; i++))
do
	./create_infiles.sh "${i}_input" $num_of_files $num_of_dirs $levels
done