#!/bin/bash


VSERVER_FILES_DIR=./files


#vergleiche alle files gegen die files im system

cd $VSERVER_FILES_DIR

echo '[32m----- files not in main system (but in vserver_dir) ----[0m'
for i in $(find .)
do
	file="/"$i
 	test -e $file || echo "[31mmissing: $file[0m"
done 

echo '[32m----- diff: server <-> vserver_dir -----[0m'
for i in $(find .)
do
	#dont diff directories, only files that are present on
	#server and vserver_dir

	if [ ! -d $i -a -e $i -a -e /$i ]; then
 		diff -q /$i $i >/dev/null || {
			echo "[31mdifferent: $i -> starting meld[0m"
			meld /$i $i
		}
	fi
done 
