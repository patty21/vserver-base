#!/bin/sh

#commands $1: up, down, connect, establish, disestablish, verify

#environment variables for valid for all commands
# FASTD_PID: fastd’s PID
# INTERFACE: the interface name
# INTERFACE_MTU: the configured MTU
# LOCAL_KEY: the local public key

#environment variables for: connect, establish, disestablish
# LOCAL_ADDRESS: the local IP address
# LOCAL_PORT: the local UDP port
# PEER_ADDRESS: the peer’s IP address
# PEER_PORT: the peer’s UDP port
# PEER_NAME: the peer’s name in the local configuration
# PEER_KEY: the peer’s public key

# cmd: verify
# on verifiy is called when an unknown connection attempt was made
# when command returns 0 then this connection is accepted (default not)
# evt kann ich dadurch jede verbindung zum testen erlauben

false && {
cat <<EOM >>/tmp/fastd-cmd-env.log
---------------------------------------
command: $1

FASTD_PID: $FASTD_PID
INTERFACE: $INTERFACE
INTERFACE_MTU: $INTERFACE_MTU
LOCAL_KEY: $LOCAL_KEY

LOCAL_ADDRESS: $LOCAL_ADDRESS
LOCAL_PORT: $LOCAL_PORT
PEER_ADDRESS: $PEER_ADDRESS
PEER_PORT: $PEER_PORT
PEER_NAME: $PEER_NAME
PEER_KEY: $PEER_KEY

EOM
}

eval $(ddmesh-ipcalc.sh -n $(nvram get ddmesh_node))

case $1 in

 up)
  /sbin/ip link set $INTERFACE down
  /sbin/ip link set $INTERFACE promisc off
  /sbin/ip link set $INTERFACE multicast off mtu $INTERFACE_MTU
  /sbin/ip addr add $_ddmesh_nonprimary_ip/$_ddmesh_netpre broadcast $_ddmesh_broadcast dev $INTERFACE
  /sbin/ip link set $INTERFACE up
  /etc/init.d/S52batmand addif $INTERFACE
 ;;

 down)
  /etc/init.d/S52batmand delif $INTERFACE
  /sbin/ip link set $INTERFACE down
  /sbin/ip addr del $_ddmesh_nonprimary_ip/$_ddmesh_netpre broadcast $_ddmesh_broadcast dev $INTERFACE
 ;;


esac

exit 0

