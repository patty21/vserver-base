#!/bin/bash
#cmd tun_dev tun_mtu link_mtu ifconfig_local_ip ifconfig_remote_ip

#make a point-to-point connection with "route_vpn_gateway" because this was working for
#ovpn.to; Freie Netze e.V.;CyberGhost
ifconfig $dev $ifconfig_local dstaddr $route_vpn_gateway

m=${dev#vpn}
ip route add default dev $dev via $route_vpn_gateway table gateway_pool metric $m

iptables -t nat -A POSTROUTING -o vpn+ -j SNAT --to-source $ifconfig_local

#update gateway infos and routing tables, fast after openvpn open connection
#Run in background, else openvpn blocks
/usr/bin/freifunk-gateway-check.sh&

#tell always "ok" to openvpn;else in case of errors of "ip route..." openvpn exits
exit 0

