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
#include <sys/socket.h>
#include <sys/un.h>
#include "spi.h"

static void report_focus_event (void *fp);
static void report_button_press (void *fp);
static boolean report_key_event (void *fp);
static void check_property_change (void *fp);
static void get_environment_vars (void);

static int _festival_init ();
static void _festival_say (const char *text, const char *voice, boolean shutup);
static void _festival_write (const char *buff, int fd);

static boolean use_magnifier = FALSE;
static boolean use_festival = FALSE;
static boolean festival_chatty = FALSE;

static SpiAccessibleEventListener *focus_listener;
static SpiAccessibleEventListener *property_listener;
static SpiAccessibleEventListener *button_listener;
static KeystrokeListener *key_listener;

int
main(int argc, char **argv)
{
  int i, j;
  int n_desktops;
  int n_apps;
  SpiAccessible *desktop;
  SpiAccessible *application;

  if ((argc > 1) && (!strncmp(argv[1],"-h",2)))
  {
    printf ("Usage: simple-at\n");
    printf ("\tEnvironment variables used:\n\t\tFESTIVAL\n\t\tMAGNIFIER\n\t\tFESTIVAL_CHATTY\n");
    exit(0);
  }

  SPI_init();

  focus_listener = createEventListener (report_focus_event);
  property_listener = createEventListener (check_property_change); 
  button_listener = createEventListener (report_button_press);
  registerGlobalEventListener (focus_listener, "focus:");
  registerGlobalEventListener (property_listener, "object:property-change:accessible-selection"); 
  registerGlobalEventListener (button_listener, "Gtk:GtkWidget:button-press-event");
  n_desktops = getDesktopCount ();

  for (i=0; i<n_desktops; ++i)
    {
      desktop = getDesktop (i);
      fprintf (stderr, "desktop %d name: %s\n", i, SpiAccessible_getName (desktop));
      n_apps = SpiAccessible_getChildCount (desktop);
      for (j=0; j<n_apps; ++j)
        {
          application = SpiAccessible_getChildAtIndex (desktop, j);
          fprintf (stderr, "app %d name: %s\n", j, SpiAccessible_getName (application));
	  SpiAccessible_unref (application);
        }
    }

  /* prepare the keyboard snoopers */
  key_listener = createKeystrokeListener(report_key_event);
  /* will listen only to Alt-key combinations */
  registerKeystrokeListener(key_listener,
			    (KeySet *) ALL_KEYS,
			    KEYMASK_ALT,
			    (unsigned long) ( KeyPress | KeyRelease),
			    KEYSPI_LISTENER_CANCONSUME);

  get_environment_vars();

  SPI_event_main(TRUE);
}

static void
get_environment_vars()
{
  if (getenv ("FESTIVAL"))
  {
    use_festival = TRUE;
    if (getenv ("FESTIVAL_CHATTY"))
    {
      festival_chatty = TRUE;
    }
  }
  if (getenv("MAGNIFIER"))
  {
    use_magnifier = TRUE;
  }  
}

void
report_focussed_accessible (SpiAccessible *obj, boolean shutup_previous_speech)
{
  if (use_festival)
    {
    if (festival_chatty) 	    
      {
        _festival_say (SpiAccessible_getRole (obj), "voice_don_diphone", shutup_previous_speech);
      }
      fprintf (stderr, "getting Name\n");
      _festival_say (SpiAccessible_getName (obj), "voice_kal_diphone",
		     shutup_previous_speech || festival_chatty);
    }
  
  if (SpiAccessible_isSpiComponent (obj))
    {
      long x, y, width, height;
      SpiAccessibleComponent *component = SpiAccessible_getComponent (obj);
      SpiAccessibleComponent_getExtents (component, &x, &y, &width, &height,
                                      COORD_TYPE_SCREEN);
      fprintf (stderr, "Bounding box: (%ld, %ld) ; (%ld, %ld)\n",
               x, y, x+width, y+height);
      if (use_magnifier) {
	      magnifier_set_roi ((short) 0, x, y, width, height);
      }
    }
  /* if this is a text object, speak the first sentence. */
  if (SpiAccessible_isSpiText(obj))
  {
     SpiAccessibleText *spi_text_interface;
     long start_offset, end_offset;
     char *first_sentence = "empty";
     spi_text_interface = SpiAccessible_getText (obj);
     fprintf (stderr, "isSpiText...%p %p\n", spi_text_interface, (void *)*spi_text_interface);
     first_sentence = SpiAccessibleText_getTextAtOffset (
	       spi_text_interface, (long) 0, SPI_TEXT_BOUNDARY_SENTENCE_START, &start_offset, &end_offset);
     if (first_sentence) _festival_say(first_sentence, "voice_don_diphone", FALSE);
     fprintf (stderr, "done reporting on focussed object\n");
  }
}

void
report_focus_event (void *p)
{
  SpiAccessibleEvent *ev = (SpiAccessibleEvent *) p;
  fprintf (stderr, "%s event from %s\n", ev->type,
           SpiAccessible_getName (&ev->source));
  report_focussed_accessible (&ev->source, TRUE);
}

void
report_button_press (void *p)
{
  SpiAccessibleEvent *ev = (SpiAccessibleEvent *) p;
  fprintf (stderr, "%s event from %s\n", ev->type,
           SpiAccessible_getName (&ev->source));
}


void
check_property_change (void *p)
{
  SpiAccessibleEvent *ev = (SpiAccessibleEvent *) p;
  SpiAccessibleSelection *selection = SpiAccessible_getSelection (&ev->source);
  int n_selections;
  int i;
  if (selection)
  {
    n_selections = (int) SpiAccessibleSelection_getNSelectedChildren (selection);
    fprintf (stderr, "(Property) %s event from %s, %d selected children\n", ev->type,
           SpiAccessible_getName (&ev->source), n_selections);
  /* for now, speak entire selection set */
    for (i=0; i<n_selections; ++i)
    {
	  SpiAccessible *obj = SpiAccessibleSelection_getSelectedChild (selection, (long) i);
	  g_return_if_fail (obj);
          fprintf (stderr, "Child %d, name=%s\n", i, SpiAccessible_getName (obj));
	  report_focussed_accessible (obj, i==0);
    }
  }
}

static void
simple_at_exit()
{
  deregisterGlobalEventListenerAll (focus_listener);
  deregisterGlobalEventListenerAll (property_listener);
  deregisterGlobalEventListenerAll (button_listener);
  deregisterKeystrokeListener (key_listener, KEYMASK_ALT );
  
  SPI_exit ();
}

static boolean
is_command_key (KeyStroke *key)
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

static boolean
report_key_event (void *p)
{
  KeyStroke *key = (KeyStroke *) p;
  fprintf(stderr, "KeyEvent %s%c (keycode %d)\n",
	  (key->modifiers & KEYMASK_ALT)?"Alt-":"",
	  ((key->modifiers & KEYMASK_SHIFT)^(key->modifiers & KEYMASK_SHIFTLOCK))?
	  (char) toupper((int) key->keyID) : (char) tolower((int) key->keyID),
	  (int) key->keycode);
  return is_command_key (key);
}

static int _festival_init ()
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

static void _festival_say (const char *text, const char *voice, boolean shutup)
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

  sprintf (prefix, "(SaySpiText \"");

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

static void _festival_write (const gchar *command_string, int fd)
{
  fprintf(stderr, command_string);
  if (fd < 0) {
    perror("socket");
    return;
  }
  write(fd, command_string, strlen(command_string));
}

