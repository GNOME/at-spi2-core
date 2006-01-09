/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <glib-object.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <cspi/spi.h>
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

static SPIBoolean report_keypad_key_event  (const AccessibleKeystroke *stroke, void *user_data);

static short *get_keypad_keycodes (void);

static AccessibleKeystrokeListener *keypad_key_listener;
static AccessibleKeySet            *keypad_keyset;

static Display *get_display (void);

int
main (int argc, char **argv)
{
  SPIBoolean retval = FALSE;
	
  SPI_init ();

  gdk_init (&argc, &argv); /* 
		* needed in this test program only  
		* because we are calling GDK_DISPLAY() 
		*/

  /* prepare the keyboard snoopers */
  keypad_key_listener = SPI_createAccessibleKeystrokeListener (report_keypad_key_event, NULL);
  keypad_keyset = SPI_createAccessibleKeySet (16, NULL, get_keypad_keycodes (), NULL);

  retval = SPI_registerAccessibleKeystrokeListener(keypad_key_listener,
			       		  keypad_keyset,
					  SPI_KEYMASK_UNMODIFIED,
					  (unsigned long) ( SPI_KEY_PRESSED |
							    SPI_KEY_RELEASED ),
					  SPI_KEYLISTENER_ALL_WINDOWS);
  fprintf (stderr, "Keypad key registry: result %s\n", retval ? "succeeded" : 
	"failed");

  retval = SPI_registerAccessibleKeystrokeListener(keypad_key_listener,
			       		  keypad_keyset,
					  SPI_KEYMASK_SHIFT,
					  (unsigned long) ( SPI_KEY_PRESSED |
							    SPI_KEY_RELEASED ),
					  SPI_KEYLISTENER_ALL_WINDOWS);

  fprintf (stderr, "Shift-Keypad key registry: result %s\n", retval ? "succeeded" : 
	"failed");

  retval = SPI_registerAccessibleKeystrokeListener(keypad_key_listener,
			       		  keypad_keyset,
					  SPI_KEYMASK_MOD2,
					  (unsigned long) ( SPI_KEY_PRESSED |
							    SPI_KEY_RELEASED ),
					  SPI_KEYLISTENER_ALL_WINDOWS);

  fprintf (stderr, "Mod2-Keypad key registry: result %s\n", retval ? "succeeded" : 
	"failed");

  retval = SPI_registerAccessibleKeystrokeListener(keypad_key_listener,
			       		  keypad_keyset,
					  SPI_KEYMASK_MOD3,
					  (unsigned long) ( SPI_KEY_PRESSED |
							    SPI_KEY_RELEASED ),
					  SPI_KEYLISTENER_ALL_WINDOWS);

  fprintf (stderr, "Mod3-Keypad key registry: result %s\n", retval ? "succeeded" : 
	"failed");

  retval = SPI_registerAccessibleKeystrokeListener(keypad_key_listener,
			       		  keypad_keyset,
					  SPI_KEYMASK_MOD2 | SPI_KEYMASK_SHIFT,
					  (unsigned long) ( SPI_KEY_PRESSED |
							    SPI_KEY_RELEASED ),
					  SPI_KEYLISTENER_ALL_WINDOWS);

  fprintf (stderr, "Mod2-Shift-Keypad key registry: result %s\n", retval ? "succeeded" : 
	"failed");

  retval = SPI_registerAccessibleKeystrokeListener(keypad_key_listener,
			       		  keypad_keyset,
					  SPI_KEYMASK_MOD3 | SPI_KEYMASK_SHIFT,
					  (unsigned long) ( SPI_KEY_PRESSED |
							    SPI_KEY_RELEASED ),
					  SPI_KEYLISTENER_ALL_WINDOWS);

  fprintf (stderr, "Mod3-Shift-Keypad key registry: result %s\n", retval ? "succeeded" : 
	"failed");

  SPI_event_main ();

  putenv ("AT_BRIDGE_SHUTDOWN=1");

  return SPI_exit ();
}

static Display*
get_display (void)
{
	return GDK_DISPLAY ();
}

static short*
get_keypad_keycodes (void)
{
	short *keycodes = (short *) g_new0 (short *, 16);
	int i;

	keycodes [0] = XKeysymToKeycode (get_display (), XK_KP_0);
	keycodes [1] = XKeysymToKeycode (get_display (), XK_KP_1);
	keycodes [2] = XKeysymToKeycode (get_display (), XK_KP_2);
	keycodes [3] = XKeysymToKeycode (get_display (), XK_KP_3);
	keycodes [4] = XKeysymToKeycode (get_display (), XK_KP_4);
	keycodes [5] = XKeysymToKeycode (get_display (), XK_KP_5);
	keycodes [6] = XKeysymToKeycode (get_display (), XK_KP_6);
	keycodes [7] = XKeysymToKeycode (get_display (), XK_KP_7);
	keycodes [8] = XKeysymToKeycode (get_display (), XK_KP_8);
	keycodes [9] = XKeysymToKeycode (get_display (), XK_KP_9);
	keycodes [10] = XKeysymToKeycode (get_display (), XK_KP_Divide);
	keycodes [11] = XKeysymToKeycode (get_display (), XK_KP_Multiply);
	keycodes [12] = XKeysymToKeycode (get_display (), XK_KP_Add);
	keycodes [13] = XKeysymToKeycode (get_display (), XK_KP_Subtract);
	keycodes [14] = XKeysymToKeycode (get_display (), XK_KP_Decimal);
	keycodes [15] = XKeysymToKeycode (get_display (), XK_KP_Enter);

	for (i = 0; i < 16; ++i)
	{
		fprintf (stderr, "keycode[%d] = %d\n", i, (int) keycodes[i]);
	}
	return keycodes;
}

static void
print_key_event (const AccessibleKeystroke *key, char *prefix)
{
  fprintf (stderr, "%s KeyEvent %s%c [keysym 0x%ld] (keycode %d); string=%s; time=%lx\n",
	   prefix,
	  (key->modifiers & SPI_KEYMASK_ALT)?"Alt-":"",
	  ((key->modifiers & SPI_KEYMASK_SHIFT)^(key->modifiers & SPI_KEYMASK_SHIFTLOCK))?
	  (char) toupper((int) key->keyID) : (char) tolower((int) key->keyID),
	   key->keyID,
	  (int) key->keycode,
   	  key->keystring,
	  (long int) key->timestamp);	
}

static SPIBoolean
report_keypad_key_event (const AccessibleKeystroke *key, void *user_data)
{
  print_key_event (key, "keypad");
  return FALSE;
}


