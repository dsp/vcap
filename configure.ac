AC_PREREQ(2.61)
AC_INIT(vcap, 0.0.1-dev, sn_@gmx.net)
AC_CONFIG_SRCDIR([src/vcap.c])
AC_CONFIG_HEADER([src/config.h])

AM_INIT_AUTOMAKE

AC_DEFINE([_GNU_SOURCE], [1], [We use glibc stuff])

AC_DISABLE_STATIC

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES(DEPS, gtk+-2.0 >= 2.2 glib-2.0 >= 2.2 libglade-2.0 >= 2.2 gmodule-2.0)


# Checks for header files.
AC_ISC_POSIX
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h time.h unistd.h netinet/ip.h netinet/tcp.h netinet/if_ether netinet/in.h arpa/inet.h sys/socket pcap.h getopt.h])
AC_FUNC_MALLOC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for flags
AC_MSG_CHECKING(if you want to enable debugging)
AC_ARG_ENABLE(debug,
   [  --enable-debug           specify you are building with debugging on],
   WITH_DEBUG=yes ; AC_DEFINE([DEBUG], [1], [Debugging]) , WITH_DEBUG=no)
AC_MSG_RESULT($WITH_DEBUG)

if test "$WITH_DEBUG" = "yes" ; then
   CFLAGS="-g -Wall -W -D_REENTRANT -D__NO_MATH_INLINES -fsigned-char"
fi

######
# pcap
######
AC_CHECK_HEADER(pcap.h,[enable_pcap=yes],[enable_pcap=no])
AC_CHECK_LIB([pcap],[pcap_open_live],enable_pcap="yes", enable_pcap="no")
AC_DEFINE([HAVE_PCAP], [1], [Define to 1 if you have packet capture library])

LIB_PCAP="-lpcap"
DEPS_LIBS="$DEPS_LIBS -lpcap -pthread"
DEPS_CFLAGS="$DEPS_CFLAGS -Isrc/"

# Checks for library functions.
AC_SUBST([LIB_PCAP])
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_OUTPUT(Makefile src/Makefile)
