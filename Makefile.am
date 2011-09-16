ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

if USE_GTK3
VER=3
else
VER=
endif

V         = @
Q         = $(V:1=)
QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

SUBDIRS = src example

%-0.1.pc: %.pc
	$(QUIET_GEN) cp -f $< $@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libido$(VER)-0.1.pc

CLEANFILES = libido-0.1.pc libido3-0.1.pc
DISTCLEANFILES = libido.pc libido3.pc
EXTRA_DIST = libido.pc.in libido3.pc.in

DISTCHECK_CONFIGURE_FLAGS = --disable-gtk-doc
