#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "usage: ./clean_all <number of clients>"
	exit 1
fi


num_of_cliens=$1
for ((i=1; i<=$num_of_cliens; i++))
do
	rm -rf "${i}_mirror"
	rm -rf "log_file${i}"
	rm -rf "${i}_input"
done

rm common/*
