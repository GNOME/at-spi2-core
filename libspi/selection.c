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

static AtkSelection *
get_selection (DBusMessage * message)
{
  AtkObject *obj = spi_dbus_get_object (dbus_message_get_path (message));
  if (!obj)
    return NULL;
  return ATK_SELECTION (obj);
}

static AtkSelection *
get_selection_from_path (const char *path, void *user_data)
{
  AtkObject *obj = spi_dbus_get_object (path);
  if (!obj || ~ATK_IS_SELECTION(obj))
    return NULL;
  return ATK_SELECTION (obj);
}

static dbus_bool_t
impl_get_nSelectedChildren (const char *path, DBusMessageIter * iter,
			    void *user_data)
{
  AtkSelection *selection = get_selection_from_path (path, user_data);
  if (!selection)
    return FALSE;
  return droute_return_v_int32 (iter,
				atk_selection_get_selection_count
				(selection));
}

static char *
impl_get_nSelectedChildren_str (void *datum)
{
  g_assert (ATK_IS_HYPERLINK (datum));
  return g_strdup_printf ("%d",
			  atk_selection_get_selection_count ((AtkSelection *)
							     datum));
}

static DBusMessage *
impl_getSelectedChild (DBusConnection * bus, DBusMessage * message,
		       void *user_data)
{
  AtkSelection *selection = get_selection (message);
  DBusError error;
  dbus_int32_t selectedChildIndex;
  AtkObject *atk_object;

  if (!selection)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &selectedChildIndex,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  atk_object = atk_selection_ref_selection (selection, selectedChildIndex);
  return spi_dbus_return_object (message, atk_object, TRUE);
}

static DBusMessage *
impl_selectChild (DBusConnection * bus, DBusMessage * message,
		  void *user_data)
{
  AtkSelection *selection = get_selection (message);
  DBusError error;
  dbus_int32_t childIndex;
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!selection)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &childIndex, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  rv = atk_selection_add_selection (selection, childIndex);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_deselectSelectedChild (DBusConnection * bus, DBusMessage * message,
			    void *user_data)
{
  AtkSelection *selection = get_selection (message);
  DBusError error;
  dbus_int32_t selectedChildIndex;
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!selection)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &selectedChildIndex,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  rv = atk_selection_remove_selection (selection, selectedChildIndex);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_isChildSelected (DBusConnection * bus, DBusMessage * message,
		      void *user_data)
{
  AtkSelection *selection = get_selection (message);
  DBusError error;
  dbus_int32_t childIndex;
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!selection)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &childIndex, DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  rv = atk_selection_is_child_selected (selection, childIndex);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_selectAll (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkSelection *selection = get_selection (message);
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!selection)
    return spi_dbus_general_error (message);
  rv = atk_selection_select_all_selection (selection);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_clearSelection (DBusConnection * bus, DBusMessage * message,
		     void *user_data)
{
  AtkSelection *selection = get_selection (message);
  dbus_bool_t rv;
  DBusMessage *reply;

  if (!selection)
    return spi_dbus_general_error (message);
  rv = atk_selection_clear_selection (selection);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_deselectChild (DBusConnection * bus, DBusMessage * message,
		    void *user_data)
{
  AtkSelection *selection = get_selection (message);
  DBusError error;
  dbus_int32_t selectedChildIndex;
  dbus_bool_t rv = FALSE;
  gint i, nselected;
  DBusMessage *reply;

  if (!selection)
    return spi_dbus_general_error (message);
  dbus_error_init (&error);
  if (!dbus_message_get_args
      (message, &error, DBUS_TYPE_INT32, &selectedChildIndex,
       DBUS_TYPE_INVALID))
    {
      return SPI_DBUS_RETURN_ERROR (message, &error);
    }
  nselected = atk_selection_get_selection_count (selection);
  for (i = 0; i < nselected; ++i)
    {
      AtkObject *selected_obj = atk_selection_ref_selection (selection, i);
      if (atk_object_get_index_in_parent (selected_obj) == selectedChildIndex)
	{
	  g_object_unref (G_OBJECT (selected_obj));
	  rv = atk_selection_remove_selection (selection, i);
	  break;
	}
      g_object_unref (G_OBJECT (selected_obj));
    }
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DRouteMethod methods[] = {
  {impl_getSelectedChild, "getSelectedChild"},
  {impl_selectChild, "selectChild"},
  {impl_deselectSelectedChild, "deselectSelectedChild"},
  {impl_isChildSelected, "isChildSelected"},
  {impl_selectAll, "selectAll"},
  {impl_clearSelection, "clearSelection"},
  {impl_deselectChild, "deselectChild"},
  {NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_nSelectedChildren, NULL, "nSelectedChildren"},
  {NULL, NULL, NULL}
};

void
spi_initialize_selection (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.atspi.Selection",
			methods, properties,
			(DRouteGetDatumFunction) get_selection_from_path,
			NULL);
};
