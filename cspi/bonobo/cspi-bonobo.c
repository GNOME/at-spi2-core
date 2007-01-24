/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *           2002 Ximian Inc.
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

#include <string.h>
#include <libbonobo.h>
#include "../cspi-lowlevel.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>

CORBA_Object
cspi_dup_ref (CORBA_Object object)
{
  return bonobo_object_dup_ref (object, cspi_ev ());
}

void
cspi_release_unref (CORBA_Object object)
{
    bonobo_object_release_unref (object, NULL); 
}

SPIBoolean
cspi_check_ev (const char *error_string)
{
  CORBA_Environment *ev = cspi_peek_ev ();

  if (ev->_major != CORBA_NO_EXCEPTION)
    {
      char *err;

      err = bonobo_exception_get_text (ev);

      if (!cspi_exception_throw (ev, (char *) error_string)) {
	fprintf (stderr, "Warning: AT-SPI error: %s: %s\n",
		 error_string, err);
      }

      g_free (err);

      CORBA_exception_free (ev);

      return FALSE;
    }
  else
    {
      return TRUE;
    }
}

char *
cspi_exception_get_text (void)
{
  char *ret, *txt;

  txt = bonobo_exception_get_text (cspi_ev ());
  ret = strdup (txt);
  g_free (txt);

  return ret;
}

/* 
 * Returns a 'canonicalized' value for DISPLAY,
 * with the screen number stripped off if present.
 */
static const gchar*
cspi_display_name (void)
{
    static const char *canonical_display_name = NULL;
    if (!canonical_display_name)
    {
        const gchar *display_env = g_getenv ("AT_SPI_DISPLAY");
	if (!display_env)
	{
	    display_env = g_getenv ("DISPLAY");
	    if (!display_env || !display_env[0]) 
		canonical_display_name = ":0";
	    else
	    {
		gchar *display_p, *screen_p;
		canonical_display_name = g_strdup (display_env);
		display_p = strrchr (canonical_display_name, ':');
		screen_p = strrchr (canonical_display_name, '.');
		if (screen_p && display_p && (screen_p > display_p))
		{
		    *screen_p = '\0';
		}
	    }
	}
	else
	{
	    canonical_display_name = display_env;
	}
    }
    return canonical_display_name;
}

static gchar *
cspi_get_registry_ior (void)
{
  Atom AT_SPI_IOR;
  Atom actual_type;
  int actual_format;
  unsigned char *data = NULL;
  unsigned long nitems;
  unsigned long leftover;
  static Display *display = NULL;
  if (!display)
    display = XOpenDisplay (cspi_display_name ());

  AT_SPI_IOR = XInternAtom (display, "AT_SPI_IOR", False);
  XGetWindowProperty(display,
                     XDefaultRootWindow (display),
                     AT_SPI_IOR, 0L,
                     (long)BUFSIZ, False,
                     (Atom) 31, &actual_type, &actual_format,
                     &nitems, &leftover, &data);
  if (data == NULL)
    g_warning ("AT_SPI_REGISTRY was not started at session startup.");

  return (gchar *) data;
}

CORBA_Object
cspi_init (void)
{
  CORBA_Object registry = NULL;
  CORBA_Environment ev;
  char *ior =  NULL;

  if (!bonobo_init (NULL, NULL))
    {
      g_error ("Could not initialize Bonobo");
    }

  CORBA_exception_init (&ev);

  ior = (char *) cspi_get_registry_ior ();
  if (ior != NULL)
    {
      registry = CORBA_ORB_string_to_object (bonobo_activation_orb_get (),
			  			 ior, &ev);
    }

  if (ev._major != CORBA_NO_EXCEPTION)
    {
      g_error ("AT-SPI error: during registry activation: %s\n",
	       bonobo_exception_get_text (&ev));
    }

  if (registry == CORBA_OBJECT_NIL)
    {
      g_warning ("Could not locate registry");
    }

  bonobo_activate ();

  return registry;
}

SPIBoolean
cspi_ping (CORBA_Object object)
{
	return bonobo_unknown_ping (object, NULL);
}

void
cspi_main (void)
{
  bonobo_main ();
}

void
cspi_main_quit (void)
{
  bonobo_main_quit ();
}
