lib_LTLIBRARIES = libdummyatk.la

libdummyatk_la_CFLAGS = $(ATK_CFLAGS) \
			$(GLIB_CFLAGS) \
			$(GOBJ_CFLAGS) \
			-I$(top_srcdir) \
			-Wall

libdummyatk_la_LDFLAGS = -no-undefined

libdummyatk_la_LIBADD = $(ATK_LIBS) \
			$(GLIB_LIBS) \
			$(GOBJ_LIBS)

libdummyatk_la_SOURCES = \
			 my-atk-action.c \
			 my-atk-action.h \
			 my-atk-component.c \
			 my-atk-component.h \
			 my-atk-editable-text.c \
			 my-atk-editable-text.h \
			 my-atk-image.c \
			 my-atk-image.h \
			 my-atk-text.c \
			 my-atk-text.h \
			 my-atk-object.c \
			 my-atk-object.h \
			 my-atk-value.c \
			 my-atk-value.h \
			 my-atk.h
