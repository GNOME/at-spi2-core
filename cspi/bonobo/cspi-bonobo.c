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

      if (!_cspi_exception_throw (ev, (char *) error_string)) {
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
		canonical_display_name = g_strdup (display_env);
		gchar *display_p = strrchr (canonical_display_name, ':');
		gchar *screen_p = strrchr (canonical_display_name, '.');
		if (screen_p && display_p && ((guint) screen_p > (guint) display_p))
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

CORBA_Object
cspi_init (void)
{
  char *obj_id;
  CORBA_Object registry;
  CORBA_Environment ev;

  if (!bonobo_init (0, NULL))
    {
      g_error ("Could not initialize Bonobo");
    }

  obj_id = "OAFIID:Accessibility_Registry:1.0";

  CORBA_exception_init (&ev);

  bonobo_activation_set_activation_env_value ("AT_SPI_DISPLAY", 
					      cspi_display_name ());

  registry = bonobo_activation_activate_from_id (
    obj_id, 0, NULL, &ev);

  if (ev._major != CORBA_NO_EXCEPTION)
    {
      g_error ("AT-SPI error: during registry activation: %s\n",
	       bonobo_exception_get_text (&ev));
    }

  if (registry == CORBA_OBJECT_NIL)
    {
      g_error ("Could not locate registry");
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
