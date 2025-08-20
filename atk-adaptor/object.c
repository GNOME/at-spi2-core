/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008, 2009, 2010 Codethink Ltd.
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
 * This module contains utility functions for exporting AT-SPI
 * objects based upon an ATK object.
 *
 * It incudes functions for marshalling object references
 * and supported interfaces to a D-Bus message.
 */

#include "atspi/atspi.h"
#include "spi-dbus.h"
#include <atk/atk.h>

#include "accessible-cache.h"
#include "accessible-leasing.h"
#include "accessible-register.h"

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
void
spi_object_lease_if_needed (GObject *obj)
{
  if (!spi_cache_in (spi_global_cache, obj))
    {
      spi_leasing_take (spi_global_leasing, obj);
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
spi_object_append_null_reference (DBusMessageIter *iter)
{
  DBusMessageIter iter_struct;
  const char *name;
  const char *path = ATSPI_DBUS_PATH_NULL;

  name = dbus_bus_get_unique_name (spi_global_app_data->bus);

  dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (iter, &iter_struct);
}

void
spi_object_append_reference (DBusMessageIter *iter, AtkObject *obj)
{
  DBusMessageIter iter_struct;
  const gchar *name;
  gchar *path;

  if (!obj || !spi_global_app_data->bus)
    {
      spi_object_append_null_reference (iter);
      return;
    }

  spi_object_lease_if_needed (G_OBJECT (obj));

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

/* TODO: Perhaps combine with spi_object_append_reference.  Leaving separate
 * for now in case we want to use a different path for hyperlinks. */
void
spi_hyperlink_append_reference (DBusMessageIter *iter, AtkHyperlink *obj)
{
  DBusMessageIter iter_struct;
  const gchar *name;
  gchar *path;

  if (!obj)
    {
      spi_object_append_null_reference (iter);
      return;
    }

  spi_object_lease_if_needed (G_OBJECT (obj));

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
spi_object_append_v_reference (DBusMessageIter *iter, AtkObject *obj)
{
  DBusMessageIter iter_variant;

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, "(so)",
                                    &iter_variant);
  spi_object_append_reference (&iter_variant, obj);
  dbus_message_iter_close_container (iter, &iter_variant);
}

void
spi_object_append_desktop_reference (DBusMessageIter *iter)
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
spi_object_return_reference (DBusMessage *msg, AtkObject *obj)
{
  DBusMessage *reply;

  reply = dbus_message_new_method_return (msg);
  if (reply)
    {
      DBusMessageIter iter;
      dbus_message_iter_init_append (reply, &iter);
      spi_object_append_reference (&iter, obj);
    }

  return reply;
}

DBusMessage *
spi_hyperlink_return_reference (DBusMessage *msg, AtkHyperlink *obj)
{
  DBusMessage *reply;

  reply = dbus_message_new_method_return (msg);
  if (reply)
    {
      DBusMessageIter iter;
      dbus_message_iter_init_append (reply, &iter);
      spi_hyperlink_append_reference (&iter, obj);
    }
  if (obj)
    g_object_unref (G_OBJECT (obj));

  return reply;
}

/*---------------------------------------------------------------------------*/

void
spi_object_append_interfaces (DBusMessageIter *iter, AtkObject *obj)
{
  const gchar *itf;

  itf = ATSPI_DBUS_INTERFACE_ACCESSIBLE;
  dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);

  if (ATK_IS_ACTION (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_ACTION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (atk_object_get_role (obj) == ATK_ROLE_APPLICATION)
    {
      itf = ATSPI_DBUS_INTERFACE_APPLICATION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_COMPONENT (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_COMPONENT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_EDITABLE_TEXT (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_EDITABLE_TEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_TEXT (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_TEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_HYPERTEXT (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_HYPERTEXT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_IMAGE (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_IMAGE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_SELECTION (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_SELECTION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_TABLE (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_TABLE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_TABLE_CELL (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_TABLE_CELL;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_VALUE (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_VALUE;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

#if 0
  if (ATK_IS_STREAMABLE_CONTENT (obj))
    {
      itf = "org.a11y.atspi.StreamableContent";
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }
#endif

  if (ATK_IS_OBJECT (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_COLLECTION;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_DOCUMENT (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_DOCUMENT;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }

  if (ATK_IS_HYPERLINK_IMPL (obj))
    {
      itf = ATSPI_DBUS_INTERFACE_HYPERLINK;
      dbus_message_iter_append_basic (iter, DBUS_TYPE_STRING, &itf);
    }
}

/*---------------------------------------------------------------------------*/

void
spi_object_append_attribute_set (DBusMessageIter *iter, AtkAttributeSet *attr)
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
init_role_lookup_table (AtspiRole *role_table)
{
  int i;
  /* if it's not in the list below, dunno what it is */
  for (i = 0; i < ATK_ROLE_LAST_DEFINED; ++i)
    {
      role_table[i] = ATSPI_ROLE_UNKNOWN;
    }

  role_table[ATK_ROLE_INVALID] = ATSPI_ROLE_INVALID;
  role_table[ATK_ROLE_ACCEL_LABEL] = ATSPI_ROLE_ACCELERATOR_LABEL;
  role_table[ATK_ROLE_ALERT] = ATSPI_ROLE_ALERT;
  role_table[ATK_ROLE_ANIMATION] = ATSPI_ROLE_ANIMATION;
  role_table[ATK_ROLE_ARROW] = ATSPI_ROLE_ARROW;
  role_table[ATK_ROLE_CALENDAR] = ATSPI_ROLE_CALENDAR;
  role_table[ATK_ROLE_CANVAS] = ATSPI_ROLE_CANVAS;
  role_table[ATK_ROLE_CHECK_BOX] = ATSPI_ROLE_CHECK_BOX;
  role_table[ATK_ROLE_CHECK_MENU_ITEM] = ATSPI_ROLE_CHECK_MENU_ITEM;
  role_table[ATK_ROLE_COLOR_CHOOSER] = ATSPI_ROLE_COLOR_CHOOSER;
  role_table[ATK_ROLE_COLUMN_HEADER] = ATSPI_ROLE_COLUMN_HEADER;
  role_table[ATK_ROLE_COMBO_BOX] = ATSPI_ROLE_COMBO_BOX;
  role_table[ATK_ROLE_DATE_EDITOR] = ATSPI_ROLE_DATE_EDITOR;
  role_table[ATK_ROLE_DESKTOP_ICON] = ATSPI_ROLE_DESKTOP_ICON;
  role_table[ATK_ROLE_DESKTOP_FRAME] = ATSPI_ROLE_DESKTOP_FRAME;
  role_table[ATK_ROLE_DIAL] = ATSPI_ROLE_DIAL;
  role_table[ATK_ROLE_DIALOG] = ATSPI_ROLE_DIALOG;
  role_table[ATK_ROLE_DIRECTORY_PANE] = ATSPI_ROLE_DIRECTORY_PANE;
  role_table[ATK_ROLE_DRAWING_AREA] = ATSPI_ROLE_DRAWING_AREA;
  role_table[ATK_ROLE_FILE_CHOOSER] = ATSPI_ROLE_FILE_CHOOSER;
  role_table[ATK_ROLE_FILLER] = ATSPI_ROLE_FILLER;
  role_table[ATK_ROLE_FONT_CHOOSER] = ATSPI_ROLE_FONT_CHOOSER;
  role_table[ATK_ROLE_FRAME] = ATSPI_ROLE_FRAME;
  role_table[ATK_ROLE_GLASS_PANE] = ATSPI_ROLE_GLASS_PANE;
  role_table[ATK_ROLE_HTML_CONTAINER] = ATSPI_ROLE_HTML_CONTAINER;
  role_table[ATK_ROLE_ICON] = ATSPI_ROLE_ICON;
  role_table[ATK_ROLE_IMAGE] = ATSPI_ROLE_IMAGE;
  role_table[ATK_ROLE_INTERNAL_FRAME] = ATSPI_ROLE_INTERNAL_FRAME;
  role_table[ATK_ROLE_LABEL] = ATSPI_ROLE_LABEL;
  role_table[ATK_ROLE_LAYERED_PANE] = ATSPI_ROLE_LAYERED_PANE;
  role_table[ATK_ROLE_LIST] = ATSPI_ROLE_LIST;
  role_table[ATK_ROLE_LIST_ITEM] = ATSPI_ROLE_LIST_ITEM;
  role_table[ATK_ROLE_MENU] = ATSPI_ROLE_MENU;
  role_table[ATK_ROLE_MENU_BAR] = ATSPI_ROLE_MENU_BAR;
  role_table[ATK_ROLE_MENU_ITEM] = ATSPI_ROLE_MENU_ITEM;
  role_table[ATK_ROLE_OPTION_PANE] = ATSPI_ROLE_OPTION_PANE;
  role_table[ATK_ROLE_PAGE_TAB] = ATSPI_ROLE_PAGE_TAB;
  role_table[ATK_ROLE_PAGE_TAB_LIST] = ATSPI_ROLE_PAGE_TAB_LIST;
  role_table[ATK_ROLE_PANEL] = ATSPI_ROLE_PANEL;
  role_table[ATK_ROLE_PASSWORD_TEXT] = ATSPI_ROLE_PASSWORD_TEXT;
  role_table[ATK_ROLE_POPUP_MENU] = ATSPI_ROLE_POPUP_MENU;
  role_table[ATK_ROLE_PROGRESS_BAR] = ATSPI_ROLE_PROGRESS_BAR;
  role_table[ATK_ROLE_BUTTON] = ATSPI_ROLE_BUTTON;
  role_table[ATK_ROLE_RADIO_BUTTON] = ATSPI_ROLE_RADIO_BUTTON;
  role_table[ATK_ROLE_RADIO_MENU_ITEM] = ATSPI_ROLE_RADIO_MENU_ITEM;
  role_table[ATK_ROLE_ROOT_PANE] = ATSPI_ROLE_ROOT_PANE;
  role_table[ATK_ROLE_ROW_HEADER] = ATSPI_ROLE_ROW_HEADER;
  role_table[ATK_ROLE_SCROLL_BAR] = ATSPI_ROLE_SCROLL_BAR;
  role_table[ATK_ROLE_SCROLL_PANE] = ATSPI_ROLE_SCROLL_PANE;
  role_table[ATK_ROLE_SEPARATOR] = ATSPI_ROLE_SEPARATOR;
  role_table[ATK_ROLE_SLIDER] = ATSPI_ROLE_SLIDER;
  role_table[ATK_ROLE_SPIN_BUTTON] = ATSPI_ROLE_SPIN_BUTTON;
  role_table[ATK_ROLE_SPLIT_PANE] = ATSPI_ROLE_SPLIT_PANE;
  role_table[ATK_ROLE_STATUSBAR] = ATSPI_ROLE_STATUS_BAR;
  role_table[ATK_ROLE_TABLE] = ATSPI_ROLE_TABLE;
  role_table[ATK_ROLE_TABLE_CELL] = ATSPI_ROLE_TABLE_CELL;
  role_table[ATK_ROLE_TABLE_COLUMN_HEADER] =
      ATSPI_ROLE_TABLE_COLUMN_HEADER;
  role_table[ATK_ROLE_TABLE_ROW_HEADER] = ATSPI_ROLE_TABLE_ROW_HEADER;
  role_table[ATK_ROLE_TEAR_OFF_MENU_ITEM] =
      ATSPI_ROLE_TEAROFF_MENU_ITEM;
  role_table[ATK_ROLE_TERMINAL] = ATSPI_ROLE_TERMINAL;
  role_table[ATK_ROLE_TEXT] = ATSPI_ROLE_TEXT;
  role_table[ATK_ROLE_TOGGLE_BUTTON] = ATSPI_ROLE_TOGGLE_BUTTON;
  role_table[ATK_ROLE_TOOL_BAR] = ATSPI_ROLE_TOOL_BAR;
  role_table[ATK_ROLE_TOOL_TIP] = ATSPI_ROLE_TOOL_TIP;
  role_table[ATK_ROLE_TREE] = ATSPI_ROLE_TREE;
  role_table[ATK_ROLE_TREE_TABLE] = ATSPI_ROLE_TREE_TABLE;
  role_table[ATK_ROLE_UNKNOWN] = ATSPI_ROLE_UNKNOWN;
  role_table[ATK_ROLE_VIEWPORT] = ATSPI_ROLE_VIEWPORT;
  role_table[ATK_ROLE_WINDOW] = ATSPI_ROLE_WINDOW;
  role_table[ATK_ROLE_HEADER] = ATSPI_ROLE_HEADER;
  role_table[ATK_ROLE_FOOTER] = ATSPI_ROLE_FOOTER;
  role_table[ATK_ROLE_PARAGRAPH] = ATSPI_ROLE_PARAGRAPH;
  role_table[ATK_ROLE_RULER] = ATSPI_ROLE_RULER;
  role_table[ATK_ROLE_APPLICATION] = ATSPI_ROLE_APPLICATION;
  role_table[ATK_ROLE_AUTOCOMPLETE] = ATSPI_ROLE_AUTOCOMPLETE;
  role_table[ATK_ROLE_EDITBAR] = ATSPI_ROLE_EDITBAR;
  role_table[ATK_ROLE_EMBEDDED] = ATSPI_ROLE_EMBEDDED;
  role_table[ATK_ROLE_ENTRY] = ATSPI_ROLE_ENTRY;
  role_table[ATK_ROLE_CHART] = ATSPI_ROLE_CHART;
  role_table[ATK_ROLE_CAPTION] = ATSPI_ROLE_CAPTION;
  role_table[ATK_ROLE_DOCUMENT_FRAME] = ATSPI_ROLE_DOCUMENT_FRAME;
  role_table[ATK_ROLE_HEADING] = ATSPI_ROLE_HEADING;
  role_table[ATK_ROLE_PAGE] = ATSPI_ROLE_PAGE;
  role_table[ATK_ROLE_SECTION] = ATSPI_ROLE_SECTION;
  role_table[ATK_ROLE_FORM] = ATSPI_ROLE_FORM;
  role_table[ATK_ROLE_REDUNDANT_OBJECT] = ATSPI_ROLE_REDUNDANT_OBJECT;
  role_table[ATK_ROLE_LINK] = ATSPI_ROLE_LINK;
  role_table[ATK_ROLE_INPUT_METHOD_WINDOW] =
      ATSPI_ROLE_INPUT_METHOD_WINDOW;
  role_table[ATK_ROLE_TABLE_ROW] = ATSPI_ROLE_TABLE_ROW;
  role_table[ATK_ROLE_TREE_ITEM] = ATSPI_ROLE_TREE_ITEM;
  role_table[ATK_ROLE_DOCUMENT_SPREADSHEET] =
      ATSPI_ROLE_DOCUMENT_SPREADSHEET;
  role_table[ATK_ROLE_DOCUMENT_PRESENTATION] =
      ATSPI_ROLE_DOCUMENT_PRESENTATION;
  role_table[ATK_ROLE_DOCUMENT_TEXT] = ATSPI_ROLE_DOCUMENT_TEXT;
  role_table[ATK_ROLE_DOCUMENT_WEB] = ATSPI_ROLE_DOCUMENT_WEB;
  role_table[ATK_ROLE_DOCUMENT_EMAIL] = ATSPI_ROLE_DOCUMENT_EMAIL;
  role_table[ATK_ROLE_COMMENT] = ATSPI_ROLE_COMMENT;
  role_table[ATK_ROLE_LIST_BOX] = ATSPI_ROLE_LIST_BOX;
  role_table[ATK_ROLE_GROUPING] = ATSPI_ROLE_GROUPING;
  role_table[ATK_ROLE_IMAGE_MAP] = ATSPI_ROLE_IMAGE_MAP;
  role_table[ATK_ROLE_NOTIFICATION] = ATSPI_ROLE_NOTIFICATION;
  role_table[ATK_ROLE_INFO_BAR] = ATSPI_ROLE_INFO_BAR;
  role_table[ATK_ROLE_LEVEL_BAR] = ATSPI_ROLE_LEVEL_BAR;
  role_table[ATK_ROLE_TITLE_BAR] = ATSPI_ROLE_TITLE_BAR;
  role_table[ATK_ROLE_BLOCK_QUOTE] = ATSPI_ROLE_BLOCK_QUOTE;
  role_table[ATK_ROLE_AUDIO] = ATSPI_ROLE_AUDIO;
  role_table[ATK_ROLE_VIDEO] = ATSPI_ROLE_VIDEO;
  role_table[ATK_ROLE_DEFINITION] = ATSPI_ROLE_DEFINITION;
  role_table[ATK_ROLE_ARTICLE] = ATSPI_ROLE_ARTICLE;
  role_table[ATK_ROLE_LANDMARK] = ATSPI_ROLE_LANDMARK;
  role_table[ATK_ROLE_LOG] = ATSPI_ROLE_LOG;
  role_table[ATK_ROLE_MARQUEE] = ATSPI_ROLE_MARQUEE;
  role_table[ATK_ROLE_MATH] = ATSPI_ROLE_MATH;
  role_table[ATK_ROLE_RATING] = ATSPI_ROLE_RATING;
  role_table[ATK_ROLE_TIMER] = ATSPI_ROLE_TIMER;
  role_table[ATK_ROLE_STATIC] = ATSPI_ROLE_STATIC;
  role_table[ATK_ROLE_MATH_FRACTION] = ATSPI_ROLE_MATH_FRACTION;
  role_table[ATK_ROLE_MATH_ROOT] = ATSPI_ROLE_MATH_ROOT;
  role_table[ATK_ROLE_SUBSCRIPT] = ATSPI_ROLE_SUBSCRIPT;
  role_table[ATK_ROLE_SUPERSCRIPT] = ATSPI_ROLE_SUPERSCRIPT;
  role_table[ATK_ROLE_DESCRIPTION_LIST] = ATSPI_ROLE_DESCRIPTION_LIST;
  role_table[ATK_ROLE_DESCRIPTION_TERM] = ATSPI_ROLE_DESCRIPTION_TERM;
  role_table[ATK_ROLE_DESCRIPTION_VALUE] = ATSPI_ROLE_DESCRIPTION_VALUE;
  role_table[ATK_ROLE_FOOTNOTE] = ATSPI_ROLE_FOOTNOTE;
  role_table[ATK_ROLE_CONTENT_DELETION] = ATSPI_ROLE_CONTENT_DELETION;
  role_table[ATK_ROLE_CONTENT_INSERTION] = ATSPI_ROLE_CONTENT_INSERTION;
  role_table[ATK_ROLE_MARK] = ATSPI_ROLE_MARK;
  role_table[ATK_ROLE_SUGGESTION] = ATSPI_ROLE_SUGGESTION;
  role_table[ATK_ROLE_PUSH_BUTTON_MENU] = ATSPI_ROLE_PUSH_BUTTON_MENU;
  role_table[ATK_ROLE_SWITCH] = ATSPI_ROLE_SWITCH;

  return TRUE;
}

AtspiRole
spi_accessible_role_from_atk_role (AtkRole role)
{
  static gboolean is_initialized = FALSE;
  static AtspiRole spi_role_table[ATK_ROLE_LAST_DEFINED];
  AtspiRole spi_role;

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
      spi_role = ATSPI_ROLE_EXTENDED;
    }
  return spi_role;
}

/*END------------------------------------------------------------------------*/
