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
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cspi/spi.h>
#include "../util/mag_client.h"
#include "../cspi/spi-private.h" /* A hack for now */

static void report_focus_event    (AccessibleEvent *event, void *user_data);
static void report_button_press   (AccessibleEvent *event, void *user_data);
static void check_property_change (AccessibleEvent *event, void *user_data);
static SPIBoolean report_command_key_event  (AccessibleKeystroke *stroke, void *user_data);
static SPIBoolean report_ordinary_key_event (AccessibleKeystroke *stroke, void *user_data);
static void get_environment_vars (void);

static int _festival_init ();
static void _festival_say (const char *text, const char *voice, SPIBoolean shutup);
static void _festival_write (const char *buff, int fd);

static SPIBoolean use_magnifier = FALSE;
static SPIBoolean use_festival = FALSE;
static SPIBoolean festival_chatty = FALSE;

static AccessibleEventListener *focus_listener;
static AccessibleEventListener *property_listener;
static AccessibleEventListener *button_listener;
static AccessibleKeystrokeListener *command_key_listener;
static AccessibleKeystrokeListener *ordinary_key_listener;

int
main (int argc, char **argv)
{
  int i, j;
  int n_desktops;
  int n_apps;
  Accessible *desktop;
  Accessible *application;
  char *s;

  if ((argc > 1) && (!strncmp (argv[1], "-h", 2)))
  {
    printf ("Usage: simple-at\n");
    printf ("\tEnvironment variables used:\n\t\tFESTIVAL\n\t\tMAGNIFIER\n\t\tFESTIVAL_CHATTY\n");
    exit (0);
  }

  SPI_init ();

  focus_listener = createAccessibleEventListener (report_focus_event, NULL);
  property_listener = createAccessibleEventListener (check_property_change, NULL); 
  button_listener = createAccessibleEventListener (report_button_press, NULL);
  registerGlobalEventListener (focus_listener, "focus:");
  registerGlobalEventListener (property_listener, "object:property-change:accessible-selection"); 
  registerGlobalEventListener (button_listener, "Gtk:GtkWidget:button-press-event");
  n_desktops = getDesktopCount ();

  for (i=0; i<n_desktops; ++i)
    {
      desktop = getDesktop (i);
      s = Accessible_getName (desktop);
      fprintf (stderr, "desktop %d name: %s\n", i, s);
      SPI_freeString (s);
      n_apps = Accessible_getChildCount (desktop);
      for (j=0; j<n_apps; ++j)
        {
          application = Accessible_getChildAtIndex (desktop, j);
	  s = Accessible_getName (application);
          fprintf (stderr, "app %d name: %s\n", j, s);
          SPI_freeString (s);
          Accessible_unref (application);
        }
      Accessible_unref (desktop);
    }

  /* prepare the keyboard snoopers */
  command_key_listener = createAccessibleKeystrokeListener (report_command_key_event, NULL);
  ordinary_key_listener = createAccessibleKeystrokeListener (report_ordinary_key_event, NULL);
  
  /* will listen only to Alt-key combinations, and only to KeyPress events */
  registerAccessibleKeystrokeListener(command_key_listener,
				      (AccessibleKeySet *) SPI_KEYSET_ALL_KEYS,
				      SPI_KEYMASK_ALT,
				      (unsigned long) ( KeyPress ),
				      SPI_KEYLISTENER_ALL_WINDOWS);
  
  /* will listen only to unshifted key events, both press and release */
  registerAccessibleKeystrokeListener(ordinary_key_listener,
				      (AccessibleKeySet *) SPI_KEYSET_ALL_KEYS,
				      SPI_KEYMASK_UNMODIFIED,
				      (unsigned long) ( KeyPress | KeyRelease),
				      SPI_KEYLISTENER_NOSYNC);
				      
  /* will listen only to shifted key events, both press and release */
  registerAccessibleKeystrokeListener(ordinary_key_listener,
				      (AccessibleKeySet *) SPI_KEYSET_ALL_KEYS,
				      SPI_KEYMASK_SHIFT,
				      (unsigned long) ( KeyPress | KeyRelease),
				      SPI_KEYLISTENER_NOSYNC);

  get_environment_vars ();

  SPI_event_main ();

  setenv ("AT_BRIDGE_SHUTDOWN", "1", TRUE);

  return SPI_exit ();
}

static void
get_environment_vars (void)
{
  if (g_getenv ("FESTIVAL"))
    {
      fprintf (stderr, "Using festival\n");
      use_festival = TRUE;
      if (g_getenv ("FESTIVAL_CHATTY"))
        {
          festival_chatty = TRUE;
	}
    }
  if (g_getenv ("MAGNIFIER"))
    {
      fprintf (stderr, "Using magnifier\n");
      use_magnifier = TRUE;
    }  
}

void
report_focussed_accessible (Accessible *obj, SPIBoolean shutup_previous_speech)
{
  char *s;
  int len;

  g_warning ("Report focused !");

  if (use_festival)
    {
    if (festival_chatty) 	    
      {
        _festival_say (Accessible_getRole (obj), "voice_don_diphone", shutup_previous_speech);
      }
      fprintf (stderr, "getting Name\n");
      s = Accessible_getName (obj);
      _festival_say (s, "voice_kal_diphone",
		     shutup_previous_speech || festival_chatty);
      SPI_freeString (s);
    }
  
  if (Accessible_isComponent (obj))
    {
      long x, y, width, height;
      AccessibleComponent *component = Accessible_getComponent (obj);
      AccessibleComponent_getExtents (component, &x, &y, &width, &height,
                                      SPI_COORD_TYPE_SCREEN);
      fprintf (stderr, "Bounding box: (%ld, %ld) ; (%ld, %ld)\n",
               x, y, x+width, y+height);
      if (use_magnifier) {
	      magnifier_set_roi ((short) 0, x, y, width, height);
      }
    }
  /* if this is a text object, speak the first sentence. */

  if (Accessible_isText(obj))

  {
     AccessibleText *text_interface;
     long start_offset, end_offset;
     char *first_sentence = "empty";
     text_interface = Accessible_getText (obj);
     first_sentence = AccessibleText_getTextAtOffset (
	       text_interface, (long) 0, SPI_TEXT_BOUNDARY_SENTENCE_START, &start_offset, &end_offset);
     if (first_sentence)
       {
	 _festival_say(first_sentence, "voice_don_diphone", FALSE);
	 SPI_freeString (first_sentence);
       }
     len = AccessibleText_getCharacterCount (text_interface);
     s = AccessibleText_getText (text_interface, 0, len);
     fprintf (stderr, "done reporting on focussed object, text=%s\n", s);
  }
}

void
report_focus_event (AccessibleEvent *event, void *user_data)
{
  char *s;

  g_warning ("report focus event");

  g_return_if_fail (event->source != NULL);

  s = Accessible_getName (event->source);
  if (cspi_warn_ev (cspi_ev (), "Foobar"))
    {
      fprintf (stderr, "%s event from %s\n", event->type, s);
      SPI_freeString (s);
      report_focussed_accessible (event->source, TRUE);
    }
  Accessible_getParent (event->source);
}

void
report_button_press (AccessibleEvent *event, void *user_data)
{
  char *s;

  g_return_if_fail (event->source != NULL);

  s = Accessible_getName (event->source);

  fprintf (stderr, "%s event from %s\n", event->type, s);
  SPI_freeString (s);
  s = Accessible_getDescription (event->source);
  fprintf (stderr, "Object description %s\n", s);
  SPI_freeString (s);
}

void
check_property_change (AccessibleEvent *event, void *user_data)
{
  AccessibleSelection *selection = Accessible_getSelection (event->source);
  int n_selections;
  int i;
  char *s;
  if (selection)
  {
    n_selections = (int) AccessibleSelection_getNSelectedChildren (selection);
    s = Accessible_getName (event->source);
    fprintf (stderr, "(Property) %s event from %s, %d selected children\n",
	     event->type, s, n_selections);
    SPI_freeString (s);
  /* for now, speak entire selection set */
    for (i=0; i<n_selections; ++i)
      {
        Accessible *obj = AccessibleSelection_getSelectedChild (selection, (long) i);
	g_return_if_fail (obj);
	s = Accessible_getName (obj);
	fprintf (stderr, "Child %d, name=%s\n", i, s);
	SPI_freeString (s);
	report_focussed_accessible (obj, i==0);
    }
  }
}

static void
simple_at_exit ()
{
  deregisterGlobalEventListenerAll (focus_listener);
  AccessibleEventListener_unref    (focus_listener);

  deregisterGlobalEventListenerAll (property_listener);
  AccessibleEventListener_unref    (property_listener);

  deregisterGlobalEventListenerAll (button_listener);
  AccessibleEventListener_unref    (button_listener);

  deregisterAccessibleKeystrokeListener (command_key_listener, SPI_KEYMASK_ALT);
  deregisterAccessibleKeystrokeListener (ordinary_key_listener, SPI_KEYMASK_UNMODIFIED);
  deregisterAccessibleKeystrokeListener (ordinary_key_listener, SPI_KEYMASK_SHIFT);
  AccessibleKeystrokeListener_unref (command_key_listener);
  AccessibleKeystrokeListener_unref (ordinary_key_listener);
  
  SPI_event_quit ();
}

static SPIBoolean
is_command_key (AccessibleKeystroke *key)
{
  switch (key->keyID)
    {
    case 'Q':
    case 'q':
	    simple_at_exit(); 
	    return TRUE; /* not reached */
    case 'M':
    case 'm':
	    use_magnifier = ! use_magnifier;
	    return TRUE;
    case 'F':
    case 'f':
	    use_festival = ! use_festival;
	    return TRUE;
    default:
	    return FALSE;
    }
}

static SPIBoolean
report_command_key_event (AccessibleKeystroke *key, void *user_data)
{
  fprintf (stderr, "Command KeyEvent %s%c (keycode %d)\n",
	  (key->modifiers & SPI_KEYMASK_ALT)?"Alt-":"",
	  ((key->modifiers & SPI_KEYMASK_SHIFT)^(key->modifiers & SPI_KEYMASK_SHIFTLOCK))?
	  (char) toupper((int) key->keyID) : (char) tolower((int) key->keyID),
	  (int) key->keycode);
  return is_command_key (key);
}


static SPIBoolean
report_ordinary_key_event (AccessibleKeystroke *key, void *user_data)
{
  fprintf (stderr, "Received key event:\tsym %ld\n\tmods %x\n\tcode %d\n\ttime %ld\n",
	   (long) key->keyID,
	   (unsigned int) key->modifiers,
	   (int) key->keycode,
	   (long int) key->timestamp);
  return FALSE;
}

static int
_festival_init ()
{
  int fd;
  struct sockaddr_in name;
  int tries = 2;

  name.sin_family = AF_INET;
  name.sin_port = htons (1314);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  fd = socket (PF_INET, SOCK_STREAM, 0);

  while (connect(fd, (struct sockaddr *) &name, sizeof (name)) < 0) {
    if (!tries--) {
      perror ("connect");
      return -1;
    }
  }

  _festival_write ("(audio_mode'async)\n", fd);
  _festival_write ("(Parameter.set 'Duration_Model 'Tree_ZScore)\n", fd);
  _festival_write ("(Parameter.set 'Duration_Stretch 0.75)\n", fd);
  return fd;
}

static void 
_festival_say (const char *text, const char *voice, SPIBoolean shutup)
{
  static int fd = 0;
  gchar *quoted;
  gchar *p;
  gchar prefix[50];
  static gchar voice_spec[32];
  
  if (!fd)
    {
      fd = _festival_init ();
    }

  fprintf (stderr, "saying text: %s\n", text);
  
  quoted = g_malloc(64+strlen(text)*2);

  sprintf (prefix, "(SayText \"");

  strncpy(quoted, prefix, 10);
  p = quoted+strlen(prefix);
  while (*text) {
    if ( *text == '\\' || *text == '"' )
      *p = '\\';
    *p++ = *text++;
  }
  *p++ = '"';
  *p++ = ')';
  *p++ = '\n';
  *p = 0;

  if (shutup) _festival_write ("(audio_mode'shutup)\n", fd);
  if (voice && (strncmp (voice, (char *) (voice_spec+1), strlen(voice))))
    {
      snprintf (voice_spec, 32, "(%s)\n", voice); 
      _festival_write (voice_spec, fd);
      _festival_write ("(Parameter.set 'Duration_Model 'Tree_ZScore)\n", fd);
      _festival_write ("(Parameter.set 'Duration_Stretch 0.75)\n", fd);
    }

  _festival_write (quoted, fd);

  g_free(quoted);
}

static void
_festival_write (const gchar *command_string, int fd)
{
  fprintf(stderr, command_string);
  if (fd < 0) {
    perror("socket");
    return;
  }
  write(fd, command_string, strlen(command_string));
}

