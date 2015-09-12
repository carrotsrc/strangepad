#!/bin/sh
if [ "$1" == "gdb" ]; then
	args=($@)
	len=${#args[@]}

	params=${args[@]:1:$len-1}
	LD_LIBRARY_PATH=$STRANGEFW:./ $1 --args ./pad $params
else
	LD_LIBRARY_PATH=$STRANGEFW:./ ./pad $@
fi
