# Freifunk Dresden: Basic Vserver
Configures an Ubuntu-Server (at least 14.10) as Freifunk-Dresden Server, that could be used as internet gateway an as basis to add further services.

Freifunk Ziel:
----
Freifunk hat es sich zum Ziel gesetzt, Menschen möglichst flächendeckend mit freiem WLAN zu versorgen. Freier Zugang zu Informationen ist nicht nur eine Quelle für Wissen, sondern kann sich auch positiv auf die wirtschaftliche und kulturelle Entwicklung einer Stadt, Region und Land auswirken, da das Internet in der heutigen Zeit sicher ein fester Bestandteil des täglichen Lebens geworden ist. Freifunk bietet die Möglichkeit, Internet per WLAN frei zu nutzen - ohne Zugangssperren und sicher, da der Internettraffic via verschlüsselten Internettunnel (VPN) ins Ausland geroutet wird. 

Basic Vserver
----

Dieses Repository bildet die minimale Funktionalität eines Servers für Freifunk Dresden. Der Vserver arbeitet wie
ein Freifunk Knoten und ist soweit konfiguriert, dass dieser eine Knotenwebseite anbietet, Backboneverbindungen
(vtund) akzeptiert und via Openvpn Internettunnel für den Internetverkehr aus dem Freifunknetz aufbauen kann.

HINWEIS:
- Der Vserver ist auf Freifunk Dresden zugeschnitten. Soll dieses als Basis für andere Freifunk Communities
verwendet werden, müssen Anpassungen gemacht werden.

- Es empfielt sich dringend für andere Communities, dieses Repository zu clonen, da hier generelle Umstellungen
zusammen mit der passenden Firmware für Dresnder Anforderungen erfolgen.
Communities sollten dann auf das geclonte Repository (gilt auch für das "firmware" Repository) aufbauen. Jede Community trägt die alleinige Verantwortung und Kontrolle über ihr Netz und sollte eigene Erfahrene Leute/Admins bereitstellen. Hilfe von Dresden ist aber jederzeit möglich, aber Administrative Aufgaben oder Garantien werden nicht übernommen, da das einfach den organisatorischen Aufwand sprengt.
Wir wissen selber nicht, wie sich das Netz in Zukunft noch verhält, wenn dieses weiter wächst.

- Routingprotokoll BMXD:<br/>
Diese Protokoll wurde anstelle von bmx6 oder bmx-advanced aus verschiedenen Gründen gewählt (siehe http://wiki.freifunk-dresden.de/). Es wird vom eigentlich Author nicht mehr weiterentwickelt oder gepflegt. Für Dresden habe ich einige Fehler behoben.
- Anpassungen:<br/>
Speziell gilt das für den IP Bereich und der Knotenberechnung. Aus Knotennummern werden mit ddmesh-ipcalc.sh 
alle notwendigen IP Adressen berechnet (http://wiki.freifunk-dresden.de/index.php/Technische_Information#Berechnung_IP_Adressen).
<br/><br/>
Freifunk Dresden verwendet zwei IP Bereiche, eines für die Knoten selber (10.200.0.0/16) und eines für das Backbone (10.201.0.0/16). Dieses ist technisch bedingt. Wird bei freifunk.net nur ein solcher Bereich reserviert (z.b. 10.13.0.0/16), so muss das Script ddmesh-ipcalc.sh in der Berechnung angepasst werden, so dass zwei separate Bereich entstehen. Die Bereiche für 10.13.0.0/16 würden dann 10.13.0.0/17 und 10.128.0.0/17 sein.
<br/><br/>
Das Script ddmesh-ipcalc.sh wird ebenfalls in der Firmware verwendet, welches dort auch angepasst werden muss.
In der Firmware gibt es zwei weitere Stellen, die dafür angepasst werden müssen. Das sind /www/nodes.cgi und /www/admin/nodes.cgi. Hier wurde auf den Aufruf von ddmesh-ipcalc.sh verzichtet und die Berechnung direkt gemacht, da die Ausgabe der Router-Webpage extrem lange dauern würde.
<br/><br/>
In  /etc/nvram.conf werden die meisten Einstellungen für den Vserver hinterlegt.
Evt. kann noch /etc/issuer.net angepasst werden, was beim Betreiben von mehreren Vservern hilfreich ist.

- Weiterhin verwendet das Freifunk Dresden Netz als Backbone-VPN Tool noch vtund. Dieses wird sich aber in 
 Zukunft ändern, wobei dann vermutlich fastd eingesetzt wird (für welches noch notwenige Funktionalitäten fehlen).
 
- /etc/openvpn enthält ein Script, mit dem verschiede Openvpn Konfiguration von Tunnelanbietern so aufbereitet werden, das diese für Freifunk Dresden richtig arbeiten.
Wie in der Firmware läuft per cron.d ein Internet-check, der in der ersten Stufe das lokale Internet testet und wenn dieses funktioniert, wird das Openvpn Internet geprüft. Ist das Openvpn Internet verfügbar, wird dieser Vserver als
Internet-Gateway im Freifunknetz bekannt gegeben.

- Auch der Vserver arbeitet als DNS Server für die Knoten, die ihn als Gateway ausgewählt haben. Der Vserver leitet allerdings die DNS Anfragen nicht über den Openvpn Tunnel, sondern geht direkt über den VServer Anbieter raus.

- ICVPN: Für ICVPN wird eigentlich alles installiert, aber ich habe dieses noch nicht mit dieser Installation getestet, da es ein Extra-Service ist und nicht auf jedem VServer in einem Netz aktiv sein braucht. Es gibt ein script, welches ebenfall dafür angepasst werden muss /etc/quagga/gen-bgpd.conf. Dieses Script greift auf github (wo alle Communities ihre Daten hinterlegen) und erzeugt entsprechende eine Konfiguration. Einfach mal damit experiementieren. Hilfreich sind hier die Befehle "ip rule" und "ip route list table zebra". Alternativ gibt es noch andere bgp Daemons, die von anderen Freifunk Communities verwendet werden (bird). Damit habe ich aber noch keine Experimente gemacht. Scripte für die Generierung von Konfigurationsfiles für bird gibt es auch irgendwo  im Github.

- Da VServer Anbieter verschieden sind, kann die Installation abbrechen. Hier sollten erfahrene Leute die Installation anpassen und mir einen Hinweis geben. Als Vserver kann <b>NICHT</b> jeder Anbieter genutzt werden. Derzeit funktionieren Netcup, Ispone, der Studenten Tarif Vserver von 1un1 für 1 Euro.
<br/></br/>
Wichtig ist, dass tun/tap devices und alle möglichen iptables module möglich sind. IPv6 ist nicht notwendig, da das Freifunk Netz in Dresden nur IPv4 unterstütz (Platzmangel auf Routern, bmxd unterstützt dieses nicht)

Installation:
----
Notwendig ist eine Ubuntu minimal Installation. Diese wird oft in der Version 14.04 TLS vom Vserveranbieter angeboten.

Schritte:<br/>

1. bringe Ubuntu auf die aktuelle Version. Das geht bei Ubuntu Schrittweise von Version zu Version.
https://help.ubuntu.com/community/UpgradeNotes
Wähle dafür aber die "server-⁠variante" (nicht desktop).

2. Für den Upgrade-Prozess wird der "update-manager" verwendet.
Dabei stolbert man über "TLS". Damit man auf 15.04 upgraden kann, muß<br/>
 /etc/update-manager/release-upgrades<br/>
 "Prompt" auf normal setzen.

3. Am Ende kann die Ubuntu-Version mit überprüft werden.<br/>
lsb_release -a
<pre>
No LSB modules are available.
Distributor ID: Ubuntu
Description:    Ubuntu 15.04
Release:        15.04
Codename:       vivid
</pre>


4. Folgends cloned das Repository. Bitte verwendet eurer eignes geclontes Repository.
<pre>
mkdir /⁠root/⁠freifunk
cd /⁠root/⁠freifunk
apt-⁠get install git
git clone git@github.com:ddmesh/vserver-base.git
cd freifunk-base
./⁠vserver-⁠install.sh
</post>
Es werden einige Pakete nachinstalliert, Files kopiert und am Ende noch einige Tools compiliert.
Wenn das fertig ist, müssen Community-Spezifische Dinge angepasst werden:
*ddmesh-ipcalc.sh
*/etc/nvram.conf
*/etc/quagga/gen-bgpd.conf (optional)
*/etc/openvpn/....
Mehr fällt mir gerade nicht ein ;-)

Wichig:
------
Im moment gibt es keinen Schutz, dass Routerfirmware einer Communitiy sich mit Servern oder Routern anderer Communities verbinden. Es ist <b>Fatal</b> wenn sich die Netze wegen gleicher WLAN BSSID oder via Backbone verbinden. Da überall das gleiche Routingprotokoll verwendet wird, würden Geräte von verschiedenen Communities miteinander reden können und das Netz würde gigantisch groß und die Router überlasten.

Bitte einhalten:
* Ändern der BSSID auf eine eigenen
* Keine Verwendung von Registratoren anderen Communities (Webserverdienst zum Verteilen von Knotennummern)
* Kein Aufbau von Brücken zwischen Routern/Vservern verschiedener Communities über Backboneverbindungen. (das wird in zukunft noch unterbunden, dazu ist aber eine Änderung am Routingprotokoll notwendig). Verbindungen von Communities dürfen nur über das ICVPN erfolgen.

Links:
------
<a href="www.freifunk-dresden.de" >Freifunk Dresden</a><br>
<a href="wiki.freifunk-dresden.de" >Wiki: Freifunk Dresden</a><br>
<a href="http://google.com/+FreifunkDresden%EF%BB%BF/about"> Google+</a><br>
<a href="https://plus.google.com/communities/108088672678522515509"> Google+ Community</a><br>
<a href="https://www.facebook.com/FreifunkDresden"> Facebook</a>


