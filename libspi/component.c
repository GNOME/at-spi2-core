/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#include "accessible.h"

static AtkComponent *
get_component (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_COMPONENT (obj);
}

static AtkComponent *
get_component_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || !ATK_IS_COMPONENT(obj))
    return NULL;
  return ATK_COMPONENT (obj);
}

static DBusMessage *
impl_contains (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkComponent *component = get_component (message);
  dbus_int32_t x, y;
  dbus_uint32_t coord_type;
  DBusError error;
  dbus_bool_t retval;
  DBusMessage *reply;

  if (!component)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &x, DBUS_TYPE_UINT32, &y,
       DBUS_TYPE_INT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getAccessibleAtPoint (DBusConnection * bus, DBusMessage * message,
			   void *user_data)
{
  AtkComponent *component = get_component (message);
  dbus_int32_t x, y;
  dbus_uint32_t coord_type;
  DBusError error;
  AtkObject *child;

  if (!component)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y,
       DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  child =
    atk_component_ref_accessible_at_point (component, x, y,
					   (AtkCoordType) coord_type);
  return spi_dbus_return_object (message, child, TRUE);
}

static DBusMessage *
impl_getExtents (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkComponent *component = get_component (message);
  DBusError error;
  dbus_uint32_t coord_type;
  gint ix, iy, iwidth, iheight;

  if (!component)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_component_get_extents (component, &ix, &iy, &iwidth, &iheight,
			     (AtkCoordType) coord_type);
  return spi_dbus_return_rect (message, ix, iy, iwidth, iheight);
}

static DBusMessage *
impl_getPosition (DBusConnection * bus, DBusMessage * message,
		  void *user_data)
{
  AtkComponent *component = get_component (message);
  DBusError error;
  dbus_uint32_t coord_type;
  gint ix = 0, iy = 0;
  dbus_int32_t x, y;
  DBusMessage *reply;

  if (!component)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_UINT32, &coord_type, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
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
impl_getSize (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkComponent *component = get_component (message);
  gint iwidth = 0, iheight = 0;
  dbus_int32_t width, height;
  DBusMessage *reply;

  if (!component)
    return spi_dbus_general_error (message);
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
impl_getLayer (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkComponent *component = get_component (message);
  AtkLayer atklayer;
  dbus_uint32_t rv;
  DBusMessage *reply;

  if (!component)
    return spi_dbus_general_error (message);
  atklayer = atk_component_get_layer (component);

  switch (atklayer)
    {
    case ATK_LAYER_BACKGROUND:
      rv = Accessibility_LAYER_BACKGROUND;
      break;
    case ATK_LAYER_CANVAS:
      rv = Accessibility_LAYER_CANVAS;
      break;
    case ATK_LAYER_WIDGET:
      rv = Accessibility_LAYER_WIDGET;
      break;
    case ATK_LAYER_MDI:
      rv = Accessibility_LAYER_MDI;
      break;
    case ATK_LAYER_POPUP:
      rv = Accessibility_LAYER_POPUP;
      break;
    case ATK_LAYER_OVERLAY:
      rv = Accessibility_LAYER_OVERLAY;
      break;
    case ATK_LAYER_WINDOW:
      rv = Accessibility_LAYER_WINDOW;
      break;
    default:
      rv = Accessibility_LAYER_INVALID;
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
impl_getMDIZOrder (DBusConnection * bus, DBusMessage * message,
		   void *user_data)
{
  AtkComponent *component = get_component (message);
  dbus_int16_t rv;
  DBusMessage *reply;

  if (!component)
    return spi_dbus_general_error (message);
  rv = atk_component_get_mdi_zorder (component);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_UINT32, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_grabFocus (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkComponent *component = get_component (message);
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!component)
    return spi_dbus_general_error (message);
  rv = atk_component_grab_focus (component);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_UINT32, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

#if 0
static DBusMessage *
impl_registerFocusHandler (DBusConnection * bus, DBusMessage * message,
			   void *user_data)
{
}

static DBusMessage *
impl_deregisterFocusHandler (DBusConnection * bus, DBusMessage * message,
			     void *user_data)
{
}
#endif

static DBusMessage *
impl_getAlpha (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkComponent *component = get_component (message);
  double rv;
  DBusMessage *reply;

  if (!component)
    return spi_dbus_general_error (message);
  rv = atk_component_get_alpha (component);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_DOUBLE, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  {impl_contains, "contains"},
  {impl_getAccessibleAtPoint, "getAccessibleAtPoint"},
  {impl_getExtents, "getExtents"},
  {impl_getPosition, "getPosition"},
  {impl_getSize, "getSize"},
  {impl_getLayer, "getLayer"},
  {impl_getMDIZOrder, "getMDIZOrder"},
  {impl_grabFocus, "grabFocus"},
  //{impl_registerFocusHandler, "registerFocusHandler"},
  //{impl_deregisterFocusHandler, "deregisterFocusHandler"},
  {impl_getAlpha, "getAlpha"},
  {NULL, NULL}
};

void
spi_initialize_component (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Component",
			methods, NULL,
			(DRouteGetDatumFunction) get_component_from_path,
			NULL);
};
