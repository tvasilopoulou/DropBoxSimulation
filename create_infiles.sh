#!/bin/bash   

if [[ $# -ne 4 ]]; then
	echo Error && exit 1;
fi

   
[[ -d $1 ]] && echo $1 already exists, aborting && exit  	#if request for an existing directory name, abort 
echo $1 
mkdir  $1  #create directory  
cd $1    #enter directory
ORIG_DIR=$(pwd) 
re='^[0-9]+$'

numOfFiles="$2"
numOfDirs="$3"
levels="$4"

if ! [[ $numOfFiles =~ $re ]] ; then echo "error: Value not a number." >&2; exit 1; fi
if ! [[ $numOfDirs =~ $re ]] ; then echo "error: Value not a number." >&2; exit 1; fi
if ! [[ $levels =~ $re ]] ; then echo "error: Value not a number." >&2; exit 1; fi


while [ $numOfDirs -gt 0 ]; do
	for (( i = 0; i < $levels; i++ )); do
		if [[ $numOfDirs -gt 0 ]]; then
			new=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 8 | head -n 1)		#https://gist.github.com/earthgecko/3089509
			mkdir $new
			cd $new
			((numOfDirs--))
		else
			break;
		fi
	done
	cd $ORIG_DIR
done
cd $ORIG_DIR


j=1
while [ $j -lt $numOfFiles ]; do
	for d in $(find $ORIG_DIR -type d); do
		if [[ $j -gt $numOfFiles ]]; then
			break
		fi
		touch $d/f$j
		b=$(( ( RANDOM % 131072 )  + 1023 ))
		echo $b
		echo $(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $b | head -n 1) >> $d/f$j
		((j++))
	done
done

