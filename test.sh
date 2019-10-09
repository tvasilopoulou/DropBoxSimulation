#!/bin/bash


var=`echo $1|cut -d $'\t' -f 1-4 text.txt|tr " " "\n"`

for i in $var
do
	echo $i
	((i++))
done

echo $var
