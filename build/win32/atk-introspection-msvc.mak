# NMake Makefile to build Introspection Files for ATK

!include detectenv-msvc.mak

APIVERSION = 1.0

CHECK_PACKAGE = gobject-2.0

!include introspection-msvc.mak

!if "$(BUILD_INTROSPECTION)" == "TRUE"
all: setgirbuildenv Atk-$(APIVERSION).gir Atk-$(APIVERSION).typelib

setgirbuildenv:
	@-set PYTHONPATH=$(PREFIX)\lib\gobject-introspection
	@-set PATH=vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(PREFIX)\bin;$(PATH)
	@-set PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)
	@-set LIB=vs$(VSVER)\$(CFG)\$(PLAT)\bin;$(LIB)

!include introspection.body.mak

install-introspection: all
	@-copy Atk-$(APIVERSION).gir $(G_IR_INCLUDEDIR)
	@-copy /b Atk-$(APIVERSION).typelib $(G_IR_TYPELIBDIR)

!else
all:
	@-echo $(ERROR_MSG)
!endif

clean:
	@-del /f/q Atk-$(APIVERSION).typelib
	@-del /f/q Atk-$(APIVERSION).gir
