ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
NULL=

V         = @
Q         = $(V:1=)
QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

SUBDIRS = src example tests

%-0.4.pc: %.pc
	$(QUIET_GEN) cp -f $< $@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libayatana-ido3-0.4.pc

CLEANFILES = \
	libayatana-ido3-0.4.pc \
	$(NULL)

DISTCLEANFILES = \
	libayatana-ido.pc \
	libayatana-ido3.pc \
	Makefile.in \
	aclocal.m4 \
	autom4te.cache/ \
	compile \
	config.guess \
	config.h.in \
	config.h.in~ \
	config.sub \
	configure \
	depcomp \
	gtk-doc.make \
	install-sh \
	ltmain.sh \
	m4/gtk-doc.m4 \
	m4/libtool.m4 \
	m4/ltoptions.m4 \
	m4/ltsugar.m4 \
	m4/ltversion.m4 \
	m4/lt~obsolete.m4 \
	missing \
	omf.make \
	test-driver \
	xmldocs.make \
	$(NULL)

EXTRA_DIST = libayatana-ido3.pc.in

DISTCHECK_CONFIGURE_FLAGS = --disable-gtk-doc --disable-silent-rules

include $(top_srcdir)/Makefile.am.coverage
