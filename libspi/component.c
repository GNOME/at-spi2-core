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
 * component.c : bonobo wrapper for accessible component implementation
 *
 */
#include <config.h>
#include <bonobo/Bonobo.h>

#include <stdio.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (Gtk Type)
 */
#include "component.h"
#include "accessible.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

/*
 * A pointer to our parent object class
 */
static GObjectClass *component_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
accessibility_component_object_finalize (GObject *object)
{
        Component *component = COMPONENT (object);

        printf("accessible_component_object_finalize called\n");
        g_object_unref (component->atko);
	component->atko = NULL;

        printf("atko freed, calling parent finalize\n");
        component_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::Component::contains method implementation
 */
static CORBA_boolean
impl_accessibility_component_contains (PortableServer_Servant servant,
                                       const CORBA_long x,
                                       const CORBA_long y,
                                       CORBA_short coord_type,
                                       CORBA_Environment     *ev)
{
  CORBA_boolean retval;
  Component *component = COMPONENT (bonobo_object_from_servant (servant));
  retval = atk_component_contains (ATK_COMPONENT (component->atko), (gint) x, (gint) y,
                                  (AtkCoordType) coord_type);
  fprintf (stderr, "Component contains() called: %s\n", retval ? "true" : "false" );
  return retval;
}

/*
 * CORBA Accessibility::Component::getAccessibleAtPoint method implementation
 */
static Accessibility_Accessible
impl_accessibility_component_get_accessible_at_point (PortableServer_Servant servant,
                                                      const CORBA_long x,
                                                      const CORBA_long y,
                                                      CORBA_short coord_type,
                                                      CORBA_Environment     *ev)
{
  Accessibility_Accessible retval;
  Component *component = COMPONENT (bonobo_object_from_servant (servant));
  AtkObject *child;
  child = atk_component_ref_accessible_at_point (ATK_COMPONENT (component->atko),
                                                  (gint) x, (gint) y,
                                                  (AtkCoordType) coord_type);
  retval = bonobo_object_corba_objref (bonobo_object (accessible_new (child)));
  return retval;
}

/*
 * CORBA Accessibility::Component::getExtents method implementation
 */
static void
impl_accessibility_component_get_extents (PortableServer_Servant servant,
                                          CORBA_long * x,
                                          CORBA_long * y,
                                          CORBA_long * width,
                                          CORBA_long * height,
                                          const CORBA_short coord_type,
                                          CORBA_Environment     *ev)
{
  Component *component = COMPONENT (bonobo_object_from_servant (servant));
  gint ix, iy, iw, ih;
  atk_component_get_extents (ATK_COMPONENT (component->atko), &ix, &iy, &iw, &ih,
                                  (AtkCoordType) coord_type);
  *x = (CORBA_long) ix;
  *y = (CORBA_long) iy;
  *width = (CORBA_long) iw;
  *height = (CORBA_long) ih;
}

/*
 * CORBA Accessibility::Component::getPosition method implementation
 */
static void
impl_accessibility_component_get_position (PortableServer_Servant servant,
                                           CORBA_long * x,
                                           CORBA_long * y,
                                           const CORBA_short coord_type,
                                           CORBA_Environment     *ev)
{
  Component *component = COMPONENT (bonobo_object_from_servant (servant));
  gint ix, iy;
  atk_component_get_position (ATK_COMPONENT (component->atko), &ix, &iy,
                              (AtkCoordType) coord_type);
  *x = (CORBA_long) ix;
  *y = (CORBA_long) iy;
}

/*
 * CORBA Accessibility::Component::getSize method implementation
 */
static void
impl_accessibility_component_get_size (PortableServer_Servant servant,
                                       CORBA_long * width,
                                       CORBA_long * height,
                                       CORBA_Environment     *ev)
{
  Component *component = COMPONENT (bonobo_object_from_servant (servant));
  gint iw, ih;
  atk_component_get_size (ATK_COMPONENT (component->atko), &iw, &ih);
  *width = (CORBA_long) iw;
  *height = (CORBA_long) ih;
}

static void
accessibility_component_class_init (ComponentClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_Component__epv *epv = &klass->epv;
        component_parent_class = g_type_class_peek_parent (klass);

        object_class->finalize = accessibility_component_object_finalize;

        epv->contains = impl_accessibility_component_contains;
        epv->getAccessibleAtPoint = impl_accessibility_component_get_accessible_at_point;
        epv->getExtents = impl_accessibility_component_get_extents;
        epv->getPosition = impl_accessibility_component_get_position;
        epv->getSize = impl_accessibility_component_get_size;
}

static void
accessibility_component_init (Component *component)
{
}

GType
accessibility_component_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (ComponentClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) accessibility_component_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Component),
                        0, /* n preallocs */
                        (GInstanceInitFunc) accessibility_component_init,
                        NULL /* value table */
                };
                /*
                 * Bonobo_type_unique auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_type_unique.
                 */
                type = bonobo_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_Component__init,
                        NULL,
                        G_STRUCT_OFFSET (ComponentClass, epv),
                        &tinfo,
                        "AccessibleComponent");
        }

        return type;
}

Component *
component_interface_new (AtkObject *o)
{
    Component *retval =
               COMPONENT (g_object_new (accessibility_component_get_type (), NULL));
    retval->atko = o;
    g_object_ref (o);
return retval;
}
