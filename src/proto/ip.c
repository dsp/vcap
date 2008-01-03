/* 
 * Parse ip headers
 */
#include "config.h"

#define __USE_BSD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* sys defs */
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>

/* network stuff */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <net/if.h>

/* protocol headers */
#include <netinet/if_ether.h> /* ethernet header */

#define __FAVOR_BSD
#include <netinet/tcp.h>	  /* tcp header */
#include <netinet/udp.h>	  /* udp header */

#include <netdb.h>

#include "vcap.h"
#include "proto/vcap_proto.h"

static struct vcap_data_entry *ip_node, *tcp_node, *udp_node, *icmp_node, *igmp_node, *sctp_node;

static void
vcap_packet_udp (const u_char * packet __attribute__((unused)))
{
  VCAP_DATA_CREATE (udp_node, "udp", "ip") VCAP_DATA_INC (udp_node);
}

static void
vcap_packet_icmp (const u_char * packet __attribute__((unused)))
{
  VCAP_DATA_CREATE (icmp_node, "icmp", "ip") VCAP_DATA_INC (icmp_node);
}

static void
vcap_packet_tcp (const u_char * packet)
{
  struct vcap_data_entry * e;
  const struct tcphdr *hdr;

  char prtchr[5];
  struct servent * db = NULL;

  VCAP_DATA_CREATE (tcp_node, "tcp", "ip") VCAP_DATA_INC (tcp_node);

  hdr =
    (struct tcphdr *) (packet + sizeof (struct ether_header) +
		       sizeof (struct ip));

  if (hdr->th_dport <= 65535 && hdr->th_dport > 0)
	{
	  if ((db = getservbyport (hdr->th_dport, "tcp")) != NULL)
		{
		  e = vcap_data_create (strdup(db->s_name), tcp_node);
		}
	  else
		{
		  sprintf(prtchr, "%d", ntohs(hdr->th_dport));
		  e = vcap_data_create (prtchr, tcp_node);
		}
	  VCAP_DATA_INC(e);
	}
}

static void
vcap_packet_igmp (const u_char * packet __attribute__((unused)))
{
  VCAP_DATA_CREATE (igmp_node, "igmp", "ip") VCAP_DATA_INC (igmp_node);
}

static void
vcap_packet_sctp (const u_char * packet __attribute__((unused)))
{
  VCAP_DATA_CREATE (sctp_node, "sctp", "ip") VCAP_DATA_INC (sctp_node);
}

void
vcap_packet_ip (const u_char * packet)
{
  const struct ip *hdr;

  VCAP_DATA_CREATE (ip_node, "ip", "eth");

  hdr = (struct ip *) (packet + sizeof (struct ether_header));

  VCAP_DATA_INC (ip_node);

  switch (hdr->ip_p)
    {
    case IPPROTO_TCP:
	  vcap_packet_tcp (packet);
	  break;
    case IPPROTO_UDP:
	  vcap_packet_udp (packet);
	  break;
    case IPPROTO_ICMP:
	  vcap_packet_icmp (packet);
	  break;
	case IPPROTO_IGMP:
	  vcap_packet_igmp (packet);
	  break;
	case IPPROTO_SCTP:
	  vcap_packet_sctp (packet);
	  break;
    default:
	  perror ("Unknown ip protocol\n");
    }
}
