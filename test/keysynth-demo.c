/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include "spi.h"

#define LABELMAXLEN 20
#define MIN_KEYCODE 9
#define CAPSLOCK_KEYCODE 66

/* these can be increased to access more keys */
#define MAX_ROWS 5
#define MAX_COLUMNS 14

static KeystrokeListener *key_listener;

static boolean shift_latched = False;
static boolean caps_lock = False;
static GtkButton **buttons[MAX_ROWS];

static void
label_buttons()
{
  int i, j;
  KeySym keysym;
  KeyCode keycode = MIN_KEYCODE;
  char label[LABELMAXLEN] = " ";
  char *button_label;
  char *keysymstring;
  boolean shifted;
  
  for (i=0; i<MAX_ROWS; ++i)
    {
      for (j=0; j<MAX_COLUMNS; ++j, ++keycode)
        {
	  shifted = caps_lock || shift_latched;
	  keysym = (KeySym) XKeycodeToKeysym (GDK_DISPLAY(), keycode, shifted ? 1 : 0);
          /* Note: these routines are not i18n-savvy,  we need to use XIM, other methods here */
	  if (keysym && g_ascii_isprint((int)keysym))
	    {
	      snprintf (label, 2, "%c", (int) keysym); 
	    }
	  else
	    {
	      keysymstring = XKeysymToString (keysym);
	      if (keysymstring)
	        { 
		  /* KP_ means keypad... we won't expose this difference */
		  if (!strncmp (keysymstring, "KP_", 3))
		       strncpy (label, (char *)(keysymstring+3), LABELMAXLEN);
	          else strncpy (label, keysymstring, LABELMAXLEN);
  	        }
	      else *label = 0;
	    }
	  button_label = 	
	  *label==' ' ? "   space   " : label;
	  gtk_button_set_label (buttons[i][j], button_label);	 
        }
    }
}

static void
do_shift (GtkButton *button)
{
  static KeyCode shift_keycode = 0;
  if (!shift_keycode) shift_keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) 0xFFE1);
  /* Note: in a real onscreen keyboard shift keycode should not be hard-coded! */
  shift_latched = !shift_latched;
  generateKeyEvent (shift_keycode, shift_latched ? KEY_PRESS : KEY_RELEASE);
  if (buttons) label_buttons (buttons);
}

static void
keysynth_exit()
{
  if (shift_latched) do_shift (NULL);
  deregisterKeystrokeListener (key_listener, KEYMASK_ALT );
  SPI_exit ();
}

static void
button_exit(GtkButton *notused, void *alsonotused)
{
  keysynth_exit();
}

static boolean
is_command_key (void *p)
{
  KeyStroke *key = (KeyStroke *)p;
  switch (key->keyID)
    {
    case 'Q':
    case 'q':
	    keysynth_exit(); 
	    return TRUE; /* not reached */
    }
}

static void
synth_keycode (GtkButton *button, KeyCode *keycode)
{
  if (*keycode)  generateKeyEvent ((long) *keycode, KEY_PRESSRELEASE);
  if (shift_latched) do_shift (button);
  if (*keycode == CAPSLOCK_KEYCODE)
    {
      caps_lock = !caps_lock;	    
      label_buttons ();
    }
}

static void
create_vkbd()
{
  GtkWidget *window, *button, *container, *hbox;
  int i, j;
  KeyCode *keycodeptr, keycode = MIN_KEYCODE;

  window = g_object_connect (gtk_widget_new (gtk_window_get_type (),
					     "user_data", NULL,
					     "can_focus", FALSE,
					     "type", GTK_WINDOW_POPUP,
					     "window-position", GTK_WIN_POS_CENTER,
					     "title", "test",
					     "allow_grow", FALSE,
					     "allow_shrink", FALSE,
					     "border_width", 10,
					     NULL),
			     "signal::destroy", keysynth_exit, NULL,
			     NULL);
  
  container = gtk_widget_new (GTK_TYPE_VBOX,
			      "GtkWidget::parent", window,
			      "GtkWidget::visible", TRUE,
			      NULL);
  for (i=0; i<MAX_ROWS; ++i)
    {
      hbox = gtk_widget_new (gtk_hbox_get_type(),
			     "GtkWidget::parent", container,
			     "GtkWidget::visible", TRUE,
			     NULL);
      buttons[i] = g_new0 (GtkButton*, MAX_COLUMNS);
      for (j=0; j<MAX_COLUMNS; ++j)
	{
	  keycodeptr = (KeyCode *) g_new0 (KeyCode, 1);
	  *keycodeptr = keycode;
	  ++keycode;
	  buttons[i][j] = g_object_connect (gtk_widget_new (gtk_button_get_type (),
						     "GtkWidget::parent", hbox,
						     "GtkWidget::visible", TRUE,
						     NULL),
				     "signal::clicked",
				     synth_keycode, keycodeptr,
				     NULL);
	}
    } 
  hbox = gtk_widget_new (gtk_hbox_get_type(),
			 "GtkWidget::parent", container,
			 "GtkWidget::visible", TRUE,
			 NULL);
  button = g_object_connect (gtk_widget_new (gtk_button_get_type (),
					     "GtkButton::label", "Quit",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     button_exit, NULL,
			     NULL);
  button = g_object_connect (gtk_widget_new (gtk_button_get_type (),
					     "GtkButton::label", "ShiftLatch",
					     "GtkWidget::parent", hbox,
					     "GtkWidget::visible", TRUE,
					     NULL),
			     "signal::clicked",
			     do_shift, NULL,
			     NULL);
  label_buttons ();
  gtk_widget_show (window);

}

int
main(int argc, char **argv)
{
  if ((argc > 1) && (!strncmp(argv[1],"-h",2)))
  {
    printf ("Usage: keysynth-demo\n");
    exit(0);
  }

  gtk_init (&argc, &argv); /* must call, because this program uses GTK+ */

  SPI_init();

  key_listener = createKeystrokeListener(is_command_key);
  /* will listen only to Alt-key combinations */
  registerKeystrokeListener(key_listener,
			    (KeySet *) ALL_KEYS,
			    KEYMASK_ALT,
			    (unsigned long) ( KeyPress | KeyRelease),
			    KEYLISTENER_CANCONSUME);
  create_vkbd();  

  SPI_event_main(TRUE);
}
