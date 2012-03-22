#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "Xsocket.h"
#include "xhcp.hh"

int main(int argc, char *argv[]) {
	char pkt[XHCP_MAX_PACKET_SIZE];
	char ddag[XHCP_MAX_DAG_LENGTH];
	char *myAD, *gw_router_hid;
	// Xsocket init
	int sockfd = Xsocket(XSOCK_DGRAM);
	if (sockfd < 0) { error("Opening Xsocket"); }
	// dag init
	sprintf(ddag, "RE %s %s", BHID, SID_XHCP);
    	// store myAD and myHID
    	myAD = (char*)malloc(snprintf(NULL, 0, "%s", AD0) + 1);
    	sprintf(myAD, "%s", AD0);
     	gw_router_hid = (char*)malloc(snprintf(NULL, 0, "%s", RHID0) + 1);
    	sprintf(gw_router_hid, "%s", RHID0);
 
	xhcp_pkt beacon_pkt;
	beacon_pkt.seq_num = 0;
	beacon_pkt.num_entries = 2;
	xhcp_pkt_entry *ad_entry = (xhcp_pkt_entry*)malloc(sizeof(short)+strlen(myAD)+1);
	xhcp_pkt_entry *gw_entry = (xhcp_pkt_entry*)malloc(sizeof(short)+strlen(gw_router_hid)+1);
	ad_entry->type = XHCP_TYPE_AD;
	gw_entry->type = XHCP_TYPE_GATEWAY_ROUTER_HID;
	sprintf(ad_entry->data, "%s", myAD);
	sprintf(gw_entry->data, "%s", gw_router_hid);

	while (1) {
		// construct packet
		memset(pkt, 0, sizeof(pkt));
		int offset = 0;
		memcpy(pkt, &beacon_pkt, sizeof(beacon_pkt.seq_num)+sizeof(beacon_pkt.num_entries));
		offset += sizeof(beacon_pkt.seq_num)+sizeof(beacon_pkt.num_entries);
		memcpy(pkt+offset, &ad_entry->type, sizeof(ad_entry->type));
		offset += sizeof(ad_entry->type);
		memcpy(pkt+offset, ad_entry->data, strlen(ad_entry->data)+1);
		offset += strlen(ad_entry->data)+1;
		memcpy(pkt+offset, &gw_entry->type, sizeof(gw_entry->type));
		offset += sizeof(gw_entry->type);
		memcpy(pkt+offset, gw_entry->data, strlen(gw_entry->data)+1);
		offset += strlen(gw_entry->data)+1;
		// send out packet
		Xsendto(sockfd, pkt, offset, 0, ddag, strlen(ddag)+1);
		//fprintf(stderr, "XHCP beacon %ld\n", beacon_pkt.seq_num);
		beacon_pkt.seq_num += 1;
		sleep(XHCP_SERVER_BEACON_INTERVAL);
	}
	free(ad_entry);
	free(gw_entry);

	return 0;
}
