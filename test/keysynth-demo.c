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
#define SHIFT_L_KEYCODE 50
#define SHIFT_R_KEYCODE 62

/* these can be increased to access more keys */
#define MAX_ROWS 5
#define MAX_COLUMNS 14

static KeystrokeListener *key_listener;
static KeystrokeListener *switch_listener;

static boolean shift_latched = False;
static boolean caps_lock = False;
static GtkButton **buttons[MAX_ROWS];

static void
label_buttons(boolean shifted)
{
  int i, j;
  KeySym keysym;
  KeyCode keycode = MIN_KEYCODE;
  char label[LABELMAXLEN] = " ";
  char *button_label;
  char *keysymstring;
  
  for (i=0; i<MAX_ROWS; ++i)
    {
      for (j=0; j<MAX_COLUMNS; ++j, ++keycode)
        {
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
show_shift (GtkButton *button, boolean *is_shifted)
{
 label_buttons (*is_shifted ^ caps_lock);	
}

static void
do_shift (GtkButton *button)
{
  static KeyCode shift_keycode = 0;
  if (!shift_keycode) shift_keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) 0xFFE1);
  /* Note: in a real onscreen keyboard shift keycode should not be hard-coded! */
  shift_latched = !shift_latched;
  generateKeyEvent (shift_keycode, shift_latched ? KEY_PRESS : KEY_RELEASE);
  if (buttons) label_buttons (caps_lock || shift_latched);
}

static void
keysynth_exit()
{
  if (shift_latched) do_shift (NULL);
  deregisterKeystrokeListener (key_listener, KEYMASK_ALT );
  deregisterKeystrokeListener (switch_listener, KEYMASK_UNMODIFIED );
  SPI_exit ();
}

static void
keysynth_realize (GtkWidget *widget)
{
  XWMHints wm_hints;
  Atom wm_window_protocols[2];
  static gboolean initialized = FALSE;
  
  if (!initialized)
    {
      wm_window_protocols[0] = gdk_x11_get_xatom_by_name ("WM_DELETE_WINDOW");
      wm_window_protocols[1] = gdk_x11_get_xatom_by_name ("_NET_WM_PING");
    }
  
  wm_hints.flags = InputHint;
  wm_hints.input = False;
  
  XSetWMHints (GDK_WINDOW_XDISPLAY (widget->window),
	       GDK_WINDOW_XWINDOW (widget->window), &wm_hints);
  
  XSetWMProtocols (GDK_WINDOW_XDISPLAY (widget->window),
		   GDK_WINDOW_XWINDOW (widget->window), wm_window_protocols, 2);
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

static boolean
switch_callback (void *p)
{
  return FALSE;
}

static void
synth_keycode (GtkButton *button, KeyCode *keycode)
{
  if (shift_latched) do_shift (button);
  if (*keycode)
    {
      if (*keycode == CAPSLOCK_KEYCODE)
        {
          caps_lock = !caps_lock;	     
          label_buttons (caps_lock || shift_latched);
        }
      generateKeyEvent ((long) *keycode, KEY_PRESSRELEASE);
    }      
}

static void
create_vkbd()
{
  GtkWidget *window, *button, *container, *hbox;
  int i, j;
  KeyCode *keycodeptr, keycode = MIN_KEYCODE;
  static boolean true_val = True;
  static boolean false_val = False;

  window = g_object_connect (gtk_widget_new (gtk_window_get_type (),
					     "user_data", NULL,
					     "can_focus", FALSE,
					     "type", GTK_WINDOW_TOPLEVEL,
					     "window-position", GTK_WIN_POS_CENTER,
					     "title", "test",
					     "allow_grow", FALSE,
					     "allow_shrink", FALSE,
					     "border_width", 10,
					     NULL),
			     "signal::realize", keysynth_realize, NULL,
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
	  buttons[i][j] = g_object_connect (gtk_widget_new (gtk_button_get_type (),
						     "GtkWidget::parent", hbox,
						     "GtkWidget::visible", TRUE,
						     NULL),
				     "signal::clicked",
				     synth_keycode, keycodeptr,
				     NULL);
	  if (keycode == SHIFT_L_KEYCODE || keycode == SHIFT_R_KEYCODE)
	    {
	      g_object_connect (buttons[i][j], "signal::pressed", show_shift,
				&true_val, NULL);  
	      g_object_connect (buttons[i][j], "signal::released", show_shift,
				&false_val, NULL);  
	    }
	  ++keycode;
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
  label_buttons (caps_lock || shift_latched);
  gtk_widget_show (window);
}

int
main(int argc, char **argv)
{
  KeySet spacebar_set;
  
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
			    KEYSPI_LISTENER_CANCONSUME | KEYSPI_LISTENER_ALLWINDOWS);
  create_vkbd();  

  /* register a listener on the spacebar, to serve as a 'single switch' */
  spacebar_set.keysyms = g_new0 (unsigned long, 1);
  spacebar_set.keycodes = g_new0 (unsigned short, 1);
  spacebar_set.len = 1;
  spacebar_set.keysyms[0] = (unsigned long) ' ';
  spacebar_set.keycodes[0] = (unsigned short) 0;
  switch_listener = createKeystrokeListener(switch_callback);
  /* registerKeystrokeListener(switch_listener,
			    &spacebar_set,
			    KEYMASK_UNMODIFIED,
			    (unsigned long) ( KeyPress | KeyRelease),
			    KEYSPI_LISTENER_CANCONSUME);
  */
  
  SPI_event_main(TRUE);
}
