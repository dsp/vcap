/*
 * Copyright (c) 2007 segv
 *
 * Licensed under the terms of the MIT License
 * See the LICENSE file
 * or http://www.opensource.org/licenses/mit-license.php
 *
 * TODO:
 *    - write a clean sigterm, siguser catcher to call pcap_breakloop()
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

/* libpcap */
#include <pcap.h>

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
#include <netinet/ip.h>		   /* ip header */
#include <netinet/if_ether.h>  /* ethernet header */
// #include <netinet/tcp.h>	   /* tcp stuff */

#include <pthread.h>		   /* threading */

#include "vcap.h"
#include "proto/vcap_proto.h"	/* ip analysis including tcp,udp */
#include "gui/vcap_gui.h"

char errbuff[PCAP_ERRBUF_SIZE];
pthread_mutex_t vcap_data_mutex;

/* holds information to pass to the thread routines 
 * this must not be a mutex as its just read never written
 * after the threads are started. */
static struct config_attr
{
  char *dev;
  int promsc;
  int quiet;
  int sleep;
} cmd_config =
{
"any", 0, 0, 1};

static struct vcap_data_entry *ether_node;

static int
vcap_listdevs (void)
{
  pcap_if_t *devs;
  int count = 0;

  printf ("Device list\n");

  pcap_findalldevs (&devs, errbuff);

  if (devs == NULL)
    {
      perror (errbuff);
      return -1;
    }

  do
    {
      printf ("  %s\n", devs->name);
      count++;
    }
  while (devs->next && (devs = devs->next));

  return count;
}

static const char *
vcap_finddev (const char *requested)
{
  pcap_if_t *devs;

  if (requested == NULL)
    {
      return NULL;
    }

  pcap_findalldevs (&devs, errbuff);

  if (devs == NULL)
    {
      perror (errbuff);
      return NULL;
    }

  do
    {
      if (strcmp (requested, devs->name) == 0)
		{
		  return requested;
		}
    }
  while (devs->next && (devs = devs->next));

  return NULL;
}

static void
vcap_capture_handler (u_char *user __attribute__((unused)), const struct pcap_pkthdr *h __attribute__((unused)), const u_char *sp)
{
  struct ether_header *ethdr;

  if (sp == NULL)
	{
	  fprintf (stderr, "Error receiving packages\n");
	  return;
	}

  VCAP_DATA_INC (ether_node);

  ethdr = (struct ether_header *) sp;
  switch (ntohs (ethdr->ether_type))
	{
	case ETHERTYPE_IP:
	  vcap_packet_ip (sp);
	  break;
	case ETHERTYPE_ARP:
	  vcap_packet_arp (sp);
	  break;
	  /*	case ETHERTYPE_IPV6:
	  vcap_packet_ipv6 (sp);
	  break;*/
	default:
#ifdef DEBUG
		printf("Unknown package: %#x\n", ntohs(ethdr->ether_type));
#endif
	  break;
	}
}

void *
vcap_capture_worker (void *param __attribute__((unused)))
{
  pcap_t *desc;
  bpf_u_int32 localnet, netmask;

  desc =
    pcap_open_live (cmd_config.dev, IP_MAXPACKET, cmd_config.promsc, 1000, errbuff);

  if (desc == NULL)
    {
      perror (errbuff);
	  exit(1);
    }

  /* XXX: Make direction as an option */
  pcap_setdirection (desc, PCAP_D_INOUT);

  if (pcap_lookupnet(cmd_config.dev, &localnet, &netmask, errbuff) < 0)
	{
	  localnet = 0;
	  netmask  = 0;
	  perror("Lookup Net failed\n");
	}

  /* add ether proto to tree */
  if (ether_node == NULL)
    {
      ether_node = vcap_data_create ("eth", NULL);
    }

  if (ether_node == NULL)
    {
      perror ("Cannot add ethernet protocol to data tree");
      pthread_exit (NULL);
    }

  pcap_loop (desc, -1, vcap_capture_handler, NULL);

  pthread_exit (NULL);
}

static void
usage (void)
{
  printf ("%s-%s by dsp\n", PACKAGE_NAME, PACKAGE_VERSION);
  printf ("licensed under the terms of the 3-clause BSD License\n\n");
  printf ("Usage: vcap [-i <if>] [-l] [-p] [-h]\n");
  printf ("  -i dev  interface to use\n");
  printf ("  -l      list available interfaces and exit\n");
  printf ("  -p      enable promiscuous mode\n");
  printf ("  -h      show this help\n");
  printf ("\n");
  printf ("using libpcap version %s from http://www.tcpdump.org\n", pcap_lib_version());
}

int
main (int argc, char *argv[])
{
  int opt;
  pthread_t worker[2];

  while ((opt = getopt (argc, argv, "i:lphq")) != -1)
    {
      switch (opt)
	{
	case 'q':
	  cmd_config.quiet = 1;
	  break;
	case 'i':
	  /* find dev */
	  if (vcap_finddev (optarg) == NULL)
	    {
	      fprintf (stderr, "Cannot find interface %s\n", optarg);
	      usage ();
	      exit (EXIT_FAILURE);
	    }
	  else
	    {
	      cmd_config.dev = optarg;
	    }
	  break;
	case 'l':
	  if (vcap_listdevs () == -1)
	    {
	      exit (EXIT_FAILURE);
	    }
	  else
	    {
	      exit (EXIT_SUCCESS);
	    }
	  break;
	case 'p':
	  /* enable promiscuous mode */
	  cmd_config.promsc = 1;
	  break;
	case 'h':
	  usage ();
	  exit (EXIT_SUCCESS);
	  break;
	}
    }

  printf ("using interface %s\n", cmd_config.dev);
  printf ("promiscuous mode %s\n",
	  (cmd_config.promsc == 0) ? "disabled" : "enabled");

#ifdef DEBUG
  printf ("debug build\n");
#endif

  /* initialize our main data mutex */
  pthread_mutex_init (&vcap_data_mutex, NULL);

  /* create thread for drawing */
  pthread_create (&worker[0], NULL, vcap_capture_worker, NULL);

  if (cmd_config.quiet == 1) 
    {
      pthread_create (&worker[1], NULL, vcap_gui_bgdisplayer, NULL); 
    }

  vcap_gui_process (&argc, &argv);

  pthread_mutex_destroy (&vcap_data_mutex);
  pthread_exit (NULL);

  return 0;
}
