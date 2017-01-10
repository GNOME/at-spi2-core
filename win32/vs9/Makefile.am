GENERATED_ITEMS = \
	atk.vcproj	\
	atk-install.vsprops

EXTRA_DIST =	\
	README.txt	\
	atk.sln	\
	atk.vcprojin	\
	atk-install.vcproj	\
	atk-build-defines.vsprops	\
	atk-gen-src.vsprops	\
	atk-install.vspropsin	\
	atk-version-paths.vsprops	\
	$(GENERATED_ITEMS)

atk-install.vsprops: $(top_srcdir)/win32/vs9/atk-install.vspropsin atk.headers
	$(CPP) -P - <$(top_srcdir)/win32/vs9/atk-install.vspropsin >$@
	rm atk.headers

DISTCLEANFILES = $(GENERATED_ITEMS)
