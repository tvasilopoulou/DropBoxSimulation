#!/bin/bash   

if [[ $# -ne 0 ]]; then
	echo Error && exit 1;
fi

flag=0
max=0
min=100
counter=0
while IFS= read -r line; do
	((counter++))
	if [[ $flag = 0 ]]; then		#initialise client for the first time ever
		client=${line:7:1}			#client number
		if [[ $max -lt ${line:7:1} ]]; then
			max=${line:7:1}
		fi
		if [[ $min -gt ${line:7:1} ]]; then
			min=${line:7:1}
		fi
		echo Client no.$client
		((flag++))
	fi
	if [[ $client -ne ${line:7:1} ]]; then
		if [[ $max -lt ${line:7:1} ]]; then
			max=${line:7:1}
		fi
		if [[ $min -gt ${line:7:1} ]]; then
			min=${line:7:1}
		fi
		client=${line:7:1}
		((flag++))					#flag is a counter
		echo Client no.$client
	fi
	bytes=$(( $bytes + ${line: -12:1} ))
done
echo Number of clients connected: $flag
echo Number of clients disconnected: $flag
echo Maximum client id: $max
echo Minimum client id: $min
echo Number of files per client: $((counter / flag))
echo Number of bytes sent: $((bytes / flag))
echo Number of bytes received: $((bytes / flag))