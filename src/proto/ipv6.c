/*
 * Parse IPv6 headers
 */
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* sys defs */
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip6.h>

#include "vcap.h"
#include "proto/vcap_proto.h"

static struct vcap_data_entry *ipv6_node;

void
vcap_packet_ipv6 (const u_char *packet __attribute__((unused)))
{
	VCAP_DATA_CREATE (ipv6_node, "ipv6", "eth");
	VCAP_DATA_INC (ipv6_node);
}

