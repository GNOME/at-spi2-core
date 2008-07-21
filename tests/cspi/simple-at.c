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
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#undef MAGNIFIER_ENABLED
#include "../../cspi/spi-private.h" /* A hack for now */

#define PRINT_TREE

static void report_focus_event    (const AccessibleEvent *event, void *user_data);
static void report_generic_event  (const AccessibleEvent *event, void *user_data);
static void report_window_event  (const AccessibleEvent *event, void *user_data);
static void report_text_event     (const AccessibleEvent *event, void *user_data);
static void report_button_press   (const AccessibleEvent *event, void *user_data);
static void check_property_change (const AccessibleEvent *event, void *user_data);
static SPIBoolean report_command_key_event  (const AccessibleKeystroke *stroke, void *user_data);
static SPIBoolean report_ordinary_key_event (const AccessibleKeystroke *stroke, void *user_data);
static void get_environment_vars (void);

static int _festival_init (void);
static void _festival_say (const char *text, const char *voice, SPIBoolean shutup);
static void _festival_write (const char *buff, int fd);

#ifdef PRINT_TREE
static void print_accessible_tree (Accessible *accessible, char *prefix);
#endif

#ifdef MAGNIFIER_ENABLED
static SPIBoolean use_magnifier = FALSE;
#endif

static SPIBoolean use_festival = FALSE;
static SPIBoolean festival_chatty = FALSE;
static SPIBoolean name_changed = FALSE;

static AccessibleEventListener *focus_listener;
static AccessibleEventListener *property_listener;
static AccessibleEventListener *generic_listener;
static AccessibleEventListener *window_listener;
static AccessibleEventListener *button_listener;
static AccessibleEventListener *text_listener;
static AccessibleKeystrokeListener *command_key_listener;
static AccessibleKeystrokeListener *ordinary_key_listener;
static AccessibleKeySet            *command_keyset;

int
main (int argc, char **argv)
{
  int i, j;
  int n_desktops;
  int n_apps;
  char *s;
  Accessible *desktop;
  Accessible *application;
  const char *modules;

  if ((argc > 1) && (!strncmp (argv[1], "-h", 2)))
    {
      printf ("Usage: simple-at\n");
      printf ("\tEnvironment variables used:\n\t\tFESTIVAL\n\t\tMAGNIFIER\n\t\tFESTIVAL_CHATTY\n");
      exit (0);
    }

  modules = g_getenv ("GTK_MODULES");
  if (!modules || modules [0] == '\0')
    {
      putenv ("GTK_MODULES=");
    }
  modules = NULL;

  SPI_init ();

  focus_listener = SPI_createAccessibleEventListener (report_focus_event, NULL);
  property_listener = SPI_createAccessibleEventListener (check_property_change, NULL); 
  generic_listener = SPI_createAccessibleEventListener (report_generic_event, NULL); 
  window_listener = SPI_createAccessibleEventListener (report_window_event, NULL); 
  text_listener = SPI_createAccessibleEventListener (report_text_event, NULL); 
  button_listener = SPI_createAccessibleEventListener (report_button_press, NULL);
  SPI_registerGlobalEventListener (focus_listener, "focus:");
  SPI_registerGlobalEventListener (property_listener, "object:property-change");
/* :accessible-selection"); */
  SPI_registerGlobalEventListener (property_listener, "object:property-change:accessible-name");
  SPI_registerGlobalEventListener (generic_listener, "object:selection-changed"); 
  SPI_registerGlobalEventListener (generic_listener, "object:children-changed"); 
  SPI_registerGlobalEventListener (generic_listener, "object:visible-data-changed"); 
  SPI_registerGlobalEventListener (generic_listener, "object:text-selection-changed"); 
  SPI_registerGlobalEventListener (text_listener, "object:text-caret-moved"); 
  SPI_registerGlobalEventListener (text_listener, "object:text-changed"); 
  SPI_registerGlobalEventListener (button_listener, "Gtk:GtkWidget:button-press-event");
  SPI_registerGlobalEventListener (window_listener, "window:minimize");
  SPI_registerGlobalEventListener (window_listener, "window:activate");
  n_desktops = SPI_getDesktopCount ();

  for (i=0; i<n_desktops; ++i)
    {
      desktop = SPI_getDesktop (i);
      s = Accessible_getName (desktop);
      fprintf (stderr, "desktop %d name: %s\n", i, s);
      SPI_freeString (s);
      n_apps = Accessible_getChildCount (desktop);
      for (j=0; j<n_apps; ++j)
        {
          application = Accessible_getChildAtIndex (desktop, j);
	  s = Accessible_getName (application);
          fprintf (stderr, "app %d name: %s\n", j, s ? s : "(nil)");
#ifdef PRINT_TREE
	  print_accessible_tree (application, "*");
#endif
          SPI_freeString (s);
          Accessible_unref (application);
        }
      Accessible_unref (desktop);
    }

  /* prepare the keyboard snoopers */
  command_key_listener = SPI_createAccessibleKeystrokeListener (report_command_key_event, NULL);
  ordinary_key_listener = SPI_createAccessibleKeystrokeListener (report_ordinary_key_event, NULL);

  command_keyset = SPI_createAccessibleKeySet (11, "qmf23456789", NULL, NULL);
  
  /* will listen only to Control-Alt-q KeyPress events */
  SPI_registerAccessibleKeystrokeListener(command_key_listener,
					  command_keyset,
					  SPI_KEYMASK_ALT | SPI_KEYMASK_CONTROL,
					  (unsigned long) ( SPI_KEY_PRESSED ),
					  SPI_KEYLISTENER_ALL_WINDOWS);

  /* will listen only to CAPSLOCK key events, both press and release */
  SPI_registerAccessibleKeystrokeListener(ordinary_key_listener,
					  (AccessibleKeySet *) SPI_KEYSET_ALL_KEYS,
					  SPI_KEYMASK_SHIFTLOCK,
					  (unsigned long) ( SPI_KEY_PRESSED | SPI_KEY_RELEASED ),
					  SPI_KEYLISTENER_NOSYNC);

  get_environment_vars ();

  SPI_event_main ();

  putenv ("AT_BRIDGE_SHUTDOWN=1");

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
#ifdef MAGNIFIER_ENABLED
  if (g_getenv ("MAGNIFIER"))
    {
      fprintf (stderr, "Using magnifier\n");
      use_magnifier = TRUE;
    }  
  else
    {
      fprintf (stderr, "Not using magnifier\n");
    }
#endif

  if (!use_festival)
    {
      fprintf (stderr, "No speech output\n");
    }
}

#ifdef PRINT_TREE
static void
print_accessible_tree (Accessible *accessible, char *prefix)
{
	int n_children;
	int i;
	char *name;
	char *role_name;
	char *parent_name = NULL;
	char *parent_role = NULL;
	char child_prefix[100];
	Accessible *child;
	Accessible *parent;
	
	strncpy (child_prefix, prefix, 98);
	strcat (child_prefix, "*");
	parent = Accessible_getParent (accessible);
	if (parent)
	  {
		parent_name = Accessible_getName (parent);
		parent_role = Accessible_getRoleName (parent);
	        Accessible_unref (parent);
	  }
	name = Accessible_getName (accessible);
	role_name = Accessible_getRoleName (accessible);
	fprintf (stdout, "%sAccessible [%s] \"%s\"; parent [%s] %s.\n",
		 prefix, role_name, name ? name : "(nil)",
		 parent_role ? parent_role : "(nil)",
		 parent_name ? parent_name : "(nil)");
	SPI_freeString (name);
	SPI_freeString (role_name);
	SPI_freeString (parent_name);
	SPI_freeString (parent_role);
	n_children = Accessible_getChildCount (accessible);
	for (i = 0; i < n_children; ++i)
	        {
			child = Accessible_getChildAtIndex (accessible, i);
			print_accessible_tree (child, child_prefix);
			Accessible_unref (child);
	        }
}
#endif

void
report_focussed_accessible (Accessible *obj, SPIBoolean shutup_previous_speech)
{
  char *s;
  int len;
  long x, y, width, height;
  /* hack for GUADEC demo, to make sure name changes are spoken */
  shutup_previous_speech = (shutup_previous_speech && !name_changed);

  if (use_festival)
    {
      if (festival_chatty) 	    
        {
	  s = Accessible_getRoleName (obj);	
          _festival_say (s, "voice_don_diphone", shutup_previous_speech);
	  SPI_freeString (s);
        }
      fprintf (stderr, "getting Name\n");
      s = Accessible_getName (obj);
      _festival_say (s, "voice_kal_diphone",
		     shutup_previous_speech || festival_chatty);
      SPI_freeString (s);
    }
  
  if (Accessible_isComponent (obj))
    {
      AccessibleComponent *component = Accessible_getComponent (obj);
      AccessibleComponent_getExtents (component, &x, &y, &width, &height,
                                      SPI_COORD_TYPE_SCREEN);
      fprintf (stderr, "Bounding box: (%ld, %ld) ; (%ld, %ld)\n",
               x, y, x+width, y+height);
      if (Accessible_isText (obj))
	{
	  long x0, y0, xN, yN, w0, h0, wN, hN, nchars;
	  AccessibleText *text = Accessible_getText (obj);
	  nchars = AccessibleText_getCharacterCount (text);
	  if (nchars > 0) 
	    {
	      AccessibleText_getCharacterExtents (text, 0, &x0, &y0, &w0, &h0,
					      SPI_COORD_TYPE_SCREEN);
	      AccessibleText_getCharacterExtents (text, nchars-1, &xN, &yN, &wN, &hN, 
						  SPI_COORD_TYPE_SCREEN);
	      x = MIN (x0, xN);
	      width = MAX (x0 + w0, xN + wN) - x;
	      fprintf (stderr, "Text bounding box: (%ld, %ld) ; (%ld, %ld)\n",
		       x, y, x+width, y+height);
	    }
	}
#ifdef MAGNIFIER_ENABLED
      if (use_magnifier) {
	magnifier_set_roi ((short) 0, x, y, width, height);
      }
#endif
    }


  if (Accessible_isValue (obj))
    {
      AccessibleValue *value = Accessible_getValue (obj);
      fprintf (stderr, "Current value = %f, min = %f; max = %f\n",
               AccessibleValue_getCurrentValue (value),
               AccessibleValue_getMinimumValue (value),
	       AccessibleValue_getMaximumValue (value));
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
     if (first_sentence && use_festival)
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
report_focus_event (const AccessibleEvent *event, void *user_data)
{
  char *s;

  g_return_if_fail (event->source != NULL);
  s = Accessible_getName (event->source);
  if (s)
    {
      fprintf (stderr, "%s event from %s\n", event->type, s);
      SPI_freeString (s);
      report_focussed_accessible (event->source, TRUE);
    }
  Accessible_getParent (event->source);
  name_changed = FALSE;
}

void
report_generic_event (const AccessibleEvent *event, void *user_data)
{
  fprintf (stderr, "%s event received\n", event->type);
}

void
report_window_event (const AccessibleEvent *event, void *user_data)
{
  fprintf (stderr, "%s event received\n", event->type);
  if (!strcmp (event->type, "window:activate"))
  {
      print_accessible_tree (event->source, "window");
  }
}

void
report_text_event (const AccessibleEvent *event, void *user_data)
{
  AccessibleText *text = Accessible_getText (event->source);
  fprintf (stderr, "%s event received\n", event->type);
#ifdef MAGNIFIER_ENABLED
  if (use_magnifier && strcmp (event->type, "object:text-changed"))
    {
      long offset = AccessibleText_getCaretOffset (text);
      long x, y, w, h;
      fprintf (stderr, "offset %d\n", (int) offset);
      AccessibleText_getCharacterExtents (text, offset, &x, &y, &w, &h, 
					  SPI_COORD_TYPE_SCREEN);
      fprintf (stderr, "new roi %d %d %d %d\n", (int) x, (int) y, (int) w, (int) h);
      magnifier_set_roi ((short) 0, x, y, w, h);
    }
#endif
  if (!strcmp (event->type, "object:text-changed"))
    {
      long start, end;
      char *new_text = AccessibleText_getTextAtOffset (text, (long) event->detail1, SPI_TEXT_BOUNDARY_WORD_START, &start, &end);
      _festival_say (new_text, "voice_kal_diphone", FALSE);
      fprintf (stderr, "text changed: %s", new_text ? new_text : "");
      SPI_freeString (new_text);
    }
  else
    {
      long start, end;
      char *word_text = AccessibleText_getTextAtOffset (text, (long) event->detail1, SPI_TEXT_BOUNDARY_WORD_START, &start, &end);
      char *sentence_text = AccessibleText_getTextAtOffset (text, (long) event->detail1, SPI_TEXT_BOUNDARY_SENTENCE_START, &start, &end);
      fprintf (stderr, "text changed: word %s; sentence %s at %ld",
	       (word_text ? word_text : ""),
      	       (sentence_text ? sentence_text : ""),
	       event->detail1);
      if (word_text) SPI_freeString (word_text);
      if (sentence_text) SPI_freeString (sentence_text);
    }
}

void
report_button_press (const AccessibleEvent *event, void *user_data)
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
check_property_change (const AccessibleEvent *event, void *user_data)
{
  AccessibleSelection *selection = Accessible_getSelection (event->source);
  int n_selections;
  int i;
  char *s;
  fprintf (stderr, "property change event!\n");
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
  else if (!strcmp (event->type, "object:property-change:accessible-name"))
    {
      name_changed = TRUE;	    
      report_focussed_accessible (event->source, TRUE);
    }
  else
    {
      fprintf (stderr, "Property change %s received\n", event->type);
    }
}

static void
simple_at_exit (void)
{
  SPI_deregisterGlobalEventListenerAll (focus_listener);
  AccessibleEventListener_unref        (focus_listener);

  SPI_deregisterGlobalEventListenerAll (property_listener);
  AccessibleEventListener_unref        (property_listener);

  SPI_deregisterGlobalEventListenerAll (generic_listener);
  AccessibleEventListener_unref        (generic_listener);

  SPI_deregisterGlobalEventListenerAll (text_listener);
  AccessibleEventListener_unref        (text_listener);

  SPI_deregisterGlobalEventListenerAll (button_listener);
  AccessibleEventListener_unref        (button_listener);

  SPI_deregisterAccessibleKeystrokeListener (command_key_listener, SPI_KEYMASK_ALT | SPI_KEYMASK_CONTROL);
  AccessibleKeystrokeListener_unref         (command_key_listener);
  SPI_freeAccessibleKeySet                  (command_keyset);

  SPI_deregisterAccessibleKeystrokeListener (ordinary_key_listener, SPI_KEYMASK_SHIFTLOCK);
  AccessibleKeystrokeListener_unref         (ordinary_key_listener);

  SPI_event_quit ();
}

static SPIBoolean
is_command_key (const AccessibleKeystroke *key)
{
  switch (key->keyID)
    {
    case 'Q':
    case 'q':
	    simple_at_exit(); 
	    return TRUE; /* not reached */
#ifdef MAGNIFIER_ENABLED
    case 'M':
    case 'm':
	    use_magnifier = ! use_magnifier;
            fprintf (stderr, "%ssing magnifier\n", use_magnifier ? "U" : "Not u");
	    return TRUE;
#endif
    case 'F':
    case 'f':
	    use_festival = ! use_festival;
            fprintf (stderr, "%speech output\n", use_festival ? "S" : "No s");
	    return TRUE;
    default:
	    return FALSE;
    }
}

static SPIBoolean
report_command_key_event (const AccessibleKeystroke *key, void *user_data)
{
  fprintf (stderr, "Command KeyEvent %s%c (keycode %d); string=%s; time=%lx\n",
	  (key->modifiers & SPI_KEYMASK_ALT)?"Alt-":"",
	  ((key->modifiers & SPI_KEYMASK_SHIFT)^(key->modifiers & SPI_KEYMASK_SHIFTLOCK))?
	  (char) toupper((int) key->keyID) : (char) tolower((int) key->keyID),
	  (int) key->keycode,
   	  key->keystring,
	  (long int) key->timestamp);
  return is_command_key (key);
}


static SPIBoolean
report_ordinary_key_event (const AccessibleKeystroke *key, void *user_data)
{
  fprintf (stderr, "Received key event:\tsym %ld\n\tmods %x\n\tcode %d\n\tstring=\'%s\'\n\ttime %lx\n",
	   (long) key->keyID,
	   (unsigned int) key->modifiers,
	   (int) key->keycode,
	   key->keystring,
	   (long int) key->timestamp);
  return FALSE;
}

static int
_festival_init (void)
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

