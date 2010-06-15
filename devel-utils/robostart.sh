#!/bin/sh

while [ "$?" -ne 1 ]
do
	sleep 1
	ping -c 1 -W 1 10.1.1.2 1> /dev/null
done

./robomain
