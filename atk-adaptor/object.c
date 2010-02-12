/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008, 2009, 2010 Codethink Ltd.
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
 * This module contains utility functions for exporting AT-SPI
 * objects based upon an ATK object.
 *
 * It incudes functions for marshalling object references
 * and supported interfaces to a D-Bus message.
 */

#include <atk/atk.h>
#include <common/spi-types.h>
#include <common/spi-dbus.h>

#include "accessible-register.h"
#include "accessible-cache.h"
#include "accessible-leasing.h"

#include "bridge.h"

/*---------------------------------------------------------------------------*/

/*
 * This is the all important function that decides whether an object should
 * be leased or not.
 *
 * The choice of algorithm for this is somewhat vuage. We want ideally to lease
 * all atk objects that are not owned by their parent.
 *
 * The 'cache' object attempts to cache all objects that are owned by their
 * parent by traversing the tree of accessibles, ignoring the children of
 * manages-descendants and transient objects.
 *
 * This function will simply look for all the accessibles that the cache object
 * has not found and assume that they need to be leased.
 */
static void
maybe_lease (AtkObject *obj)
{
  if (!spi_cache_in (spi_global_cache, G_OBJECT (obj)))
    {
      spi_leasing_take (spi_global_leasing, G_OBJECT (obj));
    }
}

/*---------------------------------------------------------------------------*/

/*
 * It is assumed that all of these functions are returning an accessible
 * object to the client side.
 *
 * All of them will lease the AtkObject if it is deemed neccessary.
 */

void
spi_object_append_null_reference (DBusMessageIter * iter)
{
  DBusMessageIter iter_struct;
  const char *name;
  const char *path = SPI_DBUS_PATH_NULL;

  name = dbus_bus_get_unique_name (spi_global_app_data->bus);

  dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &iter_struct);
}

void
spi_object_append_reference (DBusMessageIter * iter, AtkObject * obj)
{
  DBusMessageIter iter_struct;
  const gchar *name;
  gchar *path;

  if (!obj) {
    spi_object_append_null_reference (iter);
    return;
  }

  maybe_lease (obj);

  name = dbus_bus_get_unique_name (spi_global_app_data->bus);
  path = spi_register_object_to_path (spi_global_register, G_OBJECT (obj));

  if (!path)
    path = g_strdup (SPI_DBUS_PATH_NULL);

  dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &iter_struct);
  
  g_free (path);
}

void
spi_object_append_v_reference (DBusMessageIter * iter, AtkObject * obj)
{
  DBusMessageIter iter_variant;

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(so)",
                                    &iter_variant);
     spi_object_append_reference (&iter_variant, obj);
  dbus_message_iter_close_container (iter, &iter_variant);
}

void
spi_object_append_desktop_reference (DBusMessageIter * iter)
{
  DBusMessageIter iter_struct;
  const char *name = spi_global_app_data->desktop_name;
  const char *path = spi_global_app_data->desktop_path;

  dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &iter_struct);
}

DBusMessage *
spi_object_return_reference (DBusMessage * msg, AtkObject * obj)
{
  DBusMessage *reply;

  reply = dbus_message_new_method_return (msg);
  if (reply)
    {
      DBusMessageIter iter;
      dbus_message_iter_init_append (reply, &iter);
      spi_object_append_reference (&iter, obj);
    }
  g_object_unref (G_OBJECT (obj));

  return reply;
}

/*---------------------------------------------------------------------------*/

void
spi_object_append_interfaces (DBusMessageIter * iter, AtkObject * obj)
{
  const gchar *itf;

  itf = SPI_DBUS_INTERFACE_ACCESSIBLE;
  dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);

  if (ATK_IS_ACTION (obj))
    {
      itf = SPI_DBUS_INTERFACE_ACTION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_COMPONENT (obj))
    {
      itf = SPI_DBUS_INTERFACE_COMPONENT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_EDITABLE_TEXT (obj))
    {
      itf = SPI_DBUS_INTERFACE_EDITABLE_TEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_TEXT (obj))
    {
      itf = SPI_DBUS_INTERFACE_TEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_HYPERTEXT (obj))
    {
      itf = SPI_DBUS_INTERFACE_HYPERTEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_IMAGE (obj))
    {
      itf = SPI_DBUS_INTERFACE_IMAGE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_SELECTION (obj))
    {
      itf = SPI_DBUS_INTERFACE_SELECTION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_TABLE (obj))
    {
      itf = SPI_DBUS_INTERFACE_TABLE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_VALUE (obj))
    {
      itf = SPI_DBUS_INTERFACE_VALUE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

#if 0
  if (ATK_IS_STREAMABLE_CONTENT (obj))
    {
      itf = "org.a11y.atspi.StreamableContent";
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }
#endif

  if (ATK_IS_DOCUMENT (obj))
    {
      itf = "org.a11y.atspi.Collection";
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
      itf = SPI_DBUS_INTERFACE_DOCUMENT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_HYPERLINK_IMPL (obj))
    {
      itf = SPI_DBUS_INTERFACE_HYPERLINK;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }
}

/*---------------------------------------------------------------------------*/

void
spi_object_append_attribute_set (DBusMessageIter * iter, AtkAttributeSet * attr)
{
  DBusMessageIter dictIter;

  dbus_message_iter_open_container (iter, DBUS_TYPE_ARRAY, "{ss}", &dictIter);
  while (attr)
    {
      DBusMessageIter dictEntryIter;
      AtkAttribute *attribute = (AtkAttribute *) attr->data;
      const char *key = attribute->name;
      const char *value = attribute->value;

      if (key == NULL)
	key = "";
      if (value == NULL)
        value = "";

      dbus_message_iter_open_container (&dictIter, DBUS_TYPE_DICT_ENTRY, NULL,
                                        &dictEntryIter);
      dbus_message_iter_append_basic (&dictEntryIter, DBUS_TYPE_STRING,
                                      &key);
      dbus_message_iter_append_basic (&dictEntryIter, DBUS_TYPE_STRING,
                                      &value);
      dbus_message_iter_close_container (&dictIter, &dictEntryIter);
      attr = g_slist_next (attr);
    }
  dbus_message_iter_close_container (iter, &dictIter);
}

/*---------------------------------------------------------------------------*/

static gboolean
init_role_lookup_table (Accessibility_Role * role_table)
{
  int i;
  /* if it's not in the list below, dunno what it is */
  for (i = 0; i < ATK_ROLE_LAST_DEFINED; ++i)
    {
      role_table[i] = Accessibility_ROLE_UNKNOWN;
    }

  role_table[ATK_ROLE_INVALID] = Accessibility_ROLE_INVALID;
  role_table[ATK_ROLE_ACCEL_LABEL] = Accessibility_ROLE_ACCELERATOR_LABEL;
  role_table[ATK_ROLE_ALERT] = Accessibility_ROLE_ALERT;
  role_table[ATK_ROLE_ANIMATION] = Accessibility_ROLE_ANIMATION;
  role_table[ATK_ROLE_ARROW] = Accessibility_ROLE_ARROW;
  role_table[ATK_ROLE_CALENDAR] = Accessibility_ROLE_CALENDAR;
  role_table[ATK_ROLE_CANVAS] = Accessibility_ROLE_CANVAS;
  role_table[ATK_ROLE_CHECK_BOX] = Accessibility_ROLE_CHECK_BOX;
  role_table[ATK_ROLE_CHECK_MENU_ITEM] = Accessibility_ROLE_CHECK_MENU_ITEM;
  role_table[ATK_ROLE_COLOR_CHOOSER] = Accessibility_ROLE_COLOR_CHOOSER;
  role_table[ATK_ROLE_COLUMN_HEADER] = Accessibility_ROLE_COLUMN_HEADER;
  role_table[ATK_ROLE_COMBO_BOX] = Accessibility_ROLE_COMBO_BOX;
  role_table[ATK_ROLE_DATE_EDITOR] = Accessibility_ROLE_DATE_EDITOR;
  role_table[ATK_ROLE_DESKTOP_ICON] = Accessibility_ROLE_DESKTOP_ICON;
  role_table[ATK_ROLE_DESKTOP_FRAME] = Accessibility_ROLE_DESKTOP_FRAME;
  role_table[ATK_ROLE_DIAL] = Accessibility_ROLE_DIAL;
  role_table[ATK_ROLE_DIALOG] = Accessibility_ROLE_DIALOG;
  role_table[ATK_ROLE_DIRECTORY_PANE] = Accessibility_ROLE_DIRECTORY_PANE;
  role_table[ATK_ROLE_DRAWING_AREA] = Accessibility_ROLE_DRAWING_AREA;
  role_table[ATK_ROLE_FILE_CHOOSER] = Accessibility_ROLE_FILE_CHOOSER;
  role_table[ATK_ROLE_FILLER] = Accessibility_ROLE_FILLER;
  role_table[ATK_ROLE_FONT_CHOOSER] = Accessibility_ROLE_FONT_CHOOSER;
  role_table[ATK_ROLE_FRAME] = Accessibility_ROLE_FRAME;
  role_table[ATK_ROLE_GLASS_PANE] = Accessibility_ROLE_GLASS_PANE;
  role_table[ATK_ROLE_HTML_CONTAINER] = Accessibility_ROLE_HTML_CONTAINER;
  role_table[ATK_ROLE_ICON] = Accessibility_ROLE_ICON;
  role_table[ATK_ROLE_IMAGE] = Accessibility_ROLE_IMAGE;
  role_table[ATK_ROLE_INTERNAL_FRAME] = Accessibility_ROLE_INTERNAL_FRAME;
  role_table[ATK_ROLE_LABEL] = Accessibility_ROLE_LABEL;
  role_table[ATK_ROLE_LAYERED_PANE] = Accessibility_ROLE_LAYERED_PANE;
  role_table[ATK_ROLE_LIST] = Accessibility_ROLE_LIST;
  role_table[ATK_ROLE_LIST_ITEM] = Accessibility_ROLE_LIST_ITEM;
  role_table[ATK_ROLE_MENU] = Accessibility_ROLE_MENU;
  role_table[ATK_ROLE_MENU_BAR] = Accessibility_ROLE_MENU_BAR;
  role_table[ATK_ROLE_MENU_ITEM] = Accessibility_ROLE_MENU_ITEM;
  role_table[ATK_ROLE_OPTION_PANE] = Accessibility_ROLE_OPTION_PANE;
  role_table[ATK_ROLE_PAGE_TAB] = Accessibility_ROLE_PAGE_TAB;
  role_table[ATK_ROLE_PAGE_TAB_LIST] = Accessibility_ROLE_PAGE_TAB_LIST;
  role_table[ATK_ROLE_PANEL] = Accessibility_ROLE_PANEL;
  role_table[ATK_ROLE_PASSWORD_TEXT] = Accessibility_ROLE_PASSWORD_TEXT;
  role_table[ATK_ROLE_POPUP_MENU] = Accessibility_ROLE_POPUP_MENU;
  role_table[ATK_ROLE_PROGRESS_BAR] = Accessibility_ROLE_PROGRESS_BAR;
  role_table[ATK_ROLE_PUSH_BUTTON] = Accessibility_ROLE_PUSH_BUTTON;
  role_table[ATK_ROLE_RADIO_BUTTON] = Accessibility_ROLE_RADIO_BUTTON;
  role_table[ATK_ROLE_RADIO_MENU_ITEM] = Accessibility_ROLE_RADIO_MENU_ITEM;
  role_table[ATK_ROLE_ROOT_PANE] = Accessibility_ROLE_ROOT_PANE;
  role_table[ATK_ROLE_ROW_HEADER] = Accessibility_ROLE_ROW_HEADER;
  role_table[ATK_ROLE_SCROLL_BAR] = Accessibility_ROLE_SCROLL_BAR;
  role_table[ATK_ROLE_SCROLL_PANE] = Accessibility_ROLE_SCROLL_PANE;
  role_table[ATK_ROLE_SEPARATOR] = Accessibility_ROLE_SEPARATOR;
  role_table[ATK_ROLE_SLIDER] = Accessibility_ROLE_SLIDER;
  role_table[ATK_ROLE_SPIN_BUTTON] = Accessibility_ROLE_SPIN_BUTTON;
  role_table[ATK_ROLE_SPLIT_PANE] = Accessibility_ROLE_SPLIT_PANE;
  role_table[ATK_ROLE_STATUSBAR] = Accessibility_ROLE_STATUS_BAR;
  role_table[ATK_ROLE_TABLE] = Accessibility_ROLE_TABLE;
  role_table[ATK_ROLE_TABLE_CELL] = Accessibility_ROLE_TABLE_CELL;
  role_table[ATK_ROLE_TABLE_COLUMN_HEADER] =
    Accessibility_ROLE_TABLE_COLUMN_HEADER;
  role_table[ATK_ROLE_TABLE_ROW_HEADER] = Accessibility_ROLE_TABLE_ROW_HEADER;
  role_table[ATK_ROLE_TEAR_OFF_MENU_ITEM] =
    Accessibility_ROLE_TEAROFF_MENU_ITEM;
  role_table[ATK_ROLE_TERMINAL] = Accessibility_ROLE_TERMINAL;
  role_table[ATK_ROLE_TEXT] = Accessibility_ROLE_TEXT;
  role_table[ATK_ROLE_TOGGLE_BUTTON] = Accessibility_ROLE_TOGGLE_BUTTON;
  role_table[ATK_ROLE_TOOL_BAR] = Accessibility_ROLE_TOOL_BAR;
  role_table[ATK_ROLE_TOOL_TIP] = Accessibility_ROLE_TOOL_TIP;
  role_table[ATK_ROLE_TREE] = Accessibility_ROLE_TREE;
  role_table[ATK_ROLE_TREE_TABLE] = Accessibility_ROLE_TREE_TABLE;
  role_table[ATK_ROLE_UNKNOWN] = Accessibility_ROLE_UNKNOWN;
  role_table[ATK_ROLE_VIEWPORT] = Accessibility_ROLE_VIEWPORT;
  role_table[ATK_ROLE_WINDOW] = Accessibility_ROLE_WINDOW;
  role_table[ATK_ROLE_HEADER] = Accessibility_ROLE_HEADER;
  role_table[ATK_ROLE_FOOTER] = Accessibility_ROLE_FOOTER;
  role_table[ATK_ROLE_PARAGRAPH] = Accessibility_ROLE_PARAGRAPH;
  role_table[ATK_ROLE_RULER] = Accessibility_ROLE_RULER;
  role_table[ATK_ROLE_APPLICATION] = Accessibility_ROLE_APPLICATION;
  role_table[ATK_ROLE_AUTOCOMPLETE] = Accessibility_ROLE_AUTOCOMPLETE;
  role_table[ATK_ROLE_EDITBAR] = Accessibility_ROLE_EDITBAR;
  role_table[ATK_ROLE_EMBEDDED] = Accessibility_ROLE_EMBEDDED;
  role_table[ATK_ROLE_ENTRY] = Accessibility_ROLE_ENTRY;
  role_table[ATK_ROLE_CHART] = Accessibility_ROLE_CHART;
  role_table[ATK_ROLE_CAPTION] = Accessibility_ROLE_CAPTION;
  role_table[ATK_ROLE_DOCUMENT_FRAME] = Accessibility_ROLE_DOCUMENT_FRAME;
  role_table[ATK_ROLE_HEADING] = Accessibility_ROLE_HEADING;
  role_table[ATK_ROLE_PAGE] = Accessibility_ROLE_PAGE;
  role_table[ATK_ROLE_SECTION] = Accessibility_ROLE_SECTION;
  role_table[ATK_ROLE_FORM] = Accessibility_ROLE_FORM;
  role_table[ATK_ROLE_REDUNDANT_OBJECT] = Accessibility_ROLE_REDUNDANT_OBJECT;
  role_table[ATK_ROLE_LINK] = Accessibility_ROLE_LINK;
  role_table[ATK_ROLE_INPUT_METHOD_WINDOW] =
    Accessibility_ROLE_INPUT_METHOD_WINDOW;
  return TRUE;
}

Accessibility_Role
spi_accessible_role_from_atk_role (AtkRole role)
{
  static gboolean is_initialized = FALSE;
  static Accessibility_Role spi_role_table[ATK_ROLE_LAST_DEFINED];
  Accessibility_Role spi_role;

  if (!is_initialized)
    {
      is_initialized = init_role_lookup_table (spi_role_table);
    }

  if (role >= 0 && role < ATK_ROLE_LAST_DEFINED)
    {
      spi_role = spi_role_table[role];
    }
  else
    {
      spi_role = Accessibility_ROLE_EXTENDED;
    }
  return spi_role;
}

/*END------------------------------------------------------------------------*/
