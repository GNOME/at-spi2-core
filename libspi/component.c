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

/* component.c : implements the Component interface */

#include <config.h>
#include <stdio.h>
#include <libspi/accessible.h>
#include <libspi/component.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_TYPE_BASE

/* A pointer to our parent object class */
static GObjectClass *spi_component_parent_class;

static AtkComponent *
get_component_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_COMPONENT (object->gobj);
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
  AtkComponent *component = get_component_from_servant (servant);

  g_return_val_if_fail (component != NULL, FALSE);

  retval = atk_component_contains (component, x, y,
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
  AtkObject    *child;
  AtkComponent *component = get_component_from_servant (servant);

  g_return_val_if_fail (component != NULL, NULL);

  child = atk_component_ref_accessible_at_point (component,
						 x, y,
						 (AtkCoordType) coord_type);
  return spi_accessible_new_return (child, TRUE, ev);
}

/*
 * CORBA Accessibility::Component::getExtents method implementation
 */
static Accessibility_BoundingBox
impl_accessibility_component_get_extents (PortableServer_Servant servant,
                                          const CORBA_short      coord_type,
                                          CORBA_Environment     *ev)
{
  gint ix, iy, iw, ih;
  Accessibility_BoundingBox retval;
  AtkComponent *component = get_component_from_servant (servant);

  atk_component_get_extents (component, &ix, &iy, &iw, &ih,
			     (AtkCoordType) coord_type);

  retval.x = ix;
  retval.y = iy;
  retval.width = iw;
  retval.height = ih;

  return retval;
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
  gint ix, iy;
  AtkComponent *component = get_component_from_servant (servant);

  g_return_if_fail (component != NULL);

  atk_component_get_position (component, &ix, &iy,
                              (AtkCoordType) coord_type);
  *x = ix;
  *y = iy;
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
  gint iw, ih;
  AtkComponent *component = get_component_from_servant (servant);

  g_return_if_fail (component != NULL);

  atk_component_get_size (component, &iw, &ih);
  *width = iw;
  *height = ih;
}

static Accessibility_ComponentLayer
impl_accessibility_component_get_layer (PortableServer_Servant servant,
					CORBA_Environment     *ev)
{
  AtkLayer      atklayer;
  AtkComponent *component = get_component_from_servant (servant);

  g_return_val_if_fail (component != NULL, Accessibility_LAYER_INVALID);

  atklayer = atk_component_get_layer (component);
  switch (atklayer)
    {
      case ATK_LAYER_BACKGROUND:
        return Accessibility_LAYER_BACKGROUND;
      case ATK_LAYER_CANVAS:
        return Accessibility_LAYER_CANVAS;
      case ATK_LAYER_WIDGET:
        return Accessibility_LAYER_WIDGET;
      case ATK_LAYER_MDI:
        return Accessibility_LAYER_MDI;
      case ATK_LAYER_POPUP:
        return Accessibility_LAYER_POPUP;
      case ATK_LAYER_OVERLAY:
        return Accessibility_LAYER_OVERLAY;
      case ATK_LAYER_WINDOW:
        return Accessibility_LAYER_WINDOW;
      default:
        break;      
    }
  return Accessibility_LAYER_INVALID;
}

static CORBA_short
impl_accessibility_component_get_mdi_z_order (PortableServer_Servant servant,
					      CORBA_Environment     *ev)
{
  AtkComponent *component = get_component_from_servant (servant);

  g_return_val_if_fail (component != NULL, -1);

  return atk_component_get_mdi_zorder (component);
}

static CORBA_boolean
impl_accessibility_component_grab_focus (PortableServer_Servant servant,
					 CORBA_Environment     *ev)
{
  AtkComponent *component = get_component_from_servant (servant);

  g_return_val_if_fail (component != NULL, FALSE);

  return atk_component_grab_focus (component);
}

static double
impl_accessibility_component_get_alpha (PortableServer_Servant servant,
					CORBA_Environment     *ev)
{
  AtkComponent *component = get_component_from_servant (servant);

  g_return_val_if_fail (component != NULL, FALSE);

  return atk_component_get_alpha (component);
}

static void
spi_component_class_init (SpiComponentClass *klass)
{
        POA_Accessibility_Component__epv *epv = &klass->epv;
        spi_component_parent_class = g_type_class_peek_parent (klass);

        epv->contains = impl_accessibility_component_contains;
        epv->getAccessibleAtPoint = impl_accessibility_component_get_accessible_at_point;
        epv->getExtents = impl_accessibility_component_get_extents;
        epv->getPosition = impl_accessibility_component_get_position;
        epv->getSize = impl_accessibility_component_get_size;
	epv->getLayer = impl_accessibility_component_get_layer;
	epv->getMDIZOrder = impl_accessibility_component_get_mdi_z_order;
	epv->grabFocus = impl_accessibility_component_grab_focus;
	epv->getAlpha = impl_accessibility_component_get_alpha;
}

static void
spi_component_init (SpiComponent *component)
{
}

BONOBO_TYPE_FUNC_FULL (SpiComponent,
		       Accessibility_Component,
		       PARENT_TYPE,
		       spi_component)

SpiComponent *
spi_component_interface_new (AtkObject *o)
{
    SpiComponent *retval = g_object_new (SPI_COMPONENT_TYPE, NULL);

    spi_base_construct (SPI_BASE (retval), G_OBJECT(o));

    return retval;
}
