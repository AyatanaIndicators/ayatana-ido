ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

VER=3

V         = @
Q         = $(V:1=)
QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

SUBDIRS = src example tests

%-0.4.pc: %.pc
	$(QUIET_GEN) cp -f $< $@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libayatana-ido$(VER)-0.4.pc

CLEANFILES = libayatana-ido-0.4.pc libayatana-ido3-0.4.pc
DISTCLEANFILES = libayatana-ido.pc libayatana-ido3.pc
EXTRA_DIST = libayatana-ido.pc.in libayatana-ido3.pc.in

DISTCHECK_CONFIGURE_FLAGS = --disable-gtk-doc

include $(top_srcdir)/Makefile.am.coverage
