#!/bin/csh
echo "This demo requires the festival synthesis system, and modules"
echo "'gail'.  It also assumes (for the moment) that the gnome-2.0"
echo "libraries are in /opt/gnome-2.0/lib, gnome-2.0 executables are in"
echo "/opt/gnome-2.0/bin, and that festival is installed in"
echo "/usr/share/festival/festival."
echo "***WARNING*** this demo calls bonobo-slay and pkill, so it" 
echo "may kill a number of existing processes/applications on your system."
echo "you are advised to read and edit the file before use."
setenv PATH /opt/gnome-2.0/bin:/bin:/usr/bin:$HOME/gnome/at-spi/test/.libs:/usr/share/festival/festival/bin
setenv LD_LIBRARY_PATH /opt/gnome-2.0/lib
bonobo-slay
pkill festival_server
pkill festival
pkill sleep
pkill magnifier
pkill simple-at
pkill keysynth-demo
unsetenv GTK_MODULES
setenv MAGNIFIER 1
setenv FESTIVAL 1
setenv GTK_RC_FILES /opt/gnome-2.0/share/themes/Default/gtk-2.0/gtkrc
# start the text-to-speech service
festival_server &

# start the simple AT client, which uses the
#  MAGNIFIER and FESTIVAL environment variables
simple-at &
sleep 2; keysynth-demo &
# now set GTK_MODULES for use by GTK+ applications
# this will cause the gail accessibility support, 
# the ferret test tool, and the atk-bridge to be
# loaded

setenv GTK_MODULES "gail:ferret:atk-bridge"

gtk-demo

# these might be out of place, if gtk-demo backgrounds
# itself

pkill festival_server
pkill festival
pkill sleep
pkill magnifier
pkill simple-at

