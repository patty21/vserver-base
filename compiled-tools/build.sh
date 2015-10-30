#!/bin/bash
#compiles/installes all needed host tools

cd $(dirname $0)
install_dir=$(pwd)

echo "############## bmxd #########################"
cd $install_dir/bmx/bmxd
echo "compiling bmxd..."
make
make strip
echo "install bmxd"
cp bmxd /usr/bin/


echo "############## jshn #########################"
cd $install_dir/json
echo "compiling json..."
make
echo "install json"
cp jshn /usr/bin/
cp jshn.sh /usr/bin/


echo "############## mynuttcp #########################"
cd $install_dir/mynuttcp
echo "compiling mynuttcp..."
bunzip2 -k nuttcp-6.1.2.tar.bz2
tar xf nuttcp-6.1.2.tar
cd nuttcp-6.1.2
make
strip nuttcp-6.1.2
echo "install nuttcp-6.1.2"
cp nuttcp-6.1.2 /usr/bin/nuttcp


echo "############## myvtun #########################"
cd $install_dir/myvtun
echo "compiling myvtun..."
tar xzf vtun-3.0.3.tar.gz
cp my-modifications/* vtun-3.0.3/
cd vtun-3.0.3
./configure
make
strip vtund
echo "install myvtun"
cp vtund /usr/sbin/


echo "############## qrencode #########################"
cd $install_dir/qrencode
echo "compiling qrencode..."
tar xzf qrencode-3.4.2.tar.gz
cp myModification/* qrencode-3.4.2/
cd qrencode-3.4.2/
make
echo "install qrencode"
cp qrencode /usr/bin/


echo "############## tinc-1.1pre10 #########################"
cd $install_dir/tinc-1.1pre10
echo "compiling tinc..."
tar xzf tinc-1.1pre10.tar.gz
cd tinc-1.1pre10
./configure --prefix ""
make
strip src/tincd
echo "install tinc"
cp src/tincd /usr/sbin/


echo "############## fastd #########################"
cd $install_dir/fastd
./build.sh

