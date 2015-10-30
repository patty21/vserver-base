#!/bin/bash
#version 2
#install vserver stuff
#all files in vserver.d are enumerated. 
#each file may contain a package name, url, shell-command or package file name.
#
#Comments: each line starting with "#" is ignored
#
#url: 	each line that starts with http,https,ftp and ends with .dep is interpreted as
#	url. If the file is not already downloaded to the PKG_DIR, it is requested and
#	stored in PKG_DIR before installing it.
#e.g.:http://website.de/path/packet.dep
#
#package name:	a debian package name that should be installed
#e.g:bind9
#
#pre-cmd: a shell command that is executed AFTER installing packages
#pre-cmd: rm /etc/apache2/site-enabled/*
#
#post-cmd:	a shell command is exectuted AFTER installing packages and files. a temporary file is created with all the commands
#post-cmd:a2enmod ssl
#
#pre-inst-cmd: 	a shell command which is executed before installing packages
#pre-inst-cmd: apt-get remove bluez


PKG_DIR=dl
LIST_DIR=vserver.d
FILES=files
PRE_INST_PACKAGES=/tmp/vserver-preinstall-packages.sh
INST_PACKAGES=/tmp/vserver-install-packages.sh
PREINSTALL=/tmp/vserver-preinstall.sh
POSTINSTALL=/tmp/vserver-postinstall.sh

rm -rf $PRE_INST_PACKAGES $INST_PACKAGES $PREINSTALL $POSTINSTALL
> $PRE_INST_PACKAGES
chmod 755 $PRE_INST_PACKAGES
> $INST_PACKAGES
chmod 755 $INST_PACKAGES
> $PREINSTALL
chmod 755 $PREINSTALL
> $POSTINSTALL
chmod 755 $POSTINSTALL

#add missing ubuntu public keys for some vserver
sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 40976EAF437D05B5
sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 3B4FE6ACC0B21F32

apt-get update
apt-get dist-upgrade

for i in ./$LIST_DIR/[0-9][0-9][0-9]-*
do
 	echo "----- $i ----"
	IFS='
'
	for p in $(cat $i | sed 's/^#.*$//' | sed 's#\t\+.*$##')
	do
#	echo "[$p]"
		#check for url
	 	url="$(echo $p | sed -n '/^\(http\|https\|ftp\):\/\/.*.deb$/p')"	
		if [ ! "$url" = "" ]; then
			#echo "==URL:$url=="
			file="$(echo $url | sed -n '/^\(http\|https\|ftp\):\/\/.*.deb$/{s#.*/\(.*\.deb$\)#\1#;p}')"
			#echo "--FILE:$PKG_DIR/$file"
			test ! -f $PKG_DIR/$file && wget -O $PKG_DIR/$file $url
			dpkg -i $PKG_DIR/$file
		else
#echo "######1 $p-${p%%:*}"
			#check if a program should be called
			if [ "${p%%:*}" = "pre-cmd" ]; then
				cmd="${p#pre-cmd:}"
				echo "$cmd" >> $PREINSTALL
				continue
			fi
#echo "######2 $p-${p%%:*}"
			if [ "${p%%:*}" = "post-cmd" ]; then
				cmd="${p#post-cmd:}"
				echo "$cmd" >> $POSTINSTALL
				continue
			fi
#echo "######3 $p"
			if [ "${p%%:*}" = "pre-inst-cmd" ]; then
				cmd="${p#pre-inst-cmd:}"
				echo "$cmd" >> $PRE_INST_PACKAGES
				continue
			fi


			#check if we should use apt-get or dpkg
			if [ "${p##*.}" = "deb" ]; then
				cmd="dpkg -i $PKG_DIR/$p"
				echo "$cmd" >> $INST_PACKAGES
			else 
				cmd="apt-get --yes install $p"
				echo "$cmd" >> $INST_PACKAGES
			fi
		fi
	done
done

echo "========================================================="
echo "running pre install commands"
$PRE_INST_PACKAGES

echo "========================================================="
echo "running install packages"
$INST_PACKAGES

echo "========================================================="
echo "running pre-commands"
$PREINSTALL $FILES

echo "========================================================="
echo "copy files"
cp -r -d --preserve $FILES/* /

echo "========================================================="
echo "running post-commands"
$POSTINSTALL $FILES

echo "========================================================="
echo "finished."

