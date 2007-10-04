#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

#include "vcap.h"
#include "proto/vcap_proto.h"

static struct vcap_data_entry *arp_node;

void
vcap_packet_arp (const u_char * packet)
{
  VCAP_DATA_CREATE (arp_node, "arp", "eth") VCAP_DATA_INC (arp_node);
}
