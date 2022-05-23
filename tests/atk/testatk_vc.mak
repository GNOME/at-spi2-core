!include ..\build\detectenv-msvc.mak

LD_CFLAGS = /link
EXEEXT = .exe
GLIB_LIBS = gobject-2.0.lib gmodule-2.0.lib glib-2.0.lib

ATK_API_VERSION = 1.0

BUILD_PATH = ..\build\win32\vs$(VSVER)\$(CFG)\$(PLAT)\bin
LDFLAGS_PATH = /libpath:$(BUILD_PATH) /libpath:..\..\vs$(VSVER)\$(PLAT)\lib

TEST_ATK_LIBS = atk-$(ATK_API_VERSION).lib $(GLIB_LIBS)

LDFLAGS =	\
	$(LDFLAGS_PATH)	\
	$(LDFLAGS_ARCH)

CFLAGS =	\
	$(CFLAGS_ADD) /DG_DISABLE_DEPRECATED /I..	\
	/I..\..\vs$(VSVER)\$(PLAT)\include\glib-2.0	\
	/I..\..\vs$(VSVER)\$(PLAT)\lib\glib-2.0\include	\
	/I..\..\vs$(VSVER)\$(PLAT)\include\glib-2.0

EMPTY_ITEM =

test_programs = \
	testdocument$(EXEEXT)	\
	testrole$(EXEEXT)	\
	testrelation$(EXEEXT)	\
	teststateset$(EXEEXT)	\
	testvalue$(EXEEXT)	\
	$(EMPTY_ITEM)

!if "$(VALID_CFGSET)" == "FALSE"
all:
	!@-echo You need to run "nmake -f testatk_vc.mak CFG=release" or
	!@-echo "nmake -f testatk_vc.mak CFG=debug" to use this Makefile to
	!@-echo build the test programs.

clean:
	@-del /q/f *$(EXEEXT).manifest
	@-del /q/f *$(EXEEXT)
	@-del /q/f *.idb
	@-del /q/f *.obj
	@-del /q/f *.pdb
!else
all: $(test_programs)

.c$(EXEEXT):
	$(CC) $(CFLAGS) $< $(LD_CFLAGS) $(LDFLAGS) $(TEST_ATK_LIBS) /Fe$@
	@-if exist $@.manifest mt /nologo /manifest $@.manifest /outputresource:$@;1

clean:
	@-del /q/f *$(EXEEXT).manifest
	@-del /q/f *$(EXEEXT)
	@-del /q/f *.idb
	@-del /q/f *.obj
	@-del /q/f *.pdb
!endif
