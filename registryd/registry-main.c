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

#ifdef AT_SPI_DEBUG
#include <stdlib.h>
#endif

#include <libbonobo.h>
#include <glib/gmain.h>
#include "registry.h"

int
main (int argc,
      char **argv)
{
        SpiRegistry *registry;
	GSource *keyevent_source;
        char *obj_id;

        if (!bonobo_init (&argc, argv))
          {
            g_error ("Could not initialize oaf / Bonobo");
          }

        obj_id = "OAFIID:Accessibility_Registry:proto0.1";

        registry = spi_registry_new ();

        bonobo_activation_active_server_register (
                obj_id,
                bonobo_object_corba_objref (bonobo_object (registry)));

#ifdef AT_SPI_DEBUG
        fprintf (stderr, "SpiRegistry Message: SpiRegistry daemon is running.\n");
#endif
  
        gdk_init(&argc, &argv);
        g_timeout_add_full (G_PRIORITY_HIGH_IDLE, 200, registry->kbd_event_hook, registry, NULL);
/*	keyevent_source =
		g_source_new (registry->kbd_event_hook, sizeof (GSourceFunc));
		g_source_attach (keyevent_source, g_main_context_default());*/
        bonobo_main ();

        return 0;
}



