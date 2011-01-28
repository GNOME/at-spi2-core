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

/*
 *
 * AtspiComponent function implementations
 *
 */

#include "atspi-private.h"

void
atspi_rect_free (AtspiRect *rect)
{
  g_free (rect);
}

AtspiRect *
atspi_rect_copy (AtspiRect *src)
{
  AtspiRect *dst = g_new (AtspiRect, 1);
  if (dst)
  {
    dst->x = src->x;
    dst->y = src->y;
    dst->height = src->height;
    dst->width = src->width;
  }
  return dst;
}

G_DEFINE_BOXED_TYPE (AtspiRect, atspi_rect, atspi_rect_copy, atspi_rect_free)

AtspiPoint *
atspi_point_copy (AtspiPoint *src)
{
  AtspiPoint *dst = g_new (AtspiPoint, 1);
  if (dst)
  {
    dst->x = src->x;
    dst->y = src->y;
  }
  return dst;
}

G_DEFINE_BOXED_TYPE (AtspiPoint, atspi_point, atspi_point_copy, g_free)

/**
 * atspi_component_contains:
 * @obj: a pointer to the #AtspiComponent to query.
 * @x: a #long specifying the x coordinate in question.
 * @y: a #long specifying the y coordinate in question.
 * @ctype: the desired coordinate system of the point (@x, @y)
 *         (e.g. CSPI_COORD_TYPE_WINDOW, CSPI_COORD_TYPE_SCREEN).
 *
 * Query whether a given #AtspiComponent contains a particular point.
 *
 * Returns: a #TRUE if the specified component contains the point (@x, @y),
 *          otherwise #FALSE.
 **/
gboolean
atspi_component_contains (AtspiComponent *obj,
                              gint x,
                              gint y,
                              AtspiCoordType ctype, GError **error)
{
  dbus_bool_t retval = FALSE;
  dbus_int32_t d_x = x, d_y = y;
  dbus_uint32_t d_ctype = ctype;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_component, "Contains", error, "iin=>b", d_x, d_y, d_ctype, &retval);

  return retval;
}

/**
 * atspi_component_get_accessible_at_point:
 * @obj: a pointer to the #AtspiComponent to query.
 * @x: a #gint specifying the x coordinate of the point in question.
 * @y: a #gint specifying the y coordinate of the point in question.
 * @ctype: the coordinate system of the point (@x, @y)
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * Get the accessible child at a given coordinate within an #AtspiComponent.
 *
 * Returns: (transfer full): a pointer to an #AtspiAccessible child of the
 *          specified component which contains the point (@x, @y), or NULL of
 *         no child contains the point.
 **/
AtspiAccessible *
atspi_component_get_accessible_at_point (AtspiComponent *obj,
                                          gint x,
                                          gint y,
                                          AtspiCoordType ctype, GError **error)
{
  dbus_int32_t d_x = x, d_y = y;
  dbus_uint16_t d_ctype = ctype;
  DBusMessage *reply;

  g_return_val_if_fail (obj != NULL, FALSE);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_component, "GetAccessibleAtPoint", error, "iin", d_x, d_y, d_ctype);

  return _atspi_dbus_return_accessible_from_message (reply);
}

/**
 * atspi_component_get_extents:
 * @obj: a pointer to the #AtspiComponent to query.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * Returns: A #AtspiRect giving the accessible's extents.
 *
 * Get the bounding box of the specified #AtspiComponent.
 *
 **/
AtspiRect *
atspi_component_get_extents (AtspiComponent *obj,
                                AtspiCoordType ctype, GError **error)
{
  dbus_uint32_t d_ctype = ctype;
  AtspiRect bbox;

  bbox.x = bbox.y = bbox.width = bbox.height = -1;
  g_return_val_if_fail (obj != NULL, atspi_rect_copy (&bbox));

  _atspi_dbus_call (obj, atspi_interface_component, "GetExtents", error, "u=>(iiii)", d_ctype, &bbox);
  return atspi_rect_copy (&bbox);
}

/**
 * atspi_component_get_position:
 * @obj: a pointer to the #AtspiComponent to query.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * returns: A #AtspiPoint giving the position.
 * Get the minimum x and y coordinates of the specified #AtspiComponent.
 *
 **/
AtspiPoint *
atspi_component_get_position (AtspiComponent *obj,
                                 AtspiCoordType ctype, GError **error)
{
  dbus_int32_t d_x, d_y;
  dbus_uint16_t d_ctype = ctype;
  AtspiPoint ret;

  ret.x = ret.y = -1;

  if (!obj)
    return atspi_point_copy (&ret);

  _atspi_dbus_call (obj, atspi_interface_component, "GetPosition", error, "u=>ii", d_ctype, &d_x, &d_y);

  ret.x = d_x;
  ret.y = d_y;
  return atspi_point_copy (&ret);
}

/**
 * atspi_component_get_size:
 * @obj: a pointer to the #AtspiComponent to query.
 * returns: A #AtspiPoint giving the size.
 *
 * Get the size of the specified #AtspiComponent.
 *
 **/
AtspiPoint *
atspi_component_get_size (AtspiComponent *obj, GError **error)
{
  dbus_int32_t d_w, d_h;
  AtspiPoint ret;

  ret.x = ret.y = -1;
  if (!obj)
    return atspi_point_copy (&ret);

  _atspi_dbus_call (obj, atspi_interface_component, "GetSize", error, "=>ii", &d_w, &d_h);
  ret.x = d_w;
  ret.y = d_h;
  return atspi_point_copy (&ret);
}

/**
 * atspi_component_get_layer:
 * @obj: a pointer to the #AtspiComponent to query.
 *
 * Query which layer the component is painted into, to help determine its 
 *      visibility in terms of stacking order.
 *
 * Returns: the #AtspiComponentLayer into which this component is painted.
 **/
AtspiComponentLayer
atspi_component_get_layer (AtspiComponent *obj, GError **error)
{
  dbus_uint32_t zlayer = -1;

  _atspi_dbus_call (obj, atspi_interface_component, "GetLayer", error, "=>u", &zlayer);

  return zlayer;
}

/**
 * atspi_component_get_mdi_z_order:
 * @obj: a pointer to the #AtspiComponent to query.
 *
 * Query the z stacking order of a component which is in the MDI or window
 *       layer. (Bigger z-order numbers mean nearer the top)
 *
 * Returns: a short integer indicating the stacking order of the component 
 *       in the MDI layer, or -1 if the component is not in the MDI layer.
 **/
gshort
atspi_component_get_mdi_z_order (AtspiComponent *obj, GError **error)
{
  dbus_uint16_t retval = -1;

  _atspi_dbus_call (obj, atspi_interface_component, "GetMDIZOrder", error, "=>n", &retval);

  return retval;
}

/**
 * atspi_component_grab_focus:
 * @obj: a pointer to the #AtspiComponent on which to operate.
 *
 * Attempt to set the keyboard input focus to the specified
 *         #AtspiComponent.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 *
 **/
gboolean
atspi_component_grab_focus (AtspiComponent *obj, GError **error)
{
  dbus_bool_t retval = FALSE;

  _atspi_dbus_call (obj, atspi_interface_component, "GrabFocus", error, "=>b", &retval);

  return retval;
}

/**
 * atspi_component_get_alpha:
 * @obj: The #AtspiComponent to be queried.
 *
 * Get the opacity/alpha value of a component, if alpha blending is in use.
 *
 * Returns: the opacity value of a component, as a double between 0.0 and 1.0. 
 **/
gdouble      
atspi_component_get_alpha    (AtspiComponent *obj, GError **error)
{
  double retval = 1;

  _atspi_dbus_call (obj, atspi_interface_component, "GetAlpha", error, "=>d", &retval);

  return retval;
}

static void
atspi_component_base_init (AtspiComponent *klass)
{
}

GType
atspi_component_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtspiComponent),
      (GBaseInitFunc) atspi_component_base_init,
      (GBaseFinalizeFunc) NULL,
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtspiComponent", &tinfo, 0);

  }
  return type;
}
