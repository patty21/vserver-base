/******************************************************************************
*  File:        netid.h
*  Date:		2.8.2016
*
*  Author:      Stephan Enderlein
*
*  Abstract:
*               Implementation netid functionality
*
*						
*******************************************************************************/

#ifndef _NETID_H
#define _NETID_H

#include <stdint.h>
#include "batman.h"

//initializes api
void netid_init(void);
void cleanup_netid(void);

//adds the network id to the list of network ids.
//if this was already added before, reference counter for this
//network id is incremented
void netid_add(uint32_t meshNetworkId);

//decrements ttl. should be called before sending ogm
uint32_t netid_decrement(uint32_t selectedNetworkId, uint32_t meshNetworkId);

//determines the best meshNetworkId
uint32_t netid_getSelectedMeshNetworkId(uint32_t selectedNetworkId, uint32_t meshNetworkId, uint32_t preferredNetworkId);

//writes all collected networks is to debug "--networks"
void netid_showNetworks(struct ctrl_node *cn, uint32_t selectedNetworkId, uint32_t meshNetworkId);

#endif // _NETID_H
