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

/* component.c : implements the Component interface */

#include <config.h>
#include <stdio.h>
#include <libspi/accessible.h>
#include <libspi/component.h>

/* Our parent Gtk object type */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

/* A pointer to our parent object class */
static GObjectClass *spi_component_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
accessibility_component_object_finalize (GObject *object)
{
        SpiComponent *component = SPI_COMPONENT (object);

        printf("spi_accessible_component_object_finalize called\n");
        g_object_unref (component->atko);
	component->atko = NULL;

        printf("component atko freed, calling parent finalize\n");
        spi_component_parent_class->finalize (object);
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
  BonoboObject *obj;
  SpiComponent *component;

  obj = bonobo_object_from_servant (servant);
  g_return_val_if_fail (IS_SPI_COMPONENT(obj), FALSE);
  component = SPI_COMPONENT (obj);
  g_return_val_if_fail (ATK_IS_COMPONENT(component->atko), FALSE);
  retval = atk_component_contains (ATK_COMPONENT (component->atko), (gint) x, (gint) y,
                                  (AtkCoordType) coord_type);
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
  BonoboObject *obj;
  SpiComponent *component;
  Accessibility_Accessible retval;
  AtkObject *child;

  obj = bonobo_object_from_servant (servant);
  g_return_val_if_fail (IS_SPI_COMPONENT(obj), CORBA_OBJECT_NIL);
  component = SPI_COMPONENT (obj);
  g_return_val_if_fail (ATK_IS_COMPONENT(component->atko), CORBA_OBJECT_NIL);

  child = atk_component_ref_accessible_at_point (ATK_COMPONENT (component->atko),
                                                  (gint) x, (gint) y,
                                                  (AtkCoordType) coord_type);
  retval = bonobo_object_corba_objref (bonobo_object (spi_accessible_new (child)));
  return CORBA_Object_duplicate (retval, ev);
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
  BonoboObject *obj;
  SpiComponent *component;
  gint ix, iy, iw, ih;

  obj = bonobo_object_from_servant (servant);
  g_return_if_fail (IS_SPI_COMPONENT(obj));
  component = SPI_COMPONENT (obj);
  g_return_if_fail (ATK_IS_COMPONENT (component->atko));

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
  BonoboObject *obj = bonobo_object_from_servant (servant);
  SpiComponent *component;
  gint ix, iy;

  g_return_if_fail (IS_SPI_COMPONENT(obj));
  component = SPI_COMPONENT(obj);
  g_return_if_fail (ATK_IS_COMPONENT(component->atko));

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
  SpiComponent *component;
  BonoboObject *obj = bonobo_object_from_servant (servant);
  gint iw, ih;

  g_return_if_fail (IS_SPI_COMPONENT(obj));
  component = SPI_COMPONENT(obj);
  g_return_if_fail (ATK_IS_COMPONENT(component->atko));
  atk_component_get_size (ATK_COMPONENT (component->atko), &iw, &ih);
  *width = (CORBA_long) iw;
  *height = (CORBA_long) ih;
}

static void
spi_component_class_init (SpiComponentClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_Component__epv *epv = &klass->epv;
        spi_component_parent_class = g_type_class_peek_parent (klass);

        object_class->finalize = accessibility_component_object_finalize;

        epv->contains = impl_accessibility_component_contains;
        epv->getAccessibleAtPoint = impl_accessibility_component_get_accessible_at_point;
        epv->getExtents = impl_accessibility_component_get_extents;
        epv->getPosition = impl_accessibility_component_get_position;
        epv->getSize = impl_accessibility_component_get_size;
}

static void
spi_component_init (SpiComponent *component)
{
}

BONOBO_TYPE_FUNC_FULL (SpiComponent,
		       Accessibility_Component,
		       PARENT_TYPE,
		       spi_component);

SpiComponent *
spi_component_interface_new (AtkObject *o)
{
    SpiComponent *retval =
               SPI_COMPONENT (g_object_new (SPI_COMPONENT_TYPE, NULL));
    retval->atko = o;
    g_object_ref (o);
    return retval;
}
