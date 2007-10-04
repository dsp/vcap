/* 
 * Parse ip headers
 */
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/if_ether.h>	/* ethernet header */
#include <netinet/ip.h>		/* ip header */
#include <netinet/tcp.h>	/* tcp header */
#include <netinet/udp.h>	/* udp header */
#include <arpa/inet.h>
#include <netdb.h>

#include "vcap.h"
#include "proto/vcap_proto.h"

static struct vcap_data_entry *ip_node, *tcp_node, *udp_node, *icmp_node;

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

  /* buffer overflow warning */
  char prtchr[5];
  struct servent * db = NULL;

  VCAP_DATA_CREATE (tcp_node, "tcp", "ip") VCAP_DATA_INC (tcp_node);

  hdr =
    (struct tcphdr *) (packet + sizeof (struct ether_header) +
		       sizeof (struct ip));

  if (hdr->dest <= 65535) 
	{
	  if ((db = getservbyport (hdr->dest, "tcp")) != NULL)
		{
		  e = vcap_data_create (strndup(db->s_name, IDENT_MAX), tcp_node);
		}
	  else
		{
		  sprintf(&prtchr, "%d", ntohs(hdr->dest));	 
		  e = vcap_data_create (&prtchr, tcp_node);
		}
	  VCAP_DATA_INC(e);
	}
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
    default:
      perror ("Unknown ip protocol\n");
    }
}
