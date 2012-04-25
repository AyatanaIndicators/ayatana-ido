if USE_GTK3
VER=3
IDOLIB = $(top_builddir)/src/libido3-0.1.la
else
VER=
IDOLIB = $(top_builddir)/src/libido-0.1.la
endif

check_LIBRARIES = libgtest.a
check_PROGRAMS = 
TESTS =

AM_CPPFLAGS = \
	$(GTEST_CPPFLAGS) \
	-I${top_srcdir}/src

#############################
# Google Test base library
#############################

nodist_libgtest_a_SOURCES = \
	$(GTEST_SOURCE)/src/gtest-all.cc \
	$(GTEST_SOURCE)/src/gtest_main.cc
libgtest_a_CPPFLAGS = \
	$(GTEST_CPPFLAGS) -w
libgtest_a_CXXFLAGS = \
	$(AM_CXXFLAGS)
libgtest_a_CPPFLAGS = \
	$(AM_CPPFLAGS)

#############################
# Menuitem tests
#############################

TESTS += gtest-menuitems
check_PROGRAMS += gtest-menuitems

gtest_menuitems_SOURCES = \
	gtest-menuitems.cpp
gtest_menuitems_CPPFLAGS = \
	$(GCC_CFLAGS) \
	$(GTK_CFLAGS) \
	$(MAINTAINER_CFLAGS) \
	$(AM_CPPFLAGS)
gtest_menuitems_LDFLAGS = \
	-pthread
gtest_menuitems_LDADD = \
	$(GTK_LIBS) \
	$(IDOLIB) \
	libgtest.a

