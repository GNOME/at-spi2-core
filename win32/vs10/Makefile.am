GENERATED_ITEMS = \
	atk.vcxproj	\
	atk.vcxproj.filters	\
	atk-install.props

EXTRA_DIST =	\
	README.txt	\
	atk.sln	\
	atk.vcxprojin	\
	atk.vcxproj.filtersin	\
	atk-install.vcxproj	\
	atk-install.vcxproj.filters	\
	atk-build-defines.props	\
	atk-gen-src.props	\
	atk-install.propsin	\
	atk-version-paths.props	\
	$(GENERATED_ITEMS)

atk-install.props: $(top_srcdir)/win32/vs10/atk-install.propsin atk.vs10.headers
	-$(RM) $(top_builddir)/win32/vs11/atk-install.props
	-$(RM) $(top_builddir)/win32/vs12/atk-install.props
	-$(RM) $(top_builddir)/win32/vs14/atk-install.props
	-$(RM) $(top_builddir)/win32/vs15/atk-install.props
	$(CPP) -P - <$(top_srcdir)/win32/vs10/atk-install.propsin >$@
	rm atk.vs10.headers

DISTCLEANFILES = $(GENERATED_ITEMS)
