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

#ifdef AT_SPI_DEBUG
#include <stdlib.h>
#endif

#include <config.h>
#include <string.h>
#include <gdk/gdkx.h>
#include <libbonobo.h>
#include <glib/gmain.h>
#include "registry.h"

#define spi_get_display() GDK_DISPLAY()

static void registry_set_ior (SpiRegistry *registry);

int
main (int argc, char **argv)
{
  int          ret;
  char        *obj_id;
  const char  *display_name;
  char        *cp, *dp;
  SpiRegistry *registry;

  if (!bonobo_init (&argc, argv))
    {
      g_error ("Could not initialize oaf / Bonobo");
    }

  obj_id = "OAFIID:Accessibility_Registry:1.0";

  registry = spi_registry_new ();

  display_name = g_getenv ("AT_SPI_DISPLAY");
  if (!display_name)
  {
      display_name = g_getenv ("DISPLAY");
      cp = strrchr (display_name, '.');
      dp = strrchr (display_name, ':');
      if (cp && dp && (cp > dp)) *cp = '\0';
  }

  ret = bonobo_activation_register_active_server (
	  obj_id,
	  bonobo_object_corba_objref (bonobo_object (registry)),
	  NULL);

  registry_set_ior (registry);

  if (ret != Bonobo_ACTIVATION_REG_SUCCESS)
    {
#ifdef AT_SPI_DEBUG
      fprintf (stderr, "SpiRegistry Message: SpiRegistry daemon was already running.\n");
#endif
    }
  else
    {
#ifdef AT_SPI_DEBUG
      fprintf (stderr, "SpiRegistry Message: SpiRegistry daemon is running.\n");
#endif
      bonobo_main ();
    }

  return 0;
}

static void
registry_set_ior (SpiRegistry *registry){
  CORBA_Environment ev;
  Atom  AT_SPI_IOR = XInternAtom (spi_get_display (), "AT_SPI_IOR", FALSE);
  char *iorstring = NULL;

  CORBA_exception_init (&ev);

  iorstring = CORBA_ORB_object_to_string (bonobo_activation_orb_get (),
                                     bonobo_object_corba_objref (bonobo_object (registry)),
                                     &ev);

  XChangeProperty (spi_get_display(),
		   XDefaultRootWindow (spi_get_display ()),
		   AT_SPI_IOR, (Atom) 31, 8,
		   PropModeReplace,
		   (unsigned char *) iorstring,
		   iorstring ? strlen (iorstring) : 0);

  if (ev._major != CORBA_NO_EXCEPTION)
	  {
		  g_error ("Error setting IOR %s",
			   CORBA_exception_id (&ev));
                  CORBA_exception_free (&ev);
           }

  CORBA_exception_free (&ev);

}
