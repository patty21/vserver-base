#!/bin/sh


export DATE="2013";
export TITLE="$(nvram get servername)"

./cgi-bin-pre.cgi

cat<<EOF
<H1>VPN Backbone Server</H1>

<CENTER><a href="http://info.ddmesh.de/info/topo-freifunk.png"><IMG ALT="Topology" BORDER="0" heigth="600" width="480" SRC="http://info.ddmesh.de/info/topo-freifunk.png" TITLE="topo-freifunk.png"></a><BR CLEAR="all">
</CENTER>


EOF

. ./cgi-bin-post.cgi
