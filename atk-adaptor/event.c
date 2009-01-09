/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008, Codethink Ltd.
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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

#include <string.h>

#include <atk/atk.h>
#include <droute/droute.h>

#include "bridge.h"
#include "atk-dbus.h"

#include "spi-common/spi-dbus.h"

static GArray *listener_ids = NULL;

static gint atk_bridge_key_event_listener_id;
static gint atk_bridge_focus_tracker_id;

/* Amazingly the ATK event callbacks dont have a user
 * data parameter. Instead, with great sadness, we use
 * some global data. Data is declared and initialized
 * in bridge.c.
 */
extern SpiAppData *atk_adaptor_app_data;

/*---------------------------------------------------------------------------*/

#define ITF_EVENT_OBJECT   "org.freedesktop.atspi.Event.Object"
#define ITF_EVENT_WINDOW   "org.freedesktop.atspi.Event.Window"
#define ITF_EVENT_DOCUMENT "org.freedekstop.atspi.Event.Document"
#define ITF_EVENT_FOCUS    "org.freedesktop.atspi.Event.Focus"

/*---------------------------------------------------------------------------*/

/* When sending events it is safe to register an accessible object if
 * one does not already exist for a given AtkObject.
 * This is because the cache update signal should then be send before
 * the event signal is sent.
 */
static gchar *
get_object_path (AtkObject *accessible)
{
    guint ref;

    ref = atk_dbus_register_accessible (accessible);
    return atk_dbus_ref_to_path (ref);
}

/*---------------------------------------------------------------------------*/

static gboolean
Accessibility_DeviceEventController_notifyListenersSync(const Accessibility_DeviceEvent *key_event)
{
  DBusMessage *message;
  DBusError error;
  dbus_bool_t consumed = FALSE;

  message = 
  dbus_message_new_method_call(SPI_DBUS_NAME_REGISTRY, 
  			       SPI_DBUS_PATH_REGISTRY,
			       SPI_DBUS_INTERFACE_DEC,
			       "notifyListenersSync");

  dbus_error_init(&error);
  if (spi_dbus_marshal_deviceEvent(message, key_event))
  {
    DBusMessage *reply = dbus_connection_send_with_reply_and_block(atk_adaptor_app_data->bus, message, 1000, &error);
    if (reply)
    {
      DBusError error;
      dbus_error_init(&error);
      dbus_message_get_args(reply, &error, DBUS_TYPE_BOOLEAN, &consumed, DBUS_TYPE_INVALID);
      dbus_message_unref(reply);
    }
  }
  dbus_message_unref(message);
  return consumed;
}

static void
spi_init_keystroke_from_atk_key_event (Accessibility_DeviceEvent  *keystroke,
				       AtkKeyEventStruct          *event)
{
  keystroke->id        = (dbus_int32_t) event->keyval;
  keystroke->hw_code   = (dbus_int16_t) event->keycode;
  keystroke->timestamp = (dbus_uint32_t) event->timestamp;
  keystroke->modifiers = (dbus_uint16_t) (event->state & 0xFFFF);
  if (event->string)
    {
      gunichar c;

      keystroke->event_string = g_strdup (event->string);
      c = g_utf8_get_char_validated (event->string, -1);
      if (c > 0 && g_unichar_isprint (c))
        keystroke->is_text = TRUE;
      else
        keystroke->is_text = FALSE;
    }
  else
    {
      keystroke->event_string = g_strdup ("");
      keystroke->is_text = FALSE;
    }
  switch (event->type)
    {
    case (ATK_KEY_EVENT_PRESS):
      keystroke->type = Accessibility_KEY_PRESSED_EVENT;
      break;
    case (ATK_KEY_EVENT_RELEASE):
      keystroke->type = Accessibility_KEY_RELEASED_EVENT;
      break;
    default:
      keystroke->type = 0;
      break;
    }
#if 0  
  g_print ("key_event type %d; val=%d code=%d modifiers=%x name=%s is_text=%d, time=%lx\n",
	   (int) keystroke->type, (int) keystroke->id, (int) keystroke->hw_code,
	   (int) keystroke->modifiers,
	   keystroke->event_string, (int) keystroke->is_text, (unsigned long) keystroke->timestamp);
#endif
}


static gint
spi_atk_bridge_key_listener (AtkKeyEventStruct *event, gpointer data)
{
  gboolean             result;
  Accessibility_DeviceEvent key_event;

  spi_init_keystroke_from_atk_key_event (&key_event, event);

  result = Accessibility_DeviceEventController_notifyListenersSync (&key_event);

  if (key_event.event_string) g_free (key_event.event_string);

  return result;
}


/*---------------------------------------------------------------------------*/

/*
 * Emits an AT-SPI event.
 * AT-SPI events names are split into three parts:
 * class:major:minor
 * This is mapped onto D-Bus events as:
 * D-Bus Interface:Signal Name:Detail argument
 *
 * Marshals a basic type into the 'any_data' attribute of
 * the AT-SPI event.
 */

/*
 * This is a rather annoying function needed to replace
 * NULL values of strings with the empty string. Null string
 * values can be created by the atk_object_get_name or text selection
 */
static const void *
provide_defaults(const gint type,
		 const void *val)
{
  switch (type)
    {
      case DBUS_TYPE_STRING:
      case DBUS_TYPE_OBJECT_PATH:
	   if (!val)
	      return "";
	   else
	      return val;
      default:
	   return val;
    }
}

static void 
emit(AtkObject  *accessible,
     const char *klass,
     const char *major,
     const char *minor,
     dbus_int32_t detail1,
     dbus_int32_t detail2,
     const char *type,
     const void *val)
{
  DBusMessage *sig;
  DBusMessageIter iter, sub;
  gchar *path, *cname, *t;

  path = get_object_path (accessible);

  /* Tough decision here
   * We won't send events from accessible
   * objects that have not yet been added to the accessible tree.
   */
  if (path == NULL)
      return;

  if (!klass) klass = "";
  if (!major) major = "";
  if (!minor) minor = "";

  /*
   * This is very annoying, but as '-' isn't a legal signal
   * name in D-Bus (Why not??!?) The names need converting
   * on this side, and again on the client side.
   */
  cname = g_strdup(major);
  while ((t = strchr(cname, '-')) != NULL) *t = '_';

  sig = dbus_message_new_signal(path, klass, cname);
  g_free(cname);
  g_free(path);

  dbus_message_iter_init_append(sig, &iter);

  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &minor);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &detail1);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &detail2);

  dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, type, &sub);
  /*
   * I need to convert the string signature to an integer type signature.
   * DBUS_TYPE_INT32 is defined as 'i' whereas the string is "i".
   * I should just be able to cast the first character of the string to an
   * integer.
   */
  val = provide_defaults((int) *type, val);
  dbus_message_iter_append_basic(&sub, (int) *type, &val);
  dbus_message_iter_close_container(&iter, &sub);

  dbus_connection_send(atk_adaptor_app_data->bus, sig, NULL);
  dbus_message_unref(sig);
}

/*---------------------------------------------------------------------------*/

/*
 * Emits an AT-SPI event, marshalling a BoundingBox structure into the 
 * 'any_data' variant of the event.
 */
static void
emit_rect(AtkObject  *accessible,
	  const char *klass,
	  const char *major,
	  const char *minor,
	  AtkRectangle *rect)
{
  DBusMessage *sig;
  DBusMessageIter iter, variant, sub;
  gchar *path, *cname, *t;
  dbus_int32_t dummy = 0;

  path = get_object_path (accessible);

  /* Tough decision here
   * We won't send events from accessible
   * objects that have not yet been added to the accessible tree.
   */
  if (path == NULL)
      return;

  if (!klass) klass = "";
  if (!major) major = "";
  if (!minor) minor = "";

  /*
   * This is very annoying, but as '-' isn't a legal signal
   * name in D-Bus (Why not??!?) The names need converting
   * on this side, and again on the client side.
   */
  cname = g_strdup(major);
  while ((t = strchr(cname, '-')) != NULL) *t = '_';

  sig = dbus_message_new_signal(path, klass, cname);
  g_free(path);
  g_free(cname);

  dbus_message_iter_init_append (sig, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &minor);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &dummy);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &dummy);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "(iiii)", &variant);
    dbus_message_iter_open_container (&variant, DBUS_TYPE_STRUCT, NULL, &sub);
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->x));
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->y));
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->width));
      dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->height));
    dbus_message_iter_close_container (&variant, &sub);
  dbus_message_iter_close_container (&iter, &variant);

  dbus_connection_send(atk_adaptor_app_data->bus, sig, NULL);
}

/*---------------------------------------------------------------------------*/

/*
 * The tree update listener handles the following Atk signals:
 *
 * Gtk:AtkObject:property-change
 * 
 * With the folowing property names:
 *
 * accessible-name
 * accessible-description
 * accessible-parent
 *
 * It updates the server side accessible-object database, which
 * will then syncronize with the client-side accessible cache.
 * 
 */
static gboolean
tree_update_listener (GSignalInvocationHint *signal_hint,
	 	      guint                  n_param_values,
		      const GValue          *param_values,
		      gpointer               data)
{
  AtkObject *accessible;
  AtkPropertyValues *values;
  const gchar *pname = NULL;

  accessible = g_value_get_object (&param_values[0]);
  values = (AtkPropertyValues*) g_value_get_pointer (&param_values[1]);

  pname = values[0].property_name;

  if (strcmp (pname, "accessible-name") == 0 ||
      strcmp (pname, "accessible-description") == 0 ||
      strcmp (pname, "accessible-parent") == 0)
    {
      atk_dbus_update_accessible (accessible);
    }
  return TRUE;
}

/* 
 * Handles the ATK signal 'Gtk:AtkObject:children-changed'.
 *
 * It updates the server side accessible-object database, which
 * will then syncronize with the client-side accessible cache.
 *
 */
static gboolean
tree_update_children_listener (GSignalInvocationHint *signal_hint,
                               guint                  n_param_values,
                               const GValue          *param_values,
                               gpointer               data)
{
  AtkObject *accessible;
  const gchar *detail = NULL;
  AtkObject *child;
  gboolean child_needs_unref = FALSE;

  if (signal_hint->detail)
    detail = g_quark_to_string (signal_hint->detail);

  accessible = g_value_get_object (&param_values[0]);
  if (!strcmp (detail, "add"))
    {
      gpointer child;
      int index = g_value_get_uint (param_values + 1);
      child = g_value_get_pointer (param_values + 2);

      if (ATK_IS_OBJECT (child))
          g_object_ref (child);
      else
          child = atk_object_ref_accessible_child (accessible, index);

      atk_dbus_register_accessible (child);
      g_object_unref (child);
    }
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * The focus listener handles the ATK 'focus' signal and forwards it
 * as the AT-SPI event, 'focus:'
 */
static void
focus_tracker (AtkObject *accessible)
{
  emit(accessible, ITF_EVENT_FOCUS, "focus", "", 0, 0, DBUS_TYPE_INT32_AS_STRING, 0);
}

/*---------------------------------------------------------------------------*/

#define PCHANGE "property-change"

/* 
 * This handler handles the following ATK signals and
 * converts them to AT-SPI events:
 *  
 * Gtk:AtkObject:property-change -> object:property-change:(property-name)
 *
 * The property-name is part of the ATK property-change signal.
 */
static gboolean
property_event_listener (GSignalInvocationHint *signal_hint,
			 guint                  n_param_values,
			 const GValue          *param_values,
			 gpointer               data)
{
  AtkObject *accessible;
  AtkPropertyValues *values;

  const gchar *pname = NULL;

  AtkObject *otemp;
  const gchar *stemp;
  gint i;
  
  accessible = g_value_get_object (&param_values[0]);
  values = (AtkPropertyValues*) g_value_get_pointer (&param_values[1]);

  pname = values[0].property_name;

  /* TODO Could improve this control statement by matching
   * on only the end of the signal names,
   */
  if (strcmp (pname, "accessible-table-summary") == 0)
    {
      otemp = atk_table_get_summary(ATK_TABLE (accessible));
      stemp = get_object_path (otemp);
      emit(accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0, DBUS_TYPE_OBJECT_PATH_AS_STRING, stemp);
    }
  else if (strcmp (pname, "accessible-table-column-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      otemp = atk_table_get_column_header(ATK_TABLE (accessible), i);
      stemp = get_object_path (otemp);
      emit(accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0, DBUS_TYPE_OBJECT_PATH_AS_STRING, stemp);
    }
  else if (strcmp (pname, "accessible-table-row-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      otemp = atk_table_get_row_header(ATK_TABLE (accessible), i);
      stemp = get_object_path (otemp);
      emit(accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0, DBUS_TYPE_OBJECT_PATH_AS_STRING, stemp);
    }
  else if (strcmp (pname, "accessible-table-row-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      stemp = atk_table_get_row_description(ATK_TABLE (accessible), i);
      emit(accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0, DBUS_TYPE_STRING_AS_STRING, stemp);
    }
  else if (strcmp (pname, "accessible-table-column-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      stemp = atk_table_get_column_description(ATK_TABLE (accessible), i);
      emit(accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0, DBUS_TYPE_STRING_AS_STRING, stemp);
    }
  else if (strcmp (pname, "accessible-table-caption-object") == 0)
    {
      otemp = atk_table_get_caption(ATK_TABLE(accessible));
      stemp = atk_object_get_name(otemp);
      emit(accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0, DBUS_TYPE_STRING_AS_STRING, stemp);
    }
  else
    {
      emit(accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0, DBUS_TYPE_INT32_AS_STRING, 0);
    }
  return TRUE;
}

/*---------------------------------------------------------------------------*/

#define STATE_CHANGED "state-changed"

/*
 * The state event listener handles 'Gtk:AtkObject:state-change' ATK signals
 * and forwards them as object:state-changed:(param-name) AT-SPI events. Where
 * the param-name is part of the ATK state-change signal.
 */
static gboolean
state_event_listener (GSignalInvocationHint *signal_hint,
		      guint n_param_values,
		      const GValue *param_values,
		      gpointer data)
{
  AtkObject *accessible;
  gchar *pname;
  guint detail1;

  accessible = ATK_OBJECT(g_value_get_object (&param_values[0]));
  pname = g_strdup (g_value_get_string (&param_values[1]));

  /* TODO - Possibly ignore a change to the 'defunct' state.
   * This is because without reference counting defunct objects should be removed.
   */
  detail1 = (g_value_get_boolean (&param_values[2])) ? 1 : 0;
  emit(accessible, ITF_EVENT_OBJECT, STATE_CHANGED, pname, detail1, 0, DBUS_TYPE_INT32_AS_STRING, 0);
  g_free (pname);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * The window event listener handles the following ATK signals and forwards
 * them as AT-SPI events:
 *
 * window:create     -> window:create
 * window:destroy    -> window:destroy
 * window:minimize   -> window:minimize
 * window:maximize   -> window:maximize
 * window:activate   -> window:activate
 * window:deactivate -> window:deactivate
 */
static gboolean
window_event_listener (GSignalInvocationHint *signal_hint,
		       guint n_param_values,
		       const GValue *param_values,
		       gpointer data)
{
  AtkObject *accessible;
  GSignalQuery signal_query;
  const gchar *name, *s;
  
  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));
  s = atk_object_get_name (accessible);
  emit(accessible, ITF_EVENT_WINDOW, name, "", 0, 0, DBUS_TYPE_STRING_AS_STRING, s);

  return TRUE;
}

/*---------------------------------------------------------------------------*/

/* 
 * The document event listener handles the following ATK signals
 * and converts them to AT-SPI events:
 *
 * Gtk:AtkDocument:load-complete ->  document:load-complete
 * Gtk:AtkDocument:load-stopped  ->  document:load-stopped
 * Gtk:AtkDocument:reload        ->  document:reload
 */
static gboolean
document_event_listener (GSignalInvocationHint *signal_hint,
                         guint n_param_values,
                         const GValue *param_values,
                         gpointer data)
{
  AtkObject *accessible;
  GSignalQuery signal_query;
  const gchar *name, *s;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));
  s = atk_object_get_name (accessible);
  emit(accessible, ITF_EVENT_DOCUMENT, name, "", 0, 0, DBUS_TYPE_STRING_AS_STRING, s);

  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * Signal handler for  "Gtk:AtkComponent:bounds-changed". Converts
 * this to an AT-SPI event - "object:bounds-changed".
 */
static gboolean
bounds_event_listener (GSignalInvocationHint *signal_hint,
                       guint n_param_values,
                       const GValue *param_values,
                       gpointer data)
{
  AtkObject *accessible;
  AtkRectangle *atk_rect;
  GSignalQuery signal_query;
  const gchar *name, *s;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));

  if (G_VALUE_HOLDS_BOXED (param_values + 1))
    atk_rect = g_value_get_boxed (param_values + 1);

  emit_rect(accessible, ITF_EVENT_OBJECT, name, "", atk_rect);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/* 
 * Handles the ATK signal 'Gtk:AtkObject:active-descendant-changed' and 
 * converts it to the AT-SPI signal - 'object:active-descendant-changed'.
 *
 */
static gboolean
active_descendant_event_listener (GSignalInvocationHint *signal_hint,
				  guint n_param_values,
				  const GValue *param_values,
				  gpointer data)
{
  AtkObject *accessible;
  AtkObject *child;
  GSignalQuery signal_query;
  const gchar *name, *minor;
  gchar *s;
  gint detail1;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));
  child = ATK_OBJECT(g_value_get_pointer (&param_values[1]));
  g_return_val_if_fail (ATK_IS_OBJECT (child), TRUE);
  minor = g_quark_to_string (signal_hint->detail);

  detail1 = atk_object_get_index_in_parent (child);
  s = get_object_path (child);

  emit(accessible, ITF_EVENT_OBJECT, name, "", detail1, 0, DBUS_TYPE_OBJECT_PATH_AS_STRING, s);
  g_free(s);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/* 
 * Handles the ATK signal 'Gtk:AtkHypertext:link-selected' and
 * converts it to the AT-SPI signal - 'object:link-selected'
 *
 */
static gboolean
link_selected_event_listener (GSignalInvocationHint *signal_hint,
			      guint n_param_values,
			      const GValue *param_values,
			      gpointer data)
{
  AtkObject *accessible;
  GSignalQuery signal_query;
  const gchar *name, *minor;
  gint detail1;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));
  minor = g_quark_to_string (signal_hint->detail);

  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
        detail1 = g_value_get_int (&param_values[1]);

  emit(accessible, ITF_EVENT_OBJECT, name, minor, detail1, 0, DBUS_TYPE_INT32_AS_STRING, 0);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/* 
 * Handles the ATK signal 'Gtk:AtkText:text-changed' and
 * converts it to the AT-SPI signal - 'object:text-changed'
 *
 */
static gboolean
text_changed_event_listener (GSignalInvocationHint *signal_hint,
			     guint n_param_values,
			     const GValue *param_values,
			     gpointer data)
{
  AtkObject *accessible;
  GSignalQuery signal_query;
  const gchar *name, *minor;
  gchar *selected;
  gint detail1, detail2;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));
  minor = g_quark_to_string (signal_hint->detail);

  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
        detail1 = g_value_get_int (&param_values[1]);

  if (G_VALUE_TYPE (&param_values[2]) == G_TYPE_INT)
        detail2 = g_value_get_int (&param_values[2]);

  selected = atk_text_get_text (ATK_TEXT (accessible), detail1, detail1+detail2);

  emit(accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2, DBUS_TYPE_STRING_AS_STRING, selected);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/* 
 * Handles the ATK signal 'Gtk:AtkText:text-selection-changed' and
 * converts it to the AT-SPI signal - 'object:text-selection-changed'
 *
 */
static gboolean
text_selection_changed_event_listener (GSignalInvocationHint *signal_hint,
				       guint n_param_values,
				       const GValue *param_values,
				       gpointer data)
{
  AtkObject *accessible;
  GSignalQuery signal_query;
  const gchar *name, *minor;
  gint detail1, detail2;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));
  minor = g_quark_to_string (signal_hint->detail);

  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
        detail1 = g_value_get_int (&param_values[1]);

  if (G_VALUE_TYPE (&param_values[2]) == G_TYPE_INT)
        detail2 = g_value_get_int (&param_values[2]);

  emit(accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2, DBUS_TYPE_STRING_AS_STRING, "");
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * Generic signal converter and forwarder.
 *
 * Klass (Interface) org.freedesktop.atspi.Event.Object
 * Major is the signal name.
 * Minor is NULL.
 * detail1 is 0.
 * detail2 is 0.
 * any_data is NULL.
 */
static gboolean
generic_event_listener (GSignalInvocationHint *signal_hint,
			guint n_param_values,
		        const GValue *param_values,
		        gpointer data)
{
  AtkObject *accessible;
  GSignalQuery signal_query;
  const gchar *name;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT(g_value_get_object(&param_values[0]));
  emit(accessible, ITF_EVENT_OBJECT, name, "", 0, 0, DBUS_TYPE_INT32_AS_STRING, 0);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * Registers the provided function as a handler for the given signal name
 * and stores the signal id returned so that the function may be
 * de-registered later.
 */
static void
add_signal_listener (GSignalEmissionHook listener, const char *signal_name)
{
  guint id;

  id = atk_add_global_event_listener (listener, signal_name);
  g_array_append_val (listener_ids, id);
}

/*
 * Initialization for the signal handlers.
 *
 * Registers all required signal handlers.
 */
void
spi_atk_register_event_listeners (void)
{
  /*
   * Kludge to make sure the Atk interface types are registered, otherwise
   * the AtkText signal handlers below won't get registered
   */
  GObject   *ao = g_object_new (ATK_TYPE_OBJECT, NULL);
  AtkObject *bo = atk_no_op_object_new (ao);

  g_object_unref (G_OBJECT (bo));
  g_object_unref (ao);

  /* Register for focus event notifications, and register app with central registry  */
  listener_ids = g_array_sized_new (FALSE, TRUE, sizeof (guint), 16);

  atk_bridge_focus_tracker_id = atk_add_focus_tracker (focus_tracker);

  add_signal_listener (tree_update_listener,          "Gtk:AtkObject:property-change");
  add_signal_listener (tree_update_children_listener, "Gtk:AtkObject:children-changed");

  add_signal_listener (property_event_listener,               "Gtk:AtkObject:property-change");
  add_signal_listener (window_event_listener,                 "window:create");
  add_signal_listener (window_event_listener,                 "window:destroy");
  add_signal_listener (window_event_listener,                 "window:minimize");
  add_signal_listener (window_event_listener,                 "window:maximize");
  add_signal_listener (window_event_listener,                 "window:restore");
  add_signal_listener (window_event_listener,                 "window:activate");
  add_signal_listener (window_event_listener,                 "window:deactivate");
  add_signal_listener (document_event_listener,               "Gtk:AtkDocument:load-complete");
  add_signal_listener (document_event_listener,               "Gtk:AtkDocument:reload");
  add_signal_listener (document_event_listener,               "Gtk:AtkDocument:load-stopped");
  add_signal_listener (state_event_listener,                  "Gtk:AtkObject:state-change");
  add_signal_listener (active_descendant_event_listener,      "Gtk:AtkObject:active-descendant-changed");
  add_signal_listener (bounds_event_listener,                 "Gtk:AtkComponent:bounds-changed");
  add_signal_listener (text_selection_changed_event_listener, "Gtk:AtkText:text-selection-changed");
  add_signal_listener (text_changed_event_listener,           "Gtk:AtkText:text-changed");
  add_signal_listener (link_selected_event_listener,          "Gtk:AtkHypertext:link-selected");
  add_signal_listener (generic_event_listener,                "Gtk:AtkObject:visible-data-changed");
  add_signal_listener (generic_event_listener,                "Gtk:AtkSelection:selection-changed");
  add_signal_listener (generic_event_listener,                "Gtk:AtkText:text-caret-moved");
  add_signal_listener (generic_event_listener,                "Gtk:AtkTable:row-inserted");
  add_signal_listener (generic_event_listener,                "Gtk:AtkTable:row-reordered");
  add_signal_listener (generic_event_listener,                "Gtk:AtkTable:row-deleted");
  add_signal_listener (generic_event_listener,                "Gtk:AtkTable:column-inserted");
  add_signal_listener (generic_event_listener,                "Gtk:AtkTable:column-reordered");
  add_signal_listener (generic_event_listener,                "Gtk:AtkTable:column-deleted");
  add_signal_listener (generic_event_listener,                "Gtk:AtkTable:model-changed");

  /*
   * May add the following listeners to implement preemptive key listening for GTK+
   *
   * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-press-event");
   * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-release-event");
   */
  atk_bridge_key_event_listener_id = atk_add_key_event_listener (spi_atk_bridge_key_listener, NULL);
}

/*---------------------------------------------------------------------------*/

/* 
 * De-registers all ATK signal handlers.
 */
void
spi_atk_deregister_event_listeners (void)
{
  gint i;
  GArray *ids = listener_ids;
  listener_ids = NULL;

  if (atk_bridge_focus_tracker_id)
        atk_remove_focus_tracker (atk_bridge_focus_tracker_id);
  
  for (i = 0; ids && i < ids->len; i++)
    {
          atk_remove_global_event_listener (g_array_index (ids, guint, i));
    }
  
  if (atk_bridge_key_event_listener_id)
          atk_remove_key_event_listener (atk_bridge_key_event_listener_id);
}

/*---------------------------------------------------------------------------*/

/*
 * TODO This function seems out of place here.
 *
 * Emits fake deactivate signals on all top-level windows.
 * Used when shutting down AT-SPI, ensuring that all
 * windows have been removed on the client side.
 */
void
spi_atk_tidy_windows (void)
{
  AtkObject *root;
  gint n_children;
  gint i;

  root = atk_get_root ();
  n_children = atk_object_get_n_accessible_children (root);
  for (i = 0; i < n_children; i++)
    {
      AtkObject *child;
      AtkStateSet *stateset;
      const gchar *name;
     
      child = atk_object_ref_accessible_child (root, i);
      stateset = atk_object_ref_state_set (child);
      
      name = atk_object_get_name (child);
      if (atk_state_set_contains_state (stateset, ATK_STATE_ACTIVE))
        {
	  emit(child, ITF_EVENT_WINDOW, "deactivate", NULL, 0, 0, DBUS_TYPE_STRING_AS_STRING, name);
        }
      g_object_unref (stateset);

      emit(child, ITF_EVENT_WINDOW, "destroy", NULL, 0, 0, DBUS_TYPE_STRING_AS_STRING, name);
      g_object_unref (child);
    }
}

/*END------------------------------------------------------------------------*/
