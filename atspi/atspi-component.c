/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*
 *
 * AtspiComponent function implementations
 *
 */

#include "atspi-accessible-private.h"
#include "atspi-private.h"

/**
 * AtspiComponent
 *
 * An interface implemented by objects which have onscreen visual
 * representations.
 *
 * The Component interface is implemented by objects which occupy on-screen
 * space, e.g. objects which have onscreen visual representations. The methods
 * in Component allow clients to identify where the objects lie in the onscreen
 * coordinate system, their relative size, stacking order, and position. It
 * also provides a mechanism whereby keyboard focus may be transferred to
 * specific user interface elements programmatically.  This is a 2D API.
 * Coordinates of 3D objects are projected into the 2-dimensional screen view
 * for purposes of this interface.
 */

void
atspi_rect_free (AtspiRect *rect)
{
  g_free (rect);
}

AtspiRect *
atspi_rect_copy (AtspiRect *src)
{
  AtspiRect *dst = g_new (AtspiRect, 1);
  dst->x = src->x;
  dst->y = src->y;
  dst->height = src->height;
  dst->width = src->width;
  return dst;
}

G_DEFINE_BOXED_TYPE (AtspiRect, atspi_rect, atspi_rect_copy, atspi_rect_free)

AtspiPoint *
atspi_point_copy (AtspiPoint *src)
{
  AtspiPoint *dst = g_new (AtspiPoint, 1);
  dst->x = src->x;
  dst->y = src->y;
  return dst;
}

G_DEFINE_BOXED_TYPE (AtspiPoint, atspi_point, atspi_point_copy, g_free)

/**
 * atspi_component_contains:
 * @obj: a pointer to the #AtspiComponent to query.
 * @x: a #gint specifying the x coordinate in question.
 * @y: a #gint specifying the y coordinate in question.
 * @ctype: the desired coordinate system of the point (@x, @y)
 *         (e.g. CSPI_COORD_TYPE_WINDOW, CSPI_COORD_TYPE_SCREEN).
 *
 * Queries whether a given #AtspiComponent contains a particular point.
 *
 * Returns: #TRUE if the specified component contains the point (@x, @y),
 *          #FALSE otherwise.
 **/
gboolean
atspi_component_contains (AtspiComponent *obj,
                          gint x,
                          gint y,
                          AtspiCoordType ctype,
                          GError **error)
{
  dbus_bool_t retval = FALSE;
  dbus_int32_t d_x = x, d_y = y;
  dbus_uint32_t d_ctype = ctype;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_component, "Contains", error, "iiu=>b", d_x, d_y, d_ctype, &retval);

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
 * Gets the accessible child at a given coordinate within an #AtspiComponent.
 *
 * Returns: (nullable) (transfer full): a pointer to an
 *          #AtspiAccessible child of the specified component which
 *          contains the point (@x, @y), or NULL if no child contains
 *          the point.
 **/
AtspiAccessible *
atspi_component_get_accessible_at_point (AtspiComponent *obj,
                                         gint x,
                                         gint y,
                                         AtspiCoordType ctype,
                                         GError **error)
{
  dbus_int32_t d_x = x, d_y = y;
  dbus_uint32_t d_ctype = ctype;
  DBusMessage *reply;

  g_return_val_if_fail (obj != NULL, FALSE);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_component, "GetAccessibleAtPoint", error, "iiu", d_x, d_y, d_ctype);

  return _atspi_dbus_return_accessible_from_message (reply);
}

/**
 * atspi_component_get_extents:
 * @obj: a pointer to the #AtspiComponent to query.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * Gets the bounding box of the specified #AtspiComponent.
 * The returned values are meaningful only if the Component has both
 * STATE_VISIBLE and STATE_SHOWING.
 *
 * Returns: An #AtspiRect giving the accessible's extents.
 **/
AtspiRect *
atspi_component_get_extents (AtspiComponent *obj,
                             AtspiCoordType ctype,
                             GError **error)
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
 * Gets the minimum x and y coordinates of the specified #AtspiComponent.
 * The returned values are meaningful only if the Component has both
 * STATE_VISIBLE and STATE_SHOWING.
 *
 * returns: An #AtspiPoint giving the @obj's position.
 **/
AtspiPoint *
atspi_component_get_position (AtspiComponent *obj,
                              AtspiCoordType ctype,
                              GError **error)
{
  dbus_int32_t d_x = -1, d_y = -1;
  dbus_uint32_t d_ctype = ctype;
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
 *
 * Gets the size of the specified #AtspiComponent.
 * The returned values are meaningful only if the Component has both
 * STATE_VISIBLE and STATE_SHOWING.
 *
 * returns: An #AtspiPoint giving the @obj's size.
 **/
AtspiPoint *
atspi_component_get_size (AtspiComponent *obj, GError **error)
{
  dbus_int32_t d_w = -1, d_h = -1;
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
 * Queries which layer the component is painted into, to help determine its
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
 * Queries the z stacking order of a component which is in the MDI or window
 *       layer. (Bigger z-order numbers mean nearer the top)
 *
 * Returns: a #gshort indicating the stacking order of the component
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
 * Attempts to set the keyboard input focus to the specified
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
 * Gets the opacity/alpha value of a component, if alpha blending is in use.
 *
 * Returns: the opacity value of a component, as a #gdouble between 0.0 and 1.0.
 **/
gdouble
atspi_component_get_alpha (AtspiComponent *obj, GError **error)
{
  double retval = 1;

  _atspi_dbus_call (obj, atspi_interface_component, "GetAlpha", error, "=>d", &retval);

  return retval;
}

/**
 * atspi_component_set_extents:
 * @obj: a pointer to the #AtspiComponent to move.
 * @x: the new horizontal position to which the component should be moved.
 * @y: the new vertical position to which the component should be moved.
 * @width: the width to which the component should be resized.
 * @height: the height to which the component should be resized.
 * @ctype: the coordinate system in which the position is specified.
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * Moves and resizes the specified component.
 *
 * Returns: #TRUE if successful; #FALSE otherwise.
 **/
gboolean
atspi_component_set_extents (AtspiComponent *obj,
                             gint x,
                             gint y,
                             gint width,
                             gint height,
                             AtspiCoordType ctype,
                             GError **error)
{
  dbus_int32_t d_x = x, d_y = y, d_width = width, d_height = height;
  dbus_uint32_t d_ctype = ctype;
  DBusMessageIter iter, iter_struct;
  DBusMessage *message, *reply;
  dbus_bool_t retval = FALSE;
  AtspiAccessible *aobj = ATSPI_ACCESSIBLE (obj);

  g_return_val_if_fail (obj != NULL, FALSE);

  if (!aobj->parent.app || !aobj->parent.app->bus_name)
    {
      g_set_error_literal (error, ATSPI_ERROR, ATSPI_ERROR_APPLICATION_GONE,
                           _ ("The application no longer exists"));
      return FALSE;
    }

  message = dbus_message_new_method_call (aobj->parent.app->bus_name,
                                          aobj->parent.path,
                                          atspi_interface_component,
                                          "SetExtents");
  if (!message)
    return FALSE;

  dbus_message_iter_init_append (message, &iter);
  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_STRUCT, NULL, &iter_struct))
    {
      dbus_message_unref (message);
      return FALSE;
    }
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &d_x);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &d_y);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &d_width);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_INT32, &d_height);
  dbus_message_iter_close_container (&iter, &iter_struct);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &d_ctype);

  reply = _atspi_dbus_send_with_reply_and_block (message, error);
  dbus_message_get_args (reply, NULL, DBUS_TYPE_BOOLEAN, &retval,
                         DBUS_TYPE_INVALID);
  dbus_message_unref (reply);
  return retval;
}

/**
 * atspi_component_set_position:
 * @obj: a pointer to the #AtspiComponent to move.
 * @x: the new horizontal position to which the component should be moved.
 * @y: the new vertical position to which the component should be moved.
 * @ctype: the coordinate system in which the position is specified.
 *         (e.g. ATSPI_COORD_TYPE_WINDOW, ATSPI_COORD_TYPE_SCREEN).
 *
 * Moves the component to the specified position.
 *
 * Returns: #TRUE if successful; #FALSE otherwise.
 **/
gboolean
atspi_component_set_position (AtspiComponent *obj,
                              gint x,
                              gint y,
                              AtspiCoordType ctype,
                              GError **error)
{
  dbus_int32_t d_x = x, d_y = y;
  dbus_uint32_t d_ctype = ctype;
  dbus_bool_t ret = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_component, "SetPosition", error,
                    "iiu=>b", d_x, d_y, d_ctype, &ret);

  return ret;
}

/**
 * atspi_component_set_size:
 * @obj: a pointer to the #AtspiComponent to query.
 * @width: the width to which the component should be resized.
 * @height: the height to which the component should be resized.
 *
 * Resizes the specified component to the given pixel dimensions.
 *
 * Returns: #TRUE if successful; #FALSE otherwise.
 **/
gboolean
atspi_component_set_size (AtspiComponent *obj,
                          gint width,
                          gint height,
                          GError **error)
{
  dbus_int32_t d_width = width, d_height = height;
  dbus_bool_t ret = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_component, "SetSize", error, "ii=>b",
                    d_width, d_height, &ret);

  return ret;
}

/**
 * atspi_component_scroll_to:
 * @obj: a pointer to the #AtspiComponent object on which to operate.
 * @type: a #AtspiScrollType indicating where the object should be placed on the
 *        screen.
 *
 * Scrolls whatever container of the #AtspiComponent object so it becomes
 * visible on the screen.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 **/
gboolean
atspi_component_scroll_to (AtspiComponent *obj,
                           AtspiScrollType type,
                           GError **error)
{
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_component,
                    "ScrollTo", error, "u=>b", type, &retval);

  return retval;
}

/**
 * atspi_component_scroll_to_point:
 * @obj: a pointer to the #AtspiComponent object on which to operate.
 * @coords: a #AtspiCoordType indicating whether the coordinates are relative to
 *          the screen, to the window, or to the parent object.
 * @x: the x coordinate of the point to reach
 * @y: the y coordinate of the point to reach
 * @error: return location for a #GError
 *
 * Scrolls whatever container of the #AtspiComponent object so it becomes
 * visible on the screen at a given position.
 *
 * Returns: #TRUE if successful, #FALSE otherwise.
 **/
gboolean
atspi_component_scroll_to_point (AtspiComponent *obj,
                                 AtspiCoordType coords,
                                 gint x,
                                 gint y,
                                 GError **error)
{
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_component,
                    "ScrollToPoint", error, "uii=>b", coords, x, y, &retval);

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

  if (!type)
    {
      static const GTypeInfo tinfo = {
        sizeof (AtspiComponent),
        (GBaseInitFunc) atspi_component_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtspiComponent", &tinfo, 0);
    }
  return type;
}
