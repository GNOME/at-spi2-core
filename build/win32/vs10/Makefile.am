EXTRA_DIST =	\
	README.txt	\
	atk.sln	\
	atk.vcxproj	\
	atk.vcxprojin	\
	atk.vcxproj.filters	\
	atk.vcxproj.filtersin	\
	install.vcxproj	\
	atk-build-defines.props	\
	atk-gen-src.props	\
	atk-install.props	\
	atk-install.propsin	\
	atk-version-paths.props

atk-install.props atk.vcxproj.filter: atk.vcxproj

atk.vcxproj:
	$(PYTHON) $(top_srcdir)/build/atk_msvc_files.py -t vs10

DISTCLEANFILES = atk.vcxproj atk.vcxproj.filters atk-install.props
