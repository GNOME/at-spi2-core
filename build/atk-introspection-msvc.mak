# NMake Makefile to build Introspection Files for ATK

!include testsrules_msvc.mak

APIVERSION = 1.0

CHECK_PACKAGE = gobject-2.0

!include introspection-msvc.mak

!if "$(BUILD_INTROSPECTION)" == "TRUE"
all: Atk-$(APIVERSION).gir Atk-$(APIVERSION).typelib

atk_list:
	@-echo Generating Filelist to Introspect for ATK...
	$(PYTHON2) gen-file-list-atk.py

Atk-$(APIVERSION).gir: atk_list
	@-echo Generating Atk-$(APIVERSION).gir...
	@set CC=$(CC)
	@set PYTHONPATH=$(BASEDIR)\lib\gobject-introspection
	@set PATH=win32\vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(BASEDIR)\bin;$(PATH);$(MINGWDIR)\bin
	@set PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)
	@set LIB=win32\vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(BASEDIR)\lib;$(LIB)
	$(PYTHON2) $(G_IR_SCANNER) --verbose -I.. --add-include-path=.. \
	--namespace=Atk --nsversion=$(APIVERSION) --include=GObject-2.0	\
	--no-libtool --pkg=glib-2.0 --library=atk-1.0	\
	--reparse-validate --add-include-path=$(G_IR_INCLUDEDIR)	\
	--pkg-export atk --warn-all --c-include "atk/atk.h"	\
	-DATK_DISABLE_DEPRECATED -DATK_COMPILATION -DATK_LOCALEDIR=\"/dummy/share/locale\"	\
	--filelist=atk_list -o Atk-$(APIVERSION).gir

Atk-$(APIVERSION).typelib: Atk-$(APIVERSION).gir
	@-echo Compiling Atk-$(APIVERSION).typelib...
	$(G_IR_COMPILER) --includedir=. --debug --verbose Atk-1.0.gir -o Atk-1.0.typelib

install-introspection: Atk-$(APIVERSION).gir Atk-$(APIVERSION).typelib
	@-copy Atk-$(APIVERSION).gir $(G_IR_INCLUDEDIR)
	@-copy /b Atk-$(APIVERSION).typelib $(G_IR_TYPELIBDIR)

!else
all:
	@-echo $(ERROR_MSG)
!endif

clean:
	@-del /f/q Atk-$(APIVERSION).typelib
	@-del /f/q Atk-$(APIVERSION).gir
	@-del /f/q atk_list
	@-del /f/q *.pyc
