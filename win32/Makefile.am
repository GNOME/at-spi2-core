if HAVE_INTROSPECTION
GENERATED_ITEMS = \
	introspection.body.mak	\
	Atk_1_0_gir_list

introspection.body.mak: Atk-1.0.gir.msvc.introspect
	-$(RM) introspection.body.mak
	for F in `ls *.msvc.introspect`; do \
		case $$F in \
			*)	cat $(top_builddir)/win32/$$F >>$@ \
			;; \
		esac; \
	done
	$(RM) Atk-1.0.gir.msvc.introspect

DISTCLEANFILES = $(GENERATED_ITEMS)

else
GENERATED_ITEMS =
endif

SUBDIRS =	\
	vs9	\
	vs10	\
	vs11	\
	vs12	\
	vs14	\
	vs15

EXTRA_DIST =	\
	detectenv-msvc.mak	\
	introspection-msvc.mak	\
	atk-introspection-msvc.mak	\
	atkpc.py	\
	pc_base.py	\
	replace.py	\
	$(GENERATED_ITEMS)
