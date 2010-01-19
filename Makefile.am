ACLOCAL_AMFLAGS = -I build/autotools

V         = @
Q         = $(V:1=)
QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

SUBDIRS = build src example

libido-0.1.pc: libido.pc
	$(QUIET_GEN) cp -f libido.pc libido-0.1.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libido-0.1.pc

CLEANFILES = libido-0.1.pc
DISTCLEANFILES = libido.pc
EXTRA_DIST = libido.pc.in

DISTCHECK_CONFIGURE_FLAGS = --disable-gtk-doc
