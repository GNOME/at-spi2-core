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
	atk-build-defines.props	\
	atk-gen-src.props	\
	atk-install.propsin	\
	atk-version-paths.props	\
	$(GENERATED_ITEMS)

atk-install.props: $(top_srcdir)/build/win32/vs10/atk-install.propsin atk.vs10.headers
	$(CPP) -P - <$(top_srcdir)/build/win32/vs10/atk-install.propsin >$@
	rm atk.vs10.headers

DISTCLEANFILES = $(GENERATED_ITEMS)
