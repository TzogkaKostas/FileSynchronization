#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "usage: ./test_all <number of clients>"
	exit 1
fi

num_of_cliens=$1
for ((i=1; i<=$num_of_cliens; i++))
do
	for ((j=1; j<=$num_of_cliens; j++))
	do
		if [ $i -ne $j ];
		then
			diff -rq "${i}_input" "${j}_mirror/${i}"
		fi	
	done
done
