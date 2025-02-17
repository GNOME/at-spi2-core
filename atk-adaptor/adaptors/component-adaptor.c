/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#define ATK_DISABLE_DEPRECATION_WARNINGS
#include "bridge.h"
#include <atk/atk.h>
#include <droute/droute.h>
#include <string.h>

#include "spi-dbus.h"

#include "introspection.h"
#include "object.h"

static DBusMessage *
impl_Contains (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_int32_t x, y;
  dbus_uint32_t coord_type;
  dbus_bool_t retval;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
                              DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  retval =
      atk_component_contains (component, x, y, (AtkCoordType) coord_type);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &retval,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetAccessibleAtPoint (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_int32_t x, y;
  dbus_uint32_t coord_type;
  DBusMessage *reply;
  AtkObject *child;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
                              DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  child =
      atk_component_ref_accessible_at_point (component, x, y,
                                             (AtkCoordType) coord_type);
  reply = spi_object_return_reference (message, child);
  if (child)
    g_object_unref (child);

  return reply;
}

static DBusMessage *
impl_GetExtents (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_uint32_t coord_type;
  gint ix, iy, iwidth, iheight;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_component_get_extents (component, &ix, &iy, &iwidth, &iheight,
                             (AtkCoordType) coord_type);
  return spi_dbus_return_rect (message, ix, iy, iwidth, iheight);
}

static DBusMessage *
impl_GetPosition (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_uint32_t coord_type;
  gint ix = 0, iy = 0;
  dbus_int32_t x, y;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }
  atk_component_get_position (component, &ix, &iy, (AtkCoordType) coord_type);
  x = ix;
  y = iy;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32,
                                &y, DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetSize (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  gint iwidth = 0, iheight = 0;
  dbus_int32_t width, height;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  atk_component_get_size (component, &iwidth, &iheight);
  width = iwidth;
  height = iheight;
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT32, &width,
                                DBUS_TYPE_INT32, &height, DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetLayer (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  AtkLayer atklayer;
  dbus_uint32_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  atklayer = atk_component_get_layer (component);

  switch (atklayer)
    {
    case ATK_LAYER_BACKGROUND:
      rv = ATSPI_LAYER_BACKGROUND;
      break;
    case ATK_LAYER_CANVAS:
      rv = ATSPI_LAYER_CANVAS;
      break;
    case ATK_LAYER_WIDGET:
      rv = ATSPI_LAYER_WIDGET;
      break;
    case ATK_LAYER_MDI:
      rv = ATSPI_LAYER_MDI;
      break;
    case ATK_LAYER_POPUP:
      rv = ATSPI_LAYER_POPUP;
      break;
    case ATK_LAYER_OVERLAY:
      rv = ATSPI_LAYER_OVERLAY;
      break;
    case ATK_LAYER_WINDOW:
      rv = ATSPI_LAYER_WINDOW;
      break;
    default:
      rv = ATSPI_LAYER_INVALID;
      break;
    }
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_UINT32, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetMDIZOrder (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_int16_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  rv = atk_component_get_mdi_zorder (component);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_INT16, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GrabFocus (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_bool_t rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  rv = atk_component_grab_focus (component);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_GetAlpha (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  double rv;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  rv = atk_component_get_alpha (component);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_DOUBLE, &rv,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_SetExtents (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  DBusMessageIter iter, iter_struct;
  dbus_uint32_t coord_type;
  dbus_int32_t x, y, width, height;
  dbus_bool_t ret;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (strcmp (dbus_message_get_signature (message), "(iiii)u") != 0)
    {
      return droute_invalid_arguments_error (message);
    }

  dbus_message_iter_init (message, &iter);
  dbus_message_iter_recurse (&iter, &iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &x);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &y);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &width);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_get_basic (&iter_struct, &height);
  dbus_message_iter_next (&iter_struct);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &coord_type);

  ret = atk_component_set_extents (component, x, y, width, height, coord_type);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }

  return reply;
}

static DBusMessage *
impl_SetPosition (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_uint32_t coord_type;
  dbus_int32_t x, y;
  dbus_bool_t ret;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
                              DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  ret = atk_component_set_position (component, x, y, coord_type);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }

  return reply;
}

static dbus_bool_t
impl_get_ScreenExtents (DBusMessageIter *iter, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  DBusMessageIter iter_variant, iter_struct;
  gint ix = -1, iy = -1, iwidth = -1, iheight = -1;
  dbus_uint32_t x, y, width, height;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data), FALSE);

  atk_component_get_extents (component, &ix, &iy, &iwidth, &iheight, ATK_XY_SCREEN);
  x = ix;
  y = iy;
  width = iwidth;
  height = iheight;
  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(uuuu)",
                                    &iter_variant);
  dbus_message_iter_open_container (&iter_variant, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &x);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &y);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &width);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &height);
  dbus_message_iter_close_container (&iter_variant, &iter_struct);
  dbus_message_iter_close_container (iter, &iter_variant);
  return TRUE;
}

static DBusMessage *
impl_SetSize (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_int32_t width, height;
  dbus_bool_t ret;
  DBusMessage *reply;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_INT32, &width, DBUS_TYPE_INT32, &height,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  ret = atk_component_set_size (component, width, height);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }

  return reply;
}

static DBusMessage *
impl_ScrollTo (DBusConnection *bus,
               DBusMessage *message,
               void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_uint32_t type;
  dbus_bool_t ret;
  DBusMessage *reply = NULL;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_UINT32, &type, DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  ret = atk_component_scroll_to (component, type);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_ScrollToPoint (DBusConnection *bus,
                    DBusMessage *message,
                    void *user_data)
{
  AtkComponent *component = (AtkComponent *) user_data;
  dbus_uint32_t type;
  dbus_int32_t x, y;
  dbus_bool_t ret;
  DBusMessage *reply = NULL;

  g_return_val_if_fail (ATK_IS_COMPONENT (user_data),
                        droute_not_yet_handled_error (message));

  if (!dbus_message_get_args (message, NULL, DBUS_TYPE_UINT32, &type,
                              DBUS_TYPE_INT32, &x,
                              DBUS_TYPE_INT32, &y,
                              DBUS_TYPE_INVALID))
    {
      return droute_invalid_arguments_error (message);
    }

  ret = atk_component_scroll_to_point (component, type, x, y);

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret,
                                DBUS_TYPE_INVALID);
    }
  return reply;
}

static dbus_bool_t
impl_get_Version (DBusMessageIter *iter, void *user_data)
{
  return droute_return_v_uint32 (iter, SPI_DBUS_COMPONENT_VERSION);
}

static DRouteMethod methods[] = {
  { impl_Contains, "Contains" },
  { impl_GetAccessibleAtPoint, "GetAccessibleAtPoint" },
  { impl_GetExtents, "GetExtents" },
  { impl_GetPosition, "GetPosition" },
  { impl_GetSize, "GetSize" },
  { impl_GetLayer, "GetLayer" },
  { impl_GetMDIZOrder, "GetMDIZOrder" },
  { impl_GrabFocus, "GrabFocus" },
  { impl_GetAlpha, "GetAlpha" },
  { impl_SetExtents, "SetExtents" },
  { impl_SetPosition, "SetPosition" },
  { impl_SetSize, "SetSize" },
  { impl_ScrollTo, "ScrollTo" },
  { impl_ScrollToPoint, "ScrollToPoint" },
  { NULL, NULL }
};

static DRouteProperty properties[] = {
  { impl_get_ScreenExtents, NULL, "ScreenExtents" },
  { impl_get_Version, NULL, "version" },
  { NULL, NULL, NULL }
};
void
spi_initialize_component (DRoutePath *path)
{
  spi_atk_add_interface (path,
                         ATSPI_DBUS_INTERFACE_COMPONENT, spi_org_a11y_atspi_Component, methods, properties);
};
