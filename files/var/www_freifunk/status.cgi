#!/bin/sh

export DATE="14.4.2007";SCRIPT=${0#/rom}
export TITLE="Allgemein: Status"
. ./cgi-bin-pre.cgi

cat<<EOF
<h2>$TITLE</h2>
<br>
<fieldset class="bubble">
<legend>Allgemeines</legend>
<table>
<TR><th>Ger&auml;telaufzeit:</th><TD>$(uptime)</TD></TR>
</table>
</fieldset>
EOF

. ./cgi-bin-post.cgi
