
nur die folgenden schritte machen und die debian installation ueberschreiben.
http://tinc-vpn.org/download/

./configure --prefix ''
make
strip src/tincd
cp src/tincd /usr/sbin/tincd


