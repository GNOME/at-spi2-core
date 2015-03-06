lib_LTLIBRARIES = libdummyatk.la

libdummyatk_la_CFLAGS = $(ATK_CFLAGS) \
			$(GLIB_CFLAGS) \
			$(GOBJ_CFLAGS) \
			-I$(top_srcdir)

libdummyatk_la_LDFLAGS = -no-undefined

libdummyatk_la_LIBADD = $(ATK_LIBS) \
			$(GLIB_LIBS) \
			$(GOBJ_LIBS)

libdummyatk_la_SOURCES = \
			 my-atk-action.c \
			 my-atk-action.h \
			 my-atk-component.c \
			 my-atk-component.h \
			 my-atk-object.c \
			 my-atk-object.h \
			 my-atk.h
