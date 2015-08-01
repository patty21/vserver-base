# Freifunk Dresden: Basic Vserver
Configures an Ubuntu-Server (at least 14.10) as Freifunk-Dresden Server, that could be used as internet gateway an as basis to add further services.

Freifunk Ziel:
----
Freifunk hat es sich zum Ziel gesetzt, Menschen möglichst flächendeckend mit freiem WLAN zu versorgen. Freier Zugang zu Informationen ist nicht nur eine Quelle für Wissen, sondern kann sich auch positiv auf die wirtschaftliche und kulturelle Entwicklung einer Stadt, Region und Land auswirken, da das Internet in der heutigen Zeit sicher ein fester Bestandteil des täglichen Lebens geworden ist. Freifunk bietet die Möglichkeit, Internet per WLAN frei zu nutzen - ohne Zugangssperren und sicher, da der Internettraffic via verschlüsselten Internettunnel (VPN) ins Ausland geroutet wird. 

Basic Vserver
----

Dieses Repository bildet die minimale Funktionalität eines Servers für Freifunk Dresden. Der arbeitet wie
ein Freifunk Knoten und ist soweit konfiguriert, dass dieser eine Knotenwebseite anbietet, Backboneverbindungen
(vtund) akzeptiert und via Openvpn Internettunnel für den Internetverkehr aus dem Freifunknetz aufbauen kann.

HINWEIS:
- Der Vserver ist auf Freifunk Dresden zugeschnitten. Soll dieses als Basis für andere Freifunk Communities
verwendet werden, müssen Anpassungen gemacht werden.
<br/><br/>
Speziell gilt das für den IP Bereich und der Knotenberechnung. Aus Knotennummern werden mit ddmesh-ipcalc.sh 
alle notwendigen IP Adressen berechnet (http://wiki.freifunk-dresden.de/index.php/Technische_Information#Berechnung_IP_Adressen).
<br/><br/>
Freifunk Dresden verwendet zwei IP Bereiche, eines für die Knoten selber (10.200.0.0/16) und eines für das Backbone (10.201.0.0/16). Dieses ist technisch bedingt. Wird bei freifunk.net nur ein solcher Bereich reserviert (z.b. 10.13.0.0/16), so muss das Script ddmesh-ipcalc.sh in der Berechnung angepasst werden, so dass zwei separate Bereich entstehen. Die Bereiche für 10.13.0.0/16 würden dann 10.13.0.0/17 und 10.128.0.0/17 sein.
<br/><br/>
Das Script ddmesh-ipcalc.sh wird ebenfalls in der Firmware verwendet, welches dort auch angepasst werden muss.
In der Firmware gibt es zwei weitere Stellen, die dafür angepasst werden müssen. Das sind /www/nodes.cgi und /www/admin/nodes.cgi. Hier wurde auf den Aufruf von ddmesh-ipcalc.sh verzichtet und die Berechnung direkt gemacht, da die Ausgabe der Router-Webpage extrem lange dauern würde.
<br/><br/>
In  /etc/nvram.conf werden Einstellungen die meisten Einstellungen für den Vserver hinterlegt.
Evt. kann noch /etc/issuer.net angepasst werden, was beim Betreiben von mehreren Vservern hilfreich ist.

- Weiterhin verwendet das Freifunk Dresden Netz als Backbone-VPN Tool noch vtund. Dieses wird sich aber in 
 Zukunft ändern, wobei dann vermutlich fastd eingesetzt wird (für welches noch notwenige Funktionalitäten fehlen).
 
- /etc/openvpn enthält ein Script, mit dem verschiede Openvpn Konfiguration von Tunnelanbietern so aufbereitet werden, das diese für Freifunk Dresden richtig arbeiten.
Wie in der Firmware läuft per cron.d ein Internet-check, der in der ersten Stufe das lokale Internet testet und wenn dieses funktioniert, wird das Openvpn Internet geprüft. Ist das Openvpn Internet verfügbar, wird dieser Vserver als
Internet-Gateway im Freifunknetz bekannt gegeben.

- Auch der Vserver arbeitet als DNS Server für die Knoten, die ihn als Gateway ausgewählt haben. Der Vserver leitet allerdings die DNS Anfragen nicht über den Openvpn Tunnel, sonder geht direkt über den VServer Anbieter raus.

- ICVPN: Für ICVPN wird eigentlich alles installiert, aber ich abe dieses noch nicht mit dieser Installation getestet, da es ein Extra-Service ist und nicht auf jedem VServer in einem Netz aktiv sein braucht. Es gibt ein script, welches ebenfall dafür angepasst werden muss /etc/quagga/gen-bgpd.conf. Dieses Script greift auf github (wo alle Communities ihre Daten hinterlegen) und erzeugt entsprechende eine Konfiguration. Einfach mal damit experiementieren. Hilfreich sind hier die Befehle "ip rule" und "ip route list table zebra". Alternativ gibt es noch andere bgp Daemons, die von anderen Freifunk Communities verwendet werden (bird). Damit habe ich aber noch keine Experimente gemacht. Scripte für die Generierung von Konfigurationsfiles für bird gibt es auch irgendwo  im Github.

Links:
------
<a href="www.freifunk-dresden.de" >Freifunk Dresden</a><br>
<a href="wiki.freifunk-dresden.de" >Wiki: Freifunk Dresden</a><br>
<a href="http://google.com/+FreifunkDresden%EF%BB%BF/about"> Google+</a><br>
<a href="https://plus.google.com/communities/108088672678522515509"> Google+ Community</a><br>
<a href="https://www.facebook.com/FreifunkDresden"> Facebook</a>


