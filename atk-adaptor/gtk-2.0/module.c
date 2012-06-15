/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008, 2009 Codethink Ltd.
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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

#define _GNU_SOURCE
#include "config.h"

#include <gmodule.h>
#include <atk-bridge.h>

/*---------------------------------------------------------------------------*/

int
gtk_module_init (gint * argc, gchar ** argv[])
{
  return atk_bridge_adaptor_init (argc, argv);
}

gchar*
g_module_check_init (GModule *module)
{
  g_module_make_resident (module);

  return NULL;
}

void
gnome_accessibility_module_init (void)
{
  atk_bridge_adaptor_init (NULL, NULL);

  if (g_getenv ("AT_SPI_DEBUG"))
    {
      g_print ("Atk Accessibility bridge initialized\n");
    }
}

void
gnome_accessibility_module_shutdown (void)
{
    atk_bridge_adaptor_cleanup ();
}
