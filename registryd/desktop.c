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
 * desktop.c: implements Desktop.idl
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
#define PARENT_TYPE ACCESSIBLE_TYPE

/*
 * A pointer to our parent object class
 */
static AccessibleClass *parent_class;

static void
desktop_init (Desktop  *desktop)
{
  ACCESSIBLE (desktop)->atko = atk_simple_object_new();
  atk_object_set_name (ATK_OBJECT (ACCESSIBLE (desktop)->atko), "main");
}

static CORBA_long
impl_desktop_get_child_count (PortableServer_Servant servant,
                              CORBA_Environment * ev)
{
  Desktop *desktop = DESKTOP (bonobo_object_from_servant (servant));
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
  Desktop *desktop = DESKTOP (bonobo_object_from_servant (servant));
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
desktop_class_init (DesktopClass  *klass)
{
        AccessibleClass * accessible_class = (AccessibleClass *) klass;
        POA_Accessibility_Accessible__epv *epv = &accessible_class->epv;

        parent_class = g_type_class_ref (ACCESSIBLE_TYPE);

        epv->_get_childCount = impl_desktop_get_child_count;
        epv->getChildAtIndex = impl_desktop_get_child_at_index;
}

GType
desktop_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (DesktopClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) desktop_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Desktop),
                        0, /* n preallocs */
                        (GInstanceInitFunc) desktop_init,
                        NULL /* value table */
                };
                /*
                 *   Here we use bonobo_x_type_unique instead of
                 * gtk_type_unique, this auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_x_type_unique.
                 */
                type = bonobo_x_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_Desktop__init,
                        NULL,
                        G_STRUCT_OFFSET (DesktopClass, epv),
                        &tinfo,
                        "Desktop");
        }

        return type;
}

Desktop *
desktop_new (void)
{
    Desktop *retval =
               DESKTOP (g_object_new (desktop_get_type (), NULL));
    return retval;
}
