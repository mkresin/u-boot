
# Also see "include/polarssl/config.h"

CFLAGS	= -I../include -D_FILE_OFFSET_BITS=64 -Wall -Wdeclaration-after-statement
OFLAGS	= -O

# MicroBlaze specific options:
# CFLAGS += -mno-xl-soft-mul -mxl-barrel-shift

# To compile on Plan9:
# CFLAGS += -D_BSD_EXTENSION

# To compile as a shared library:
# CFLAGS += -fPIC

DLEXT=so
# OSX shared library extension:
# DLEXT=dylib

ifdef POLARSSL_TINY
OBJS_TINY =  bignum_tiny.o base64_tiny.o rsa_tiny.o sha4_tiny.o \
	     x509parse_tiny.o aes_tiny.o
else
OBJS=	aes.o		arc4.o		base64.o	\
	bignum.o	certs.o		debug.o		\
	des.o		dhm.o		havege.o	\
	md2.o		md4.o		md5.o		\
	net.o		padlock.o	rsa.o		\
	sha1.o		sha2.o		sha4.o		\
	ssl_cli.o	ssl_srv.o	ssl_tls.o	\
	timing.o	x509parse.o	xtea.o		\
	camellia.o
endif

.SILENT:

all: static

ifdef POLARSSL_TINY
CFLAGS += -fpic
static: libpolarssl_tiny.a
else
static: libpolarssl.a
endif

shared: libpolarssl.$(DLEXT)

libpolarssl.a: $(OBJS)
	echo "  AR    $@"
	ar r $@ $(OBJS)
	echo "  RL    $@"
	ranlib $@

libpolarssl_tiny.a: $(OBJS_TINY)
	echo "  AR    $@"
	ar r $@ $(OBJS_TINY)
	echo "  RL    $@"
	ranlib $@	

libpolarssl.so: libpolarssl.a
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS)

libpolarssl.dylib: libpolarssl.a
	echo "  LD    $@"
	$(CC) -dynamiclib -o $@ $(OBJS)

.c.o:
	echo "  CC    $<"
	$(CC) $(CFLAGS) $(OFLAGS) -c $<

clean:
	rm -f *.o libpolarssl.*

