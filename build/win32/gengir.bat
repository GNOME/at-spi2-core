@echo off

setlocal EnableDelayedExpansion

rem Needed environmental variables:
rem PLAT: Windows platform-Win32 (i.e. x86) or x64 (i.e. x86-64)
rem CONF: Configuration Type, Release or Debug
rem VSVER: Visual C++ version used

rem Check the environemental variables...
if /i "%PLAT%" == "Win32" goto PLAT_OK
if /i "%PLAT%" == "x64" goto PLAT_OK
if /i "%PLAT%" == "x86" (
   set PLAT=Win32
   goto PLAT_OK
)
if /i "%PLAT%" == "x86-64" (
   set PLAT=x64
   goto PLAT_OK
)
goto ERR_PLAT

:PLAT_OK
if "%VSVER%" == "9" goto VSVER_OK
if "%VSVER%" == "10" goto VSVER_OK
if "%VSVER%" == "11" goto VSVER_OK
goto ERR_VSVER
:VSVER_OK
if /i "%CONF%" == "Release" goto CONF_OK
if /i "%CONF%" == "Debug" goto CONF_OK
goto ERR_CONF
:CONF_OK
if "%BASEDIR%" == "" goto ERR_BASEDIR
if not exist %BASEDIR% goto ERR_BASEDIR

if "%PKG_CONFIG_PATH%" == "" goto ERR_PKGCONFIG
if not exist %PKG_CONFIG_PATH%\gobject-2.0.pc goto ERR_PKGCONFIG

if "%MINGWDIR%" == "" goto ERR_MINGWDIR
if not exist %MINGWDIR%\bin\gcc.exe goto ERR_MINGWDIR

set CC=cl
set BINDIR=%CD%\vs%VSVER%\%CONF%\%PLAT%\bin
set INCLUDE=%BASEDIR%\include\glib-2.0;%BASEDIR%\lib\glib-2.0\include;%INCLUDE%
set LIB=%BINDIR%;%BASEDIR%\lib;%LIB%
set PATH=%BINDIR%;%BASEDIR%\bin;%PATH%;%MINGWDIR%\bin
set PYTHONPATH=%BASEDIR%\lib\gobject-introspection;%BINDIR%

echo Setup .bat for generating ATK .gir's...

rem =================================================================================================
rem Begin setup of atk_gir.bat to create Atk-1.0.gir
rem (The ^^ is necessary to span the command to multiple lines on Windows cmd.exe!)
rem =================================================================================================

echo echo Generating Atk-1.0.gir...> atk_gir.bat
echo @echo on>> atk_gir.bat
echo.>> atk_gir.bat
rem ================================================================
rem Setup the command line flags to g-ir-scanner for Atk-1.0.gir...
rem ================================================================
echo python %BASEDIR%\bin\g-ir-scanner --verbose -I..\.. --add-include-path=..\.. ^^>> atk_gir.bat
echo --namespace=Atk --nsversion=1.0 --include=GObject-2.0 ^^>> atk_gir.bat
echo --no-libtool --pkg=glib-2.0 --library=atk-1-vs%VSVER% ^^>> atk_gir.bat
echo --reparse-validate --add-include-path=%BASEDIR%\share\gir-1.0 ^^>> atk_gir.bat
echo  --pkg-export atk --warn-all --c-include "atk/atk.h" ^^>> atk_gir.bat
echo -I..\.. -DATK_DISABLE_DEPRECATED -DATK_COMPILATION -DATK_LOCALEDIR=\"/dummy/share/locale\" ^^>> atk_gir.bat
echo --filelist=atk_list ^^>> atk_gir.bat
echo -o Atk-1.0.gir>> atk_gir.bat
echo.>> atk_gir.bat

echo Completed setup of .bat for generating Atk-1.0.gir.
echo.>> atk_gir.bat

rem ===================================================
rem Finish setup of atk_gir.bat to create Atk-1.0.gir
rem ===================================================

rem =======================
rem Now generate the .gir's
rem =======================
CALL atk_gir.bat

rem Clean up the .bat for generating the .gir files...
del atk_gir.bat

rem Now compile the generated .gir files
%BASEDIR%\bin\g-ir-compiler --includedir=. --debug --verbose Atk-1.0.gir -o Atk-1.0.typelib

rem Copy the generated .girs and .typelibs to their appropriate places

mkdir ..\..\build\win32\vs%VSVER%\%CONF%\%PLAT%\share\gir-1.0
move /y *.gir %BASEDIR%\share\gir-1.0\

mkdir ..\..\build\win32\vs%VSVER%\%CONF%\%PLAT%\lib\girepository-1.0
move /y *.typelib %BASEDIR%\lib\girepository-1.0\

goto DONE

:ERR_PLAT
echo You need to specify a valid Platform [set PLAT=Win32 or PLAT=x64]
goto DONE
:ERR_VSVER
echo You need to specify your Visual Studio version [set VSVER=9 or VSVER=10 or VSVER=11]
goto DONE
:ERR_CONF
echo You need to specify a valid Configuration [set CONF=Release or CONF=Debug]
goto DONE
:ERR_BASEDIR
echo You need to specify a valid BASEDIR.
goto DONE
:ERR_PKGCONFIG
echo You need to specify a valid PKG_CONFIG_PATH
goto DONE
:ERR_MINGWDIR
echo You need to specify a valid MINGWDIR, where a valid gcc installation can be found.
goto DONE
:DONE

