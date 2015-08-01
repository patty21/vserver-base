#!/bin/sh
echo 'Content-type: text/plain txt'
echo ''

DB_PATH=/var/lib/freifunk/bmxd

eval $(ddmesh-ipcalc.sh -n $(nvram get ddmesh_node))

#c:Dresden,node,domain,ip,gps-longitude,gps-latitude,gps-altitude,Name,Ort,email,notiz
echo "c:Dresden,$_ddmesh_node,$_ddmesh_domain,$_ddmesh_ip,$(nvram get gps_longitude),$(nvram get gps_latitude),0,$(nvram get contact_name),Dresden,,$(ls -1 /etc/openvpn/*.conf | sed -n 's#.*/\([^.]\+\)\..*#Tunnel via: \1#p')"
#echo "N:Freifunk Server (VPN,ICVPN,WWW)"

#routes
ip route list table bat_route | sed 's#\(.*\)#R:\1#'
ip route list table bat_hna | sed 's#\(.*\)#H:\1#'
ip route list table bat_default | sed 's#\(.*\)#T:\1#'

#gateways
ip route list table main | grep default | sed 's#\(.*\)#G:\1#'

sudo bmxd -cd8 | sed -n '/^WARNING/ {n}; s#\(^.*$\)#B:\1#g;p'
sudo bmxd -cd2 | sed -n '/^WARNING/ {n}; s#\(^.*$\)#b:\1#g;p'
sudo bmxd -ci | sed 's#^[ 	]*\(.*\)$#i:\1#'

#notes
ip route list table gateway | grep default | sed 's#\(.*\)#G:\1#'

#tinc
TINC_CONF=/etc/tinc/backbone/tinc.conf
if [ -f $TINC_CONF ]; then
	this=$(grep -iw name $TINC_CONF)
	this=${this#*=}
	this=$(echo $this | sed 's#[ 	]*##g')
	for i in $(grep -i connectto $TINC_CONF)
	do
		src=${i#*=}
		src=$(echo $src | sed 's#[ 	]*##g')
		cat $DB_PATH/tinc/tinc-backbone.dot | sed 's#[ 	]*##g' | grep -- "$this->$src" | sed 's#\(.*\)->\(.*\);#V:\1;\2#'
	done
fi

#vtun
src="node_"$_ddmesh_node
#alt: backbone-10-1 -> node_10_1
#neu: backbone-r160 -> node_160
for i in $(ps ax|grep ' vtund\[s\]:[ 	]*backbone[0-9]*-' | sed 's#^.*:[ 	]*backbone[0-9]*-r\([0-9-]\+\).*$#node_\1#;s#-#_#g')
do
        echo "v:$i;$src"
done

