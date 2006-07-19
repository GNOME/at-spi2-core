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
#include <gdk/gdk.h>
#include <libbonobo.h>
#include <glib/gmain.h>
#include "registry.h"

int
main (int argc, char **argv)
{
  int          ret;
  char        *obj_id;
  const char  *display_name;
  char        *cp, *dp;
  SpiRegistry *registry;
  GSList      *reg_env = NULL;

  if (!bonobo_init (&argc, argv))
    {
      g_error ("Could not initialize oaf / Bonobo");
    }

  obj_id = "OAFIID:Accessibility_Registry:1.0";

  registry = spi_registry_new ();

  display_name = g_getenv ("AT_SPI_DISPLAY");
  if (!display_name)
  {
      display_name = g_strdup (gdk_display_get_name (gdk_display_get_default ()));
      cp = strrchr (display_name, '.');
      dp = strrchr (display_name, ':');
      if (cp && dp && (cp > dp)) *cp = '\0';
  }

  reg_env = bonobo_activation_registration_env_set ( reg_env, "AT_SPI_DISPLAY", 
						     display_name);
  ret = bonobo_activation_register_active_server (
	  obj_id,
	  bonobo_object_corba_objref (bonobo_object (registry)),
	  reg_env);
  bonobo_activation_registration_env_free (reg_env);

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

