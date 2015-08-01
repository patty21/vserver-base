#/bin/sh
#http://www.atwillys.de/content/linux/blocking-countries-using-geoip-and-iptables-on-ubuntu/
 
cd $(dirname $0) || exit 1;
BASE_DIR=$(pwd);
DL_DIR=$BASE_DIR/dl
 
mkdir -p $DL_DIR >/dev/null 2>&1
cd $DL_DIR || exit 1;
 
# Get data (only if modified)
wget -N http://geolite.maxmind.com/download/geoip/database/GeoIPv6.csv.gz || exit 1;
wget -N http://geolite.maxmind.com/download/geoip/database/GeoIPCountryCSV.zip || exit 1;
 
if [ -f GeoIPv6.csv.gz ]; then
  rm GeoIPv6.csv >/dev/null 2>&1
  gzip -dc GeoIPv6.csv.gz > GeoIPv6.csv || exit 1;
fi
 
if [ -f GeoIPCountryCSV.zip ]; then
  rm  GeoIPCountryWhois.csv >/dev/null 2>&1
  unzip GeoIPCountryCSV.zip >/dev/null || exit 1;
fi
 
echo "Building ..."
cat *.csv | perl $BASE_DIR/xt_geoip_build.pl -D $DL_DIR || exit 1;
rm *.csv
 
echo "Copying to $BASE_DIR ..."
rm -rf $BASE_DIR/LE && mv -f $DL_DIR/LE $BASE_DIR/
rm -rf $BASE_DIR/BE && mv -f $DL_DIR/BE $BASE_DIR/
 
echo "Ready."

