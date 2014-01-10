EXTRA_DIST =	\
	README.txt	\
	atk.sln	\
	atk.vcproj	\
	atk.vcprojin	\
	install.vcproj	\
	atk-build-defines.vsprops	\
	atk-gen-src.vsprops	\
	atk-install.vsprops	\
	atk-install.vspropsin	\
	atk-version-paths.vsprops

atk-install.vsprops: atk.vcproj

atk.vcproj:
	$(PYTHON) $(top_srcdir)/build/atk_msvc_files.py -t vs9

DISTCLEANFILES = atk.vcproj atk-install.vsprops
