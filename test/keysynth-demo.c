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

#include <stdio.h>
#include <string.h>
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
#define MAX_ROWS 6 /* The last row only holds Quit and ShiftLatch, special-purpose keys */
#define MAX_COLUMNS 14

static AccessibleKeystrokeListener *key_listener;
static AccessibleKeystrokeListener *switch_listener;

static SPIBoolean shift_latched = False;
static SPIBoolean caps_lock = False;
static GtkButton **buttons[MAX_ROWS];

typedef enum {
	SCAN_IDLE,
	SCAN_LINES,
	SCAN_LINES_DONE,
	SCAN_KEYS,
	SCAN_KEYS_DONE
} ScanTimerState;

typedef struct {
	ScanTimerState timer_state;
	gint scan_row;
	gint scan_column;
} ScanState;

GdkColor *
button_default_bg_color (GtkButton *button)
{
  static GdkColor bg_color;
  static gboolean initialized = FALSE;
  if (!initialized)
  {
    bg_color = gtk_widget_get_style (GTK_WIDGET (button))->bg[GTK_STATE_NORMAL];
    initialized = TRUE;
  }
  return &bg_color;
}

GdkColor *
button_default_selected_color (GtkButton *button)
{
  static GdkColor selected_color;
  static gboolean initialized = FALSE;
  if (!initialized)
  {
    selected_color = gtk_widget_get_style (GTK_WIDGET (button))->bg[GTK_STATE_SELECTED];
    initialized = TRUE;
  }
  return &selected_color;
}

void
select_key (gint lineno, gint keyno)
{
  /*
   * Before we do this, we need to make sure we've saved the default normal bg.
   * The button_default_bg_color() call caches this for us (as a side-effect).
   * Probably we should do this a cleaner way...
   */
  button_default_bg_color (buttons [lineno][keyno]);	
  gtk_widget_modify_bg (GTK_WIDGET (buttons [lineno][keyno]),
			GTK_STATE_NORMAL,
			button_default_selected_color (buttons [lineno][keyno]));
}

void
deselect_key (gint lineno, gint keyno)
{
  gtk_widget_modify_bg (GTK_WIDGET (buttons [lineno][keyno]),
			GTK_STATE_NORMAL,
			button_default_bg_color (buttons [lineno][keyno]));
}

void
deselect_line (gint lineno)
{
  int i;
  int max_columns = MAX_COLUMNS;
  if (lineno == MAX_ROWS-1) max_columns = 2;
  for (i=0; i<max_columns; ++i)
	  deselect_key (lineno, i);
}

void
select_line (gint lineno)
{
  int i;
  int max_columns = MAX_COLUMNS;
  if (lineno == MAX_ROWS-1) max_columns = 2;
  for (i=0; i<max_columns; ++i)
	  select_key (lineno, i);
}


static ScanState*
scan_state ()
{
  static ScanState state = {SCAN_IDLE, 0, 0};
  return &state;
}

static gboolean
timeout_scan (gpointer data)
{
  ScanState *state = (ScanState *) data;
  state->timer_state = SCAN_IDLE;
  deselect_key (state->scan_row, state->scan_column);
  return FALSE;
}

static gboolean
increment_scan (gpointer data)
{
  ScanState *state = (ScanState *) data;
  int max_columns;
  switch (state->timer_state)
    {  
      case SCAN_IDLE: 
/* happens if switch-break occurs before the timer fires, after SCAN_KEYS_DONE*/
          return FALSE;
      case SCAN_LINES:
          deselect_line (state->scan_row);
          state->scan_row = (++state->scan_row < MAX_ROWS) ? state->scan_row : 0;
          select_line (state->scan_row);
          g_print ("line %d\n", state->scan_row);
	  break;
      case SCAN_KEYS:
          deselect_key (state->scan_row, state->scan_column);
	  if (state->scan_row == MAX_ROWS-1) max_columns = 2;
	  else max_columns = MAX_COLUMNS; /* last row has only two keys */
          state->scan_column = (++state->scan_column < max_columns) ? state->scan_column : 0;
          select_key (state->scan_row, state->scan_column);
          g_print ("row %d\n", state->scan_column);
	  break;
      case SCAN_LINES_DONE:
      case SCAN_KEYS_DONE:
	  return FALSE;
      default:
    }
  return TRUE;
}

static void
scan_start (unsigned int timestamp)
{
  ScanState *state = scan_state();
  switch (state->timer_state)
    {
    case SCAN_IDLE:
      state->timer_state = SCAN_LINES;
      state->scan_column = 0;
      state->scan_row = 0;
      g_timeout_add_full (G_PRIORITY_HIGH_IDLE, 600, increment_scan, state, NULL);
      select_line (state->scan_row);
      break;
    case SCAN_LINES_DONE:
      state->timer_state = SCAN_KEYS;
      g_timeout_add_full (G_PRIORITY_HIGH_IDLE, 600, increment_scan, state, NULL);
      deselect_line (state->scan_row);
      select_key (state->scan_row, state->scan_column);
      break;
    case SCAN_KEYS_DONE:
      gtk_button_clicked (buttons[state->scan_row][state->scan_column]);
      deselect_key (state->scan_row, state->scan_column);
      state->timer_state = SCAN_IDLE;
      break;
    default:
      g_print("unexpected state for 'scan start'\n");
    }
}

static void
scan_stop (unsigned int timestamp)
{
  /* find the element correspondin to this event's timestamp */
  ScanState *state = scan_state();
  switch (state->timer_state)
    {
    case SCAN_LINES:
      state->timer_state = SCAN_LINES_DONE;
      break;
    case SCAN_KEYS:
      state->timer_state = SCAN_KEYS_DONE;
      g_timeout_add_full (G_PRIORITY_HIGH_IDLE, 1200, timeout_scan, state, NULL);
      break;
    case SCAN_IDLE:
      break;
    default:
      g_print("unexpected state for 'scan stop'\n");
    }
}

static void
label_buttons(SPIBoolean shifted)
{
  int i, j;
  KeySym keysym;
  KeyCode keycode = MIN_KEYCODE;
  char label[LABELMAXLEN] = " ";
  char *button_label;
  char *keysymstring;
  
  for (i=0; i<MAX_ROWS-1; ++i) /* last row doesn't change */
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
show_shift (GtkButton *button, SPIBoolean *is_shifted)
{
 label_buttons (*is_shifted ^ caps_lock);	
}

static void
toggle_shift_latch (GtkButton *button) 
{ 
  shift_latched = !shift_latched;
  if (buttons) label_buttons (caps_lock || shift_latched);
}

static void
keysynth_exit (void)
{
  SPI_deregisterAccessibleKeystrokeListener (key_listener, SPI_KEYMASK_ALT);
  AccessibleKeystrokeListener_unref         (key_listener);

  SPI_deregisterAccessibleKeystrokeListener (switch_listener, SPI_KEYMASK_UNMODIFIED);
  AccessibleKeystrokeListener_unref         (switch_listener);

  SPI_event_quit ();
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
button_exit (GtkButton *notused, void *alsonotused)
{
  keysynth_exit ();
}

static SPIBoolean
is_command_key (const AccessibleKeystroke *key, void *user_data)
{
  switch (key->keyID)
    {
    case 'Q':
    case 'q':
	    keysynth_exit (); 
	    return TRUE; /* not reached */
    }
  return FALSE;
}

static SPIBoolean
switch_callback (const AccessibleKeystroke *key, void *user_data)
{
  static SPIBoolean is_down = FALSE;

  if (key->type == SPI_KEY_RELEASED)
    {
      g_print ("switch up\n");
      is_down = FALSE;
      scan_stop (key->timestamp);
    }
  else 
  if (!is_down)
    {
      g_print ("switch down\n");
      is_down = TRUE;
      scan_start (key->timestamp);
    }
  /* catch the first, ignore the rest (will repeat) until keyrelease */
  return FALSE;
}

static void
synth_keycode (GtkButton *button, KeyCode *keycode)
{
  static KeyCode shift_keycode = 0;
  if (!shift_keycode) shift_keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) 0xFFE1);
  /* Note: in a real onscreen keyboard shift keycode should not be hard-coded! */
  
  if (*keycode)
    {
      if (*keycode == CAPSLOCK_KEYCODE)
        {
          caps_lock = !caps_lock;	     
          label_buttons (caps_lock || shift_latched);
        }
      if (shift_latched)
	      SPI_generateKeyboardEvent (shift_keycode, NULL, SPI_KEY_PRESS);

      g_print ("generating key %d\n", (int) *keycode);
      SPI_generateKeyboardEvent ((long) *keycode, NULL, SPI_KEY_PRESSRELEASE);

      if (shift_latched)
        {
	  SPI_generateKeyboardEvent (shift_keycode, NULL, SPI_KEY_RELEASE);
	  toggle_shift_latch (button);
	}
    }
}

static void
create_vkbd()
{
  GtkWidget *window, *container, *hbox;
  int i, j;
  KeyCode *keycodeptr, keycode = MIN_KEYCODE;
  static SPIBoolean true_val = True;
  static SPIBoolean false_val = False;

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
  for (i=0; i<MAX_ROWS-1; ++i)
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
  buttons[i] = g_new0 (GtkButton*, 2);
  buttons[i][0] = g_object_connect (gtk_widget_new (gtk_button_get_type (),
							     "GtkButton::label", "Quit",
							     "GtkWidget::parent", hbox,
							     "GtkWidget::visible", TRUE,
							     NULL),
					     "signal::clicked",
					     button_exit, NULL,
					     NULL);
  buttons[i][1] = g_object_connect (gtk_widget_new (gtk_button_get_type (),
						    "GtkButton::label", "ShiftLatch",
						    "GtkWidget::parent", hbox,
						    "GtkWidget::visible", TRUE,
						    NULL),
				    "signal::clicked",
				    toggle_shift_latch, NULL,
				    NULL);
  label_buttons (caps_lock || shift_latched);
  gtk_widget_show (window);
}

int
main (int argc, char **argv)
{
  AccessibleKeySet switch_set;
  
  if ((argc > 1) && (!strncmp (argv[1], "-h", 2)))
    {
      printf ("Usage: keysynth-demo\n");
      exit (1);
    }

  gtk_init (&argc, &argv); /* must call, because this program uses GTK+ */

  SPI_init ();

  key_listener = SPI_createAccessibleKeystrokeListener (is_command_key, NULL);
  /* will listen only to Alt-key combinations */
  SPI_registerAccessibleKeystrokeListener (key_listener,
					   (AccessibleKeySet *) SPI_KEYSET_ALL_KEYS,
					   SPI_KEYMASK_ALT,
					   (unsigned long) ( KeyPress | KeyRelease),
					   SPI_KEYLISTENER_CANCONSUME | SPI_KEYLISTENER_ALL_WINDOWS);
  create_vkbd ();  

  /*
   * Register a listener on an 'unused' key, to serve as a 'single switch'.
   * On most Intel boxes there is at least one 'special' system key that does not
   * have a non-zero keycode assigned in the Xserver, so we will intercept any keycode
   * that is 'zero'.  Often these the are the "windows" or the "menu" keys.
   */
  switch_set.keysyms = g_new0 (unsigned long, 1);
  switch_set.keycodes = g_new0 (unsigned short, 1);
  switch_set.keystrings = g_new0 (char *, 1);
  switch_set.len = 1;
  switch_set.keysyms[0] = (unsigned long) 0;
  switch_set.keycodes[0] = (unsigned short) 0;
  switch_set.keystrings[0] = "";
  switch_listener = SPI_createAccessibleKeystrokeListener (switch_callback, NULL);
  SPI_registerAccessibleKeystrokeListener (switch_listener,
					   &switch_set,
					   SPI_KEYMASK_UNMODIFIED,
					   (unsigned long) ( SPI_KEY_PRESSED | SPI_KEY_RELEASED ),
					   SPI_KEYLISTENER_NOSYNC);
  
  SPI_event_main ();

  return SPI_exit ();
}
