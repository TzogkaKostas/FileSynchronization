#!/bin/bash

dir_name=$1
num_of_files=$2
num_of_dirs=$3
levels=$4

#check if all arguments were given
if [ "$#" -ne 4 ]; then
	echo "usage: ./create_infiles.sh dir_name num_of_files num_of_dirs levels."
	exit 1
fi
#check if arguments are positive
if [ $num_of_dirs -lt 0 ] || [ $num_of_files -lt 0 ] || [ $levels -lt 0 ]; then
	echo "num_of_files, num_of_files and levels must not be negatives."
	exit 2
fi
#create the dir_name, if it doen't exist
if [ ! -d "$dir_name" ]; then
	mkdir $dir_name
fi

#create directories with random names
max_dir_name_length=8
while [ $num_of_dirs -gt 0 ] && [ $levels -gt 0 ]
do
	in_dir_name=$dir_name
	for ((i=1; i<=$levels; i++))
	do
		#generate a random number
		rand_dir_name_length=$((1 + RANDOM % $max_dir_name_length))
		#generate a random string
		rand_dir_name=$(cat /dev/urandom | tr -cd 'a-z0-9A-Z' | head -c $rand_dir_name_length)
		#if it doesnt already exists, make a directory with this name
		if [ ! -d "${in_dir_name}/$rand_dir_name" ]; then
			in_dir_name="${in_dir_name}/$rand_dir_name"
			mkdir "$in_dir_name"
			num_of_dirs=$(($num_of_dirs-1))
		fi
		if [ ! $num_of_dirs -gt 0 ]; then
			break
		fi
	done
done

#a list with all directories under dir_name(including itself)
dir_list=$(find $dir_name -type d ! -name '.')

#create files with random names and data
max_file_name_length=8
max_kilo_bytes=128
while [ $num_of_files -gt 0 ]
do
	for directory in $dir_list
	do
		rand_file_name_length=$((1 + RANDOM % $max_file_name_length))
		rand_file_name=$(cat /dev/urandom | tr -cd 'a-z0-9A-Z' | head -c $max_file_name_length)	
		if [ ! -f "${directory}/$rand_file_name.txt" ]; then
			file_name="${directory}/$rand_file_name.txt"
			touch "$file_name"
			kilo_bytes=$(((1 + RANDOM % $max_kilo_bytes)*1000))
			head -c $kilo_bytes </dev/urandom >$file_name
			num_of_files=$(($num_of_files-1))
		fi
		if [ ! $num_of_files -gt 0 ]; then
			break
		fi
	done
done