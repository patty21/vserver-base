/******************************************************************************
*  File:        netid.h
*  Date:		2.8.2016
*
*  Author:      Stephan Enderlein
*
*  Abstract:
*               Implementation netid functionality
*
*               bmxd has two parameters meshNetworkId and meshNetworkIdPreferred.
*               meshNetworkId always selects and uses the mesh network id. it overwrites
*               meshNetworkIdPreferred.
*
*               meshNetworkId should only be set by vserver to force a specific mesh network id and
*               do disable automatic selection. a router using this would never be part of network if
*               no other router with same id is reachable.
*
*               meshNetworkIdPreferred is used by router. When this is set, automatic selection
*               process determines the selected mesh network id. this can change over the time, depending
*               whether a network disappears.
*
*               If there is always a preferred network in range and at least x packages are arrived, this
*               network id is selected. if preferred network is not found or there are not enough packages seen,
*               network meshid 0 is selected.
*
*               The selected meshNetworkId is meshNetworkIdSelected. if this is zero
*               bmxd behaves like bmxd compatibility version 10.
*               bmxd sends OGMs with no meshNetworkId.
*
*               As long as there is a ttl > 0 we know that there is a "master" and we can stick to this networkid.
*               When master goes off, then ttl falls to zero. If ttl is zero, default network id is selected until
*               the preferred network id was seen again. this ensures that we have at least a fallback id.
**
*               bmxd stores this selected network id and only accepts OGMs with this
*               new meshNetworkIdSelected. It also does only sent OGMs with meshNetworkIdSelected.
*
*               When bmxd starts and meshNetworkId was not set at command line, it starts with meshNetworkIdSelected==0.
*
*               Specific to freifunk dresden and automatic registration
*               -------------------------------------------------------
*               This automatic selection was implemented because if someone setups
*               a new freifunk node that is only connected via WLAN without own internet connection
*               a node could not register itself with the registrator to get an offical node
*               number. With this automatic meshNetworkId selection the node is connected to the
*               network where it has received the most OGMs with same network id. Higher number can be a result
*               of many nodes in neighbourhood.
*               Later when the node registers itself with the registrator it might get a new preferred mesh network id
*               assigned. freifunk dresden firmware would store and pass the new preferred mesh network id a
*               command line parameter.
*
*******************************************************************************/


#include <batman.h>
#include <originator.h>
#include <netid.h>
#include <objlist.h>
#include <schedule.h>

#define NETID_MEMORY_TAG    1000

//number of seen ogms after that corresponding meshNetworkId will be selected
#define NETID_MINIMUM_PREFERRED_ID_COUNT 10 //should be less than NETID_MAXIMUM_ID_COUNT (NETID_MINIMUM_ID_COUNT)
#define NETID_MAXIMUM_ID_COUNT  50

//defines the time in ms after which refCounts is checked and decremented (see usage of this define)
//The comparison was added to keep counter incrementing if only one node (master) is sending ogm with specific
//meshNetworkId. Incrementation would be to slow for networks with a lot of ogms. later when more nodes
//are (re)sending OGMs with same meshNetworkId, then this counter increments faster.
#define NETID_DECREMENT_POLL_TIME_MS 1000
#define NETID_DECREMENT_TIMEOUT_MS 15000  //start after 15s to decrement in NETID_DECREMENT_POLL_TIME_MS steps

//defines the ttl value indicating the node that forces to use the meshNetworkId.
#define NETID_MASTER_TTL    255

static LIST_ENTRY meshNetworkIdList;

typedef struct
{
    LIST_ENTRY list_entry;
    uint32_t meshNetworkId;
    uint32_t refCount;  //incremented when new ogm has received; decremented if no ogm has been received
                        //within NETID_DECREMENT_POLL_TIME_MS
    batman_time_t lastSeen; //holds the time in milli seconds, when network OGM was seen last
} MeshNetworkIdList_t;


static void netid_decrementRefCount( void* unused );


void netid_init(void)
{
    OLInitializeListHead(&meshNetworkIdList);

    //setup auto decrement task (one-shot)
    register_task( NETID_DECREMENT_POLL_TIME_MS, netid_decrementRefCount, NULL );
}

//adds the network id to the list of network ids.
//if this was already added before, reference counter for this
//network id is incremented
void netid_add(uint32_t meshNetworkId)
{
#if 0
    //if we have default network id than assum that it is a master
    //else nodes with preferred network id of zero won't stay selected.
    //means that ttl be refreshed.
    if(GET_MESH_NET_ID(meshNetworkId)==DEF_MESH_NET_ID)
    {
      meshNetworkId = DEF_MESH_NET_ID | SET_MESH_NET_TTL(NETID_MASTER_TTL);
    }
#endif
    //search for meshNetworkId
    OLForEach(entry, MeshNetworkIdList_t, meshNetworkIdList)
    {
        if(GET_MESH_NET_ID(entry->meshNetworkId) == GET_MESH_NET_ID(meshNetworkId))
        {
            if(entry->refCount < NETID_MAXIMUM_ID_COUNT) ++entry->refCount;

            //when receiving a new ttl I only determine if it was incremented or decremented.
            //large changes because of ttl values from far nodes with low ttl values would
            //simply cause to decrement ttl by one.
            //Rule: 1. determine tendence
            //      2. if new ttl is lower I decrement it by one
            //      3. if new ttl is higher I just use this new value
            //         This allows fast propagation of high ttl when new "master" is found


            if(GET_MESH_NET_TTL(meshNetworkId) < GET_MESH_NET_TTL(entry->meshNetworkId))
            {
                uint32_t ttl = GET_MESH_NET_TTL(entry->meshNetworkId);
                if(ttl > 0) --ttl;
                entry->meshNetworkId = GET_MESH_NET_ID(entry->meshNetworkId) | SET_MESH_NET_TTL(ttl);
                //subtract one second, so the check in netid_decrementRefCount() will decrement ref count
                //after a while
                if(entry->lastSeen > 5000)
                    entry->lastSeen -= 1000; //1000ms
            }
            else
            {
                entry->meshNetworkId = meshNetworkId;
                entry->lastSeen = batman_time; //update time only when we got an "expected" paket with a ttl that
                         //could be valid
            }

            return;
        }
    }

    //insert new entry
    MeshNetworkIdList_t *pNew = debugMalloc(sizeof(MeshNetworkIdList_t), NETID_MEMORY_TAG);
    if(!pNew)
    {
        dbgf( DBGL_SYS, DBGT_ERR, "could not allocate memory for MeshNetworkIdList_t");
        return;
    }

    //add id with current ttl. ttl is decremented when sending
    pNew->meshNetworkId = meshNetworkId;
    pNew->refCount = 1;
    pNew->lastSeen = batman_time;
    OLInsertTailList(&meshNetworkIdList,(PLIST_ENTRY) pNew);
}

// decrements ttl but only if current router is not a "master" (commandline param meshNetworkId>0)
// Das ist notwendig, damit die ttl altert und ein anderes network gewaehlt wird
// wenn keine neuen pakete mit einer hohen ttl von einem "master" kommen.
uint32_t netid_decrement(uint32_t selectedNetworkId, uint32_t meshNetworkId)
{
    //if we are "master", always set ttl to max
    if(meshNetworkId > 0)
    {
        return GET_MESH_NET_ID(selectedNetworkId) | SET_MESH_NET_TTL(NETID_MASTER_TTL);
    }
#if 0
    //search for meshNetworkId and update
    OLForEach(entry, MeshNetworkIdList_t, meshNetworkIdList)
    {
        if(GET_MESH_NET_ID(entry->meshNetworkId) == GET_MESH_NET_ID(selectedNetworkId))
        {
            uint32_t ttl = GET_MESH_NET_TTL(entry->meshNetworkId);
            if(ttl > 0) --ttl;
            uint32_t id = GET_MESH_NET_ID(entry->meshNetworkId) | SET_MESH_NET_TTL(ttl);

            entry->meshNetworkId = id;
            return id;
        }
    }

    //should never come here
    return selectedNetworkId;
#else
    uint32_t ttl = GET_MESH_NET_TTL(selectedNetworkId);
    if(ttl > 0) --ttl;
    return GET_MESH_NET_ID(selectedNetworkId) | SET_MESH_NET_TTL(ttl);
#endif
}

//determines the best meshNetworkId
//selectedNetworkId - currently selected/used network id
//meshNetworkId     - when passed as command line
//preferredNetworkId- when passed as command line
uint32_t netid_getSelectedMeshNetworkId(uint32_t selectedNetworkId, uint32_t meshNetworkId, uint32_t preferredNetworkId)
{
    MeshNetworkIdList_t *pSelected=NULL;
    uint32_t refCount = 0;

    //if this router acts as "master"
    if(meshNetworkId > 0)
    {
        uint32_t id = GET_MESH_NET_ID(meshNetworkId) | SET_MESH_NET_TTL(NETID_MASTER_TTL);
        static uint32_t rate=0;

        //add master meshNetworkId to list but there is no need to do it every time, so reduce rate. start at first call
        if(rate == 0)
        {
            netid_add(id);
            rate=10;   //reset
        }
        --rate;

        return id;
    }

    //check each entry and remember highest value
    OLForEach(entry, MeshNetworkIdList_t, meshNetworkIdList)
    {
        //preferred network id may be zero or any other value. if we have seen the preferred network
        //just return the id.
        if( GET_MESH_NET_ID(entry->meshNetworkId) == GET_MESH_NET_ID(preferredNetworkId)
                && GET_MESH_NET_TTL(entry->meshNetworkId) > 0
                && entry->refCount >= NETID_MINIMUM_PREFERRED_ID_COUNT)
        {
            return entry->meshNetworkId;
        }


        //only consider networks with "master". this is preferred over network id 0
        if(entry->refCount > refCount && GET_MESH_NET_TTL(entry->meshNetworkId) > 0
                && entry->refCount >= NETID_MINIMUM_PREFERRED_ID_COUNT)
        {
            pSelected = entry;
            refCount = entry->refCount;
        }
    }
    //if we found a master
    if(pSelected)
    {
        return pSelected->meshNetworkId;
    }

    //if no other was found, we fall back to zero
    return DEF_MESH_NET_ID;
}


void netid_showNetworks(struct ctrl_node *cn, uint32_t selectedNetworkId, uint32_t meshNetworkId)
{
    dbg_cn( cn, DBGL_ALL, DBGT_NONE, "selected Id: %lu, ttl: %lu, preferred Id: %d",
            GET_MESH_NET_ID(selectedNetworkId),
            GET_MESH_NET_TTL(selectedNetworkId),
            GET_MESH_NET_ID(meshNetworkId) > 0 ? -1 : GET_MESH_NET_ID(meshNetworkIdPreferred));

    OLForEach(entry, MeshNetworkIdList_t, meshNetworkIdList)
    {
        dbg_cn( cn, DBGL_ALL, DBGT_NONE, "%s\tid: %10d\trefCount: %lu\trcvdTTL: %lu",
                GET_MESH_NET_ID(selectedNetworkId) == GET_MESH_NET_ID(entry->meshNetworkId) ? "->" :"",
                GET_MESH_NET_ID(entry->meshNetworkId), entry->refCount, GET_MESH_NET_TTL(entry->meshNetworkId));
    }
}

void cleanup_netid(void)
{
    for(PLIST_ENTRY entry = OLGetNext(&meshNetworkIdList); !OLIsListEmpty(&meshNetworkIdList); entry = OLGetNext(&meshNetworkIdList))
    {
        OLRemoveEntryList(entry);
        debugFree(entry, NETID_MEMORY_TAG);
    }
}

void netid_decrementRefCount( void* unused )
{
    OLForEach(entry, MeshNetworkIdList_t, meshNetworkIdList)
    {
        if(entry->refCount > 0)
        {
            //start with decrement after we haven't seen a new paket since some time
            if( (entry->lastSeen + NETID_DECREMENT_TIMEOUT_MS) < batman_time)
            {
               --entry->refCount;
            }
        }
        else
        {
            //set ttl=0 (no master for this network id present in network)
            entry->meshNetworkId = GET_MESH_NET_ID(entry->meshNetworkId);
        }
    }//for

    register_task( NETID_DECREMENT_POLL_TIME_MS, netid_decrementRefCount, NULL );
}
