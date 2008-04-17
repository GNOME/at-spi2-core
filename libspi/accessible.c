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
#include "bitarray.h"

#define get_object(message) spi_dbus_get_object(dbus_message_get_path(message))

static AtkObject *
get_object_from_path (const char *path, void *user_data)
{
  return spi_dbus_get_object (path);
}

static dbus_bool_t
impl_get_name (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkObject *object = spi_dbus_get_object (path);
  if (!object)
    return FALSE;
  return droute_return_v_string (iter, atk_object_get_name (object));
}

static char *
impl_get_name_str (void *datum)
{
  g_assert (ATK_IS_OBJECT (datum));
  return g_strdup (atk_object_get_name ((AtkObject *) datum));
}

static dbus_bool_t
impl_set_name (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkObject *object = spi_dbus_get_object (path);
  const char *name = droute_get_v_string (iter);
  atk_object_set_name (object, name);
  return TRUE;
}

static dbus_bool_t
impl_get_description (const char *path, DBusMessageIter * iter,
		      void *user_data)
{
  AtkObject *object = spi_dbus_get_object (path);
  if (!object)
    return FALSE;
  return droute_return_v_string (iter, atk_object_get_description (object));
}

static char *
impl_get_description_str (void *datum)
{
  g_assert (ATK_IS_OBJECT (datum));
  return g_strdup (atk_object_get_description ((AtkObject *) datum));
}

static dbus_bool_t
impl_set_description (const char *path, DBusMessageIter * iter,
		      void *user_data)
{
  AtkObject *object = spi_dbus_get_object (path);
  const char *description = droute_get_v_string (iter);
  atk_object_set_description (object, description);
  return TRUE;
}

static dbus_bool_t
impl_get_parent (const char *path, DBusMessageIter * iter, void *user_data)
{
  AtkObject *object = spi_dbus_get_object (path);

  if (!object)
    return FALSE;
  return spi_dbus_return_v_object (iter, atk_object_get_parent (object),
				   FALSE);
}

static char *
impl_get_parent_str (void *datum)
{
  g_assert (ATK_IS_OBJECT (datum));
  return spi_dbus_get_path (atk_object_get_parent ((AtkObject *) datum));
}

static dbus_bool_t
impl_get_childCount (const char *path, DBusMessageIter * iter,
		     void *user_data)
{
  AtkObject *object = spi_dbus_get_object (path);

  if (!object)
    return FALSE;
  return droute_return_v_int32 (iter,
				atk_object_get_n_accessible_children
				(object));
}

static char *
impl_get_childCount_str (void *datum)
{
  int count;
  g_assert (ATK_IS_OBJECT (datum));
  count = atk_object_get_n_accessible_children ((AtkObject *) datum);
  return g_strdup_printf ("%d", count);
}

static DBusMessage *
impl_getChildren (DBusConnection * bus, DBusMessage * message,
		  void *user_data)
{
  AtkObject *object = get_object (message);
  gint i;
  gint count;
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  if (!object)
    return spi_dbus_general_error (message);
  count = atk_object_get_n_accessible_children (object);
  reply = dbus_message_new_method_return (message);
  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container
      (&iter, DBUS_TYPE_ARRAY, "o", &iter_array))
    goto oom;
  for (i = 0; i < count; i++)
    {
      AtkObject *child = atk_object_ref_accessible_child (object, i);
      char *path = spi_dbus_get_path (child);
      if (path)
	{
	  dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_OBJECT_PATH,
					  &path);
	  g_free (path);
	}
      if (child)
	g_object_unref (child);
    }
  if (!dbus_message_iter_close_container (&iter, &iter_array))
    goto oom;
  return reply;
oom:
  // TODO: handle out-of-memory
  return reply;
}

static DBusMessage *
impl_getIndexInParent (DBusConnection * bus, DBusMessage * message,
		       void *user_data)
{
  AtkObject *object = get_object (message);
  dbus_uint32_t rv;
  DBusMessage *reply;

  if (!object)
    return spi_dbus_general_error (message);
  rv = atk_object_get_index_in_parent (object);
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
impl_getRelationSet (DBusConnection * bus, DBusMessage * message,
		     void *user_data)
{
}
#endif

static gboolean
spi_init_role_lookup_table (Accessibility_Role * role_table)
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
      is_initialized = spi_init_role_lookup_table (spi_role_table);
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

static DBusMessage *
impl_getRole (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkObject *object = get_object (message);
  gint role;
  dbus_uint32_t rv;
  DBusMessage *reply;

  if (!object)
    return spi_dbus_general_error (message);
  role = atk_object_get_role (object);
  rv = spi_accessible_role_from_atk_role (role);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_UINT32, &rv,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static char *
impl_get_role_str (void *datum)
{
  g_assert (ATK_IS_OBJECT (datum));
  return g_strdup_printf ("%d",
			  spi_accessible_role_from_atk_role
			  (atk_object_get_role ((AtkObject *) datum)));
}

static DBusMessage *
impl_getRoleName (DBusConnection * bus, DBusMessage * message,
		  void *user_data)
{
  AtkObject *object = get_object (message);
  gint role;
  const char *role_name;
  DBusMessage *reply;

  if (!object)
    return spi_dbus_general_error (message);
  role = atk_object_get_role (object);
  role_name = atk_role_get_name (role);
  if (!role_name)
    role_name = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &role_name,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getLocalizedRoleName (DBusConnection * bus, DBusMessage * message,
			   void *user_data)
{
  AtkObject *object = get_object (message);
  gint role;
  const char *role_name;
  DBusMessage *reply;

  if (!object)
    return spi_dbus_general_error (message);
  role = atk_object_get_role (object);
  role_name = atk_role_get_localized_name (role);
  if (!role_name)
    role_name = "";
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &role_name,
				DBUS_TYPE_INVALID);
    }
  return reply;
}

static Accessibility_StateType *accessible_state_types = NULL;
static AtkStateType *atk_state_types = NULL;

static gboolean
spi_init_state_type_tables (void)
{
  gint i;

  if (accessible_state_types || atk_state_types)
    return FALSE;
  if (!accessible_state_types)
    accessible_state_types =
      g_new (Accessibility_StateType, ATK_STATE_LAST_DEFINED);
  if (!atk_state_types)
    atk_state_types = g_new (AtkStateType, Accessibility_STATE_LAST_DEFINED);
  g_return_val_if_fail (accessible_state_types, FALSE);
  g_return_val_if_fail (atk_state_types, FALSE);

  for (i = 0; i < Accessibility_STATE_LAST_DEFINED; i++)
    {
      atk_state_types[i] = ATK_STATE_INVALID;
    }

  for (i = 0; i < ATK_STATE_LAST_DEFINED; i++)
    {
      accessible_state_types[i] = Accessibility_STATE_INVALID;
    }

  accessible_state_types[ATK_STATE_ACTIVE] = Accessibility_STATE_ACTIVE;
  atk_state_types[Accessibility_STATE_ACTIVE] = ATK_STATE_ACTIVE;
  accessible_state_types[ATK_STATE_ARMED] = Accessibility_STATE_ARMED;
  atk_state_types[Accessibility_STATE_ARMED] = ATK_STATE_ARMED;
  accessible_state_types[ATK_STATE_BUSY] = Accessibility_STATE_BUSY;
  atk_state_types[Accessibility_STATE_BUSY] = ATK_STATE_BUSY;
  accessible_state_types[ATK_STATE_CHECKED] = Accessibility_STATE_CHECKED;
  atk_state_types[Accessibility_STATE_CHECKED] = ATK_STATE_CHECKED;
  accessible_state_types[ATK_STATE_DEFUNCT] = Accessibility_STATE_DEFUNCT;
  atk_state_types[Accessibility_STATE_DEFUNCT] = ATK_STATE_DEFUNCT;
  accessible_state_types[ATK_STATE_EDITABLE] = Accessibility_STATE_EDITABLE;
  atk_state_types[Accessibility_STATE_EDITABLE] = ATK_STATE_EDITABLE;
  accessible_state_types[ATK_STATE_ENABLED] = Accessibility_STATE_ENABLED;
  atk_state_types[Accessibility_STATE_ENABLED] = ATK_STATE_ENABLED;
  accessible_state_types[ATK_STATE_EXPANDABLE] =
    Accessibility_STATE_EXPANDABLE;
  atk_state_types[Accessibility_STATE_EXPANDABLE] = ATK_STATE_EXPANDABLE;
  accessible_state_types[ATK_STATE_EXPANDED] = Accessibility_STATE_EXPANDED;
  atk_state_types[Accessibility_STATE_EXPANDED] = ATK_STATE_EXPANDED;
  accessible_state_types[ATK_STATE_FOCUSABLE] = Accessibility_STATE_FOCUSABLE;
  atk_state_types[Accessibility_STATE_FOCUSABLE] = ATK_STATE_FOCUSABLE;
  accessible_state_types[ATK_STATE_FOCUSED] = Accessibility_STATE_FOCUSED;
  atk_state_types[Accessibility_STATE_FOCUSED] = ATK_STATE_FOCUSED;
  accessible_state_types[ATK_STATE_HORIZONTAL] =
    Accessibility_STATE_HORIZONTAL;
  atk_state_types[Accessibility_STATE_HORIZONTAL] = ATK_STATE_HORIZONTAL;
  accessible_state_types[ATK_STATE_ICONIFIED] = Accessibility_STATE_ICONIFIED;
  atk_state_types[Accessibility_STATE_ICONIFIED] = ATK_STATE_ICONIFIED;
  accessible_state_types[ATK_STATE_MODAL] = Accessibility_STATE_MODAL;
  atk_state_types[Accessibility_STATE_MODAL] = ATK_STATE_MODAL;
  accessible_state_types[ATK_STATE_MULTI_LINE] =
    Accessibility_STATE_MULTI_LINE;
  atk_state_types[Accessibility_STATE_MULTI_LINE] = ATK_STATE_MULTI_LINE;
  accessible_state_types[ATK_STATE_MULTISELECTABLE] =
    Accessibility_STATE_MULTISELECTABLE;
  atk_state_types[Accessibility_STATE_MULTISELECTABLE] =
    ATK_STATE_MULTISELECTABLE;
  accessible_state_types[ATK_STATE_OPAQUE] = Accessibility_STATE_OPAQUE;
  atk_state_types[Accessibility_STATE_OPAQUE] = ATK_STATE_OPAQUE;
  accessible_state_types[ATK_STATE_PRESSED] = Accessibility_STATE_PRESSED;
  atk_state_types[Accessibility_STATE_PRESSED] = ATK_STATE_PRESSED;
  accessible_state_types[ATK_STATE_RESIZABLE] = Accessibility_STATE_RESIZABLE;
  atk_state_types[Accessibility_STATE_RESIZABLE] = ATK_STATE_RESIZABLE;
  accessible_state_types[ATK_STATE_SELECTABLE] =
    Accessibility_STATE_SELECTABLE;
  atk_state_types[Accessibility_STATE_SELECTABLE] = ATK_STATE_SELECTABLE;
  accessible_state_types[ATK_STATE_SELECTED] = Accessibility_STATE_SELECTED;
  atk_state_types[Accessibility_STATE_SELECTED] = ATK_STATE_SELECTED;
  accessible_state_types[ATK_STATE_SENSITIVE] = Accessibility_STATE_SENSITIVE;
  atk_state_types[Accessibility_STATE_SENSITIVE] = ATK_STATE_SENSITIVE;
  accessible_state_types[ATK_STATE_SHOWING] = Accessibility_STATE_SHOWING;
  atk_state_types[Accessibility_STATE_SHOWING] = ATK_STATE_SHOWING;
  accessible_state_types[ATK_STATE_SINGLE_LINE] =
    Accessibility_STATE_SINGLE_LINE;
  atk_state_types[Accessibility_STATE_SINGLE_LINE] = ATK_STATE_SINGLE_LINE;
  accessible_state_types[ATK_STATE_STALE] = Accessibility_STATE_STALE;
  atk_state_types[Accessibility_STATE_STALE] = ATK_STATE_STALE;
  accessible_state_types[ATK_STATE_TRANSIENT] = Accessibility_STATE_TRANSIENT;
  atk_state_types[Accessibility_STATE_TRANSIENT] = ATK_STATE_TRANSIENT;
  accessible_state_types[ATK_STATE_VERTICAL] = Accessibility_STATE_VERTICAL;
  atk_state_types[Accessibility_STATE_VERTICAL] = ATK_STATE_VERTICAL;
  accessible_state_types[ATK_STATE_VISIBLE] = Accessibility_STATE_VISIBLE;
  atk_state_types[Accessibility_STATE_VISIBLE] = ATK_STATE_VISIBLE;
  accessible_state_types[ATK_STATE_MANAGES_DESCENDANTS] =
    Accessibility_STATE_MANAGES_DESCENDANTS;
  atk_state_types[Accessibility_STATE_MANAGES_DESCENDANTS] =
    ATK_STATE_MANAGES_DESCENDANTS;
  accessible_state_types[ATK_STATE_INDETERMINATE] =
    Accessibility_STATE_INDETERMINATE;
  atk_state_types[Accessibility_STATE_INDETERMINATE] =
    ATK_STATE_INDETERMINATE;
  accessible_state_types[ATK_STATE_TRUNCATED] = Accessibility_STATE_TRUNCATED;
  atk_state_types[Accessibility_STATE_TRUNCATED] = ATK_STATE_TRUNCATED;
  accessible_state_types[ATK_STATE_REQUIRED] = Accessibility_STATE_REQUIRED;
  atk_state_types[Accessibility_STATE_REQUIRED] = ATK_STATE_REQUIRED;
  accessible_state_types[ATK_STATE_INVALID_ENTRY] =
    Accessibility_STATE_INVALID_ENTRY;
  atk_state_types[Accessibility_STATE_INVALID_ENTRY] =
    ATK_STATE_INVALID_ENTRY;
  accessible_state_types[ATK_STATE_SUPPORTS_AUTOCOMPLETION] =
    Accessibility_STATE_SUPPORTS_AUTOCOMPLETION;
  atk_state_types[Accessibility_STATE_SUPPORTS_AUTOCOMPLETION] =
    ATK_STATE_SUPPORTS_AUTOCOMPLETION;
  accessible_state_types[ATK_STATE_SELECTABLE_TEXT] =
    Accessibility_STATE_SELECTABLE_TEXT;
  atk_state_types[Accessibility_STATE_SELECTABLE_TEXT] =
    ATK_STATE_SELECTABLE_TEXT;
  accessible_state_types[ATK_STATE_DEFAULT] = Accessibility_STATE_IS_DEFAULT;
  atk_state_types[Accessibility_STATE_IS_DEFAULT] = ATK_STATE_DEFAULT;
  accessible_state_types[ATK_STATE_VISITED] = Accessibility_STATE_VISITED;
  atk_state_types[Accessibility_STATE_VISITED] = ATK_STATE_VISITED;


  return TRUE;
}

static void
get_state (AtkObject * object, dbus_uint32_t * array)
{
  AtkStateSet *set = atk_object_ref_state_set (object);
  int i;

  array[0] = 0;
  array[1] = 0;
  if (!set)
    return;
  spi_init_state_type_tables ();

  g_assert (ATK_STATE_LAST_DEFINED <= 64);
  for (i = 0; i < ATK_STATE_LAST_DEFINED; i++)
    {
      if (atk_state_set_contains_state (set, i))
	{
	  int a = accessible_state_types[i];
	  g_assert (a < 64);
	  BITARRAY_SET (array, a);
	}
    }
  g_object_unref (set);
}

static DBusMessage *
impl_getState (DBusConnection * bus, DBusMessage * message, void *user_data)
{
  AtkObject *object = get_object (message);
  dbus_uint32_t rv[2];
  DBusMessage *reply;

  if (!object)
    return spi_dbus_general_error (message);
  get_state (object, rv);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_ARRAY, DBUS_TYPE_INT32, &rv,
				2, DBUS_TYPE_INVALID);
    }
  return reply;
}

static DBusMessage *
impl_getAttributes (DBusConnection * bus, DBusMessage * message,
		    void *user_data)
{
  AtkObject *object = get_object (message);
  DBusMessage *reply;
  AtkAttributeSet *attributes;
  AtkAttribute *attr = NULL;
  char **retval;
  gint n_attributes = 0;
  gint i;

  if (!object)
    return spi_dbus_general_error (message);

  attributes = atk_object_get_attributes (object);
  if (attributes)
    n_attributes = g_slist_length (attributes);

  retval = (char **) g_malloc (n_attributes * sizeof (char *));

  for (i = 0; i < n_attributes; ++i)
    {
      attr = g_slist_nth_data (attributes, i);
      retval[i] = g_strconcat (attr->name, ":", attr->value, NULL);
    }
  if (attributes)
    atk_attribute_set_free (attributes);
  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING,
				&retval, n_attributes, DBUS_TYPE_INVALID);
    }
  for (i = 0; i < n_attributes; i++)
    g_free (retval[i]);
  g_free (retval);
  return reply;
}

static DBusMessage *
impl_getApplication (DBusConnection * bus, DBusMessage * message,
		     void *user_data)
{
  AtkObject *root = atk_get_root ();
  return spi_dbus_return_object (message, root, FALSE);
}

static DRouteMethod methods[] = {
  //{ DROUTE_METHOD, impl_isEqual, "isEqual", "o,obj,i:b,,o" },
  {DROUTE_METHOD, impl_getChildren, "getChildren", "ao,,o"},
  {DROUTE_METHOD, impl_getIndexInParent, "getIndexInParent", "i,,o"},
  //{ DROUTE_METHOD, impl_getRelationSet, "getRelationSet", "a{uao},,o" },
  {DROUTE_METHOD, impl_getRole, "getRole", "u,,o"},
  {DROUTE_METHOD, impl_getRoleName, "getRoleName", "s,,o"},
  {DROUTE_METHOD, impl_getLocalizedRoleName, "getLocalizedRoleName", "s,,o"},
  {DROUTE_METHOD, impl_getState, "getState", "o,,au"},
  {DROUTE_METHOD, impl_getAttributes, "getAttributes", "as,,o"},
  {DROUTE_METHOD, impl_getApplication, "getApplication", "o,,o"},
  {0, NULL, NULL, NULL}
};

static DRouteProperty properties[] = {
  {impl_get_name, impl_get_name_str, impl_set_name, NULL, "name", "s"},
  {impl_get_description, impl_get_description_str, impl_set_description, NULL,
   "description", "s"},
  {impl_get_parent, impl_get_parent_str, NULL, NULL, "parent", "o"},
  //{ impl_get_childCount, impl_get_childCount_str, NULL, NULL, "childCount", "n" },
  {NULL, impl_get_role_str, NULL, NULL, "role", "u"},
  {NULL, NULL, NULL, NULL, NULL, NULL}
};

void
spi_initialize_accessible (DRouteData * data)
{
  droute_add_interface (data, "org.freedesktop.accessibility.Accessible",
			methods, properties,
			(DRouteGetDatumFunction) get_object_from_path, NULL);
};
