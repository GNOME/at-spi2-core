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

/*
 * desktop.c: implements SpiDesktop.idl
 *
 */

/* #include <config.h> */
#include <libbonobo.h>

#include <stdio.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (Gtk Type)
 */
#include "desktop.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE SPI_ACCESSIBLE_TYPE

/*
 * A pointer to our parent object class
 */
static SpiAccessibleClass *parent_class;

static void
spi_desktop_init (SpiDesktop  *desktop)
{
  SPI_ACCESSIBLE (desktop)->atko = g_object_new (atk_object_get_type(), NULL);
  desktop->applications = NULL;
  atk_object_set_name (ATK_OBJECT (SPI_ACCESSIBLE (desktop)->atko), "main");
}

static CORBA_long
impl_desktop_get_child_count (PortableServer_Servant servant,
                              CORBA_Environment * ev)
{
  SpiDesktop *desktop = SPI_DESKTOP (bonobo_object_from_servant (servant));
  if (desktop->applications)
    {
      return g_list_length (desktop->applications);
    }
  else
    {
      return 0;
    }
}

static Accessibility_Accessible
impl_desktop_get_child_at_index (PortableServer_Servant servant,
                                 const CORBA_long index,
                                 CORBA_Environment * ev)
{
  SpiDesktop *desktop = SPI_DESKTOP (bonobo_object_from_servant (servant));
  CORBA_Object retval;
  if ((desktop->applications) && (index < g_list_length (desktop->applications)))
    {
      fprintf (stderr, "getting application %ld\n", (long) index);
      /* */
      fprintf (stderr, "object address %p\n",
               g_list_nth_data (desktop->applications, index));
      retval =  CORBA_Object_duplicate (
              (CORBA_Object) g_list_nth_data (desktop->applications, index), ev);
    }
  else
    {
      fprintf (stderr, "no %ldth child\n", (long) index);
      retval = CORBA_OBJECT_NIL;
    }
  return (Accessibility_Accessible) retval;
}

static void
spi_desktop_class_init (SpiDesktopClass  *klass)
{
        SpiAccessibleClass * spi_accessible_class = (SpiAccessibleClass *) klass;
        POA_Accessibility_Accessible__epv *epv = &spi_accessible_class->epv;

        parent_class = g_type_class_ref (SPI_ACCESSIBLE_TYPE);

        epv->_get_childCount = impl_desktop_get_child_count;
        epv->getChildAtIndex = impl_desktop_get_child_at_index;
}

BONOBO_TYPE_FUNC_FULL (SpiDesktop,
		       Accessibility_Desktop,
		       PARENT_TYPE,
		       spi_desktop);

SpiDesktop *
spi_desktop_new (void)
{
    SpiDesktop *retval = SPI_DESKTOP (g_object_new (SPI_DESKTOP_TYPE, NULL));

    return retval;
}
