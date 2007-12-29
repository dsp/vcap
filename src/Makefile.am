bin_PROGRAMS = vcap
vcap_SOURCES = vcap.c data.c proto/ip.c proto/ipv6.c proto/arp.c gui/draw.c gui/sunburst.c

AM_CFLAGS = @DEPS_CFLAGS@ @CFLAGS@
AM_LDFLAGS = @DEPS_LIBS@ --export-dynamic
