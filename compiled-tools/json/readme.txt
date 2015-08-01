
Das script jshn.sh nutzt das binary jshn.
Es sollte wie im Beispiel in ein script eingebunden werden.

Beispiel:

#include functions
. /usr/bin/jshn.sh


#beispiel json, welches aber fuer das beispiel die falsche struktur hat.
json='{"address":"77.87.49.14","proto":"ipv4","country_code":"DE","country":"Germany"}'

#beispiel wurde aus der firmware von /usr/lib/ddmesh/ddmesh-utils-network-info.sh
#entnommen

 #load json to json parser                                  
 json_load "$json"                                                       
                                                                       
 #extract values from json                                             
 json_get_var net_device "device" # always valid if router has WAN port
 json_get_var net_up "up"                                              
 [ "$net_up" = "1" ] && {                                              
        json_get_var net_connect_time "uptime"                
                                                              
        #select object/array;get array entry 1;go one level up
        json_select "dns-server"                              
        json_get_type type "1"                                
        if [ "$type" = "string" ]; then       
                json_get_var net_dns 1        
        fi                                                             
        json_select ..                                                 
                                                                       
        json_select "ipv4-address"                                     
        json_get_type type "1"                                         
        if [ "$type" = "object" ]; then                                
                json_select 1                                 
                json_get_var net_ipaddr "address"             
                json_get_var net_mask "mask"                  
                json_select ..                                
        fi                                                    
        json_select ..                 
                                       
        json_select "route"            
        json_get_type type "1"                    
        if [ "$type" = "object" ]; then           
                json_select 1                     
                json_get_var net_gateway "nexthop"
                json_select ..                                
        fi                                    
        json_select ..                            
                                                                       
        #calculate rest                                                
        [ -n "$net_ipaddr" ] && {                                      
                eval $(ipcalc.sh $net_ipaddr/$net_mask)                
                net_broadcast=$BROADCAST                               
                net_netmask=$NETMASK                                   
                net_network=$NETWORK                          
        }                                                     
 }                                                            

