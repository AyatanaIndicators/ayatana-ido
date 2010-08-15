noinst_PROGRAMS =               \
	gesture			\
	messagedialog		\
        menus

gesture_SOURCES =		\
	gesture.c

gesture_CPPFLAGS =		\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	$(GCC_FLAGS)		\
	$(GTK_CFLAGS)		\
	$(MAINTAINER_CFLAGS)

messagedialog_SOURCES =		\
	messagedialog.c

menus_SOURCES =                 \
        menus.c

messagedialog_CPPFLAGS =	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/src	\
	-I$(top_builddir)/src	\
	$(GCC_FLAGS)		\
	$(GTK_CFLAGS)		\
	$(MAINTAINER_CFLAGS)

menus_CPPFLAGS =                \
        -I$(top_srcdir)         \
        -I$(top_srcdir)/src     \
        -I$(top_builddir)/src   \
        $(GCC_FLAGS)            \
        $(GTK_CFLAGS)           \
        $(MAINTAINER_CFLAGS)

gesture_LDADD = $(top_builddir)/src/libido-0.1.la

messagedialog_LDADD = $(top_builddir)/src/libido-0.1.la

menus_LDADD = $(top_builddir)/src/libido-0.1.la
