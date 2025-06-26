/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2011, F123 Consulting & Mais Diferenças
 * Copyright 2008, 2009, Codethink Ltd.
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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

#include <ctype.h>
#include <string.h>

#define ATK_DISABLE_DEPRECATION_WARNINGS
#include <atk/atk.h>
#include <atspi/atspi.h>
#include <droute/droute.h>

#include "accessible-register.h"
#include "bridge.h"

#include "event.h"
#include "object.h"
#include "spi-dbus.h"

static GArray *listener_ids = NULL;

static gint atk_bridge_key_event_listener_id;
static gint atk_bridge_focus_tracker_id;

GMainContext *spi_context = NULL;

/*---------------------------------------------------------------------------*/

#define ITF_EVENT_OBJECT "org.a11y.atspi.Event.Object"
#define ITF_EVENT_WINDOW "org.a11y.atspi.Event.Window"
#define ITF_EVENT_DOCUMENT "org.a11y.atspi.Event.Document"
#define ITF_EVENT_FOCUS "org.a11y.atspi.Event.Focus"

/*---------------------------------------------------------------------------*/

typedef struct _SpiReentrantCallClosure
{
  DBusConnection *bus;
  GMainLoop *loop;
  DBusMessage *reply;
  guint timeout;
} SpiReentrantCallClosure;

static void
switch_main_context (GMainContext *cnx)
{
  GList *list;

  if (spi_global_app_data->server)
    atspi_dbus_server_setup_with_g_main (spi_global_app_data->server, cnx);
  atspi_dbus_connection_setup_with_g_main (spi_global_app_data->bus, cnx);
  atspi_set_main_context (cnx);
  for (list = spi_global_app_data->direct_connections; list; list = list->next)
    atspi_dbus_connection_setup_with_g_main (list->data, cnx);

  if (_atk_bridge_remove_pending_application_registration (spi_global_app_data))
    _atk_bridge_schedule_application_registration (spi_global_app_data);
}

guint
spi_idle_add (GSourceFunc function, gpointer data)
{
  GSource *source;
  guint id;

  source = g_idle_source_new ();
  g_source_set_callback (source, function, data, NULL);
  id = g_source_attach (source, spi_context);
  g_source_unref (source);

  return id;
}

guint
spi_timeout_add_seconds (gint interval, GSourceFunc function, gpointer data)
{
  GSource *source;
  guint id;

  source = g_timeout_source_new_seconds (interval);
  g_source_set_callback (source, function, data, NULL);
  id = g_source_attach (source, spi_context);
  g_source_unref (source);

  return id;
}

guint
spi_timeout_add_full (gint priority, guint interval, GSourceFunc function, gpointer data, GDestroyNotify notify)
{
  GSource *source;
  guint id;

  source = g_timeout_source_new (interval);
  g_source_set_priority (source, priority);
  g_source_set_callback (source, function, data, notify);
  id = g_source_attach (source, spi_context);
  g_source_unref (source);

  return id;
}

static void
set_reply (DBusPendingCall *pending, void *user_data)
{
  SpiReentrantCallClosure *closure = (SpiReentrantCallClosure *) user_data;

  closure->reply = dbus_pending_call_steal_reply (pending);
  dbus_pending_call_unref (pending);
  switch_main_context (spi_context);
  g_main_loop_quit (closure->loop);
}

static gboolean
timeout_reply (void *data)
{
  SpiReentrantCallClosure *closure = data;

  switch_main_context (spi_context);
  g_main_loop_quit (closure->loop);
  closure->timeout = -1;
  return FALSE;
}

static DBusMessage *
send_and_allow_reentry (DBusConnection *bus, DBusMessage *message)
{
  DBusPendingCall *pending;
  SpiReentrantCallClosure closure;
  GSource *source;

  closure.bus = bus;
  closure.loop = g_main_loop_new (spi_global_app_data->main_context, FALSE);
  closure.reply = NULL;
  switch_main_context (spi_global_app_data->main_context);

  if (!dbus_connection_send_with_reply (bus, message, &pending, 9000) || !pending)
    {
      switch_main_context (spi_context);
      return NULL;
    }
  dbus_pending_call_set_notify (pending, set_reply, (void *) &closure, NULL);
  source = g_timeout_source_new (500);
  g_source_set_callback (source, timeout_reply, &closure, NULL);
  closure.timeout = g_source_attach (source, spi_global_app_data->main_context);
  g_source_unref (source);
  g_main_loop_run (closure.loop);
  if (closure.timeout != -1)
    g_source_destroy (source);

  g_main_loop_unref (closure.loop);
  if (!closure.reply)
    dbus_pending_call_cancel (pending);
  return closure.reply;
}

void
atk_bridge_set_event_context (GMainContext *cnx)
{
  spi_context = cnx;
  switch_main_context (spi_context);
}

/*---------------------------------------------------------------------------*/

/*
 * Functionality related to sending device events from the application.
 *
 * This is used for forwarding key events on to the registry daemon.
 */

static gboolean
Accessibility_DeviceEventController_NotifyListenersSync (const AtspiDeviceEvent
                                                             *key_event)
{
  DBusMessage *message;
  dbus_bool_t consumed = FALSE;

  message =
      dbus_message_new_method_call (SPI_DBUS_NAME_REGISTRY,
                                    ATSPI_DBUS_PATH_DEC,
                                    ATSPI_DBUS_INTERFACE_DEC,
                                    "NotifyListenersSync");

  if (spi_dbus_marshal_deviceEvent (message, key_event))
    {
      DBusMessage *reply =
          send_and_allow_reentry (spi_global_app_data->bus, message);
      if (reply)
        {
          DBusError error;
          dbus_error_init (&error);
          if (!dbus_message_get_args (reply, &error, DBUS_TYPE_BOOLEAN,
                                      &consumed, DBUS_TYPE_INVALID))
            {
              /* TODO: print a warning */
              dbus_error_free (&error);
            }
          dbus_message_unref (reply);
        }
    }
  dbus_message_unref (message);
  return consumed;
}

static void
spi_init_keystroke_from_atk_key_event (AtspiDeviceEvent *keystroke,
                                       AtkKeyEventStruct *event)
{
  keystroke->id = (dbus_int32_t) event->keyval;
  keystroke->hw_code = (dbus_int16_t) event->keycode;
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
      keystroke->type = ATSPI_KEY_PRESSED_EVENT;
      break;
    case (ATK_KEY_EVENT_RELEASE):
      keystroke->type = ATSPI_KEY_RELEASED_EVENT;
      break;
    default:
      g_error ("atk passed us an AtkKeyEventStruct invalid type %d", event->type);
      return;
    }
}

static gint
spi_atk_bridge_key_listener (AtkKeyEventStruct *event, gpointer data)
{
  gboolean result;
  AtspiDeviceEvent key_event;

  spi_init_keystroke_from_atk_key_event (&key_event, event);

  result =
      Accessibility_DeviceEventController_NotifyListenersSync (&key_event);

  if (key_event.event_string)
    g_free (key_event.event_string);

  return result;
}

/*---------------------------------------------------------------------------*/

static const void *
validate_for_dbus (const gint type,
                   const void *val)
{
  switch (type)
    {
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
      if (!val)
        return "";
      else if (!g_utf8_validate (val, -1, NULL))
        {
          g_warning ("atk-bridge: Received bad UTF-8 string when emitting event");
          return "";
        }
      else
        return val;
    default:
      return val;
    }
}

static void
append_basic (DBusMessageIter *iter,
              const char *type,
              const void *val)
{
  DBusMessageIter sub;

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, type, &sub);

  val = validate_for_dbus ((int) *type, val);
  dbus_message_iter_append_basic (&sub, (int) *type, &val);

  dbus_message_iter_close_container (iter, &sub);
}

static void
append_rect (DBusMessageIter *iter,
             const char *type,
             const void *val)
{
  DBusMessageIter variant, sub;
  const AtkRectangle *rect = (const AtkRectangle *) val;

  dbus_message_iter_open_container (iter, DBUS_TYPE_VARIANT, type, &variant);

  dbus_message_iter_open_container (&variant, DBUS_TYPE_STRUCT, NULL, &sub);

  dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->x));
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->y));
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->width));
  dbus_message_iter_append_basic (&sub, DBUS_TYPE_INT32, &(rect->height));

  dbus_message_iter_close_container (&variant, &sub);

  dbus_message_iter_close_container (iter, &variant);
}

static void
append_object (DBusMessageIter *iter,
               const char *type,
               const void *val)
{
  spi_object_append_v_reference (iter, ATK_OBJECT (val));
}

static gchar *
signal_name_to_dbus (const gchar *s)
{
  gchar *ret = g_strdup (s);
  gchar *t;

  if (!ret)
    return NULL;
  ret[0] = toupper (ret[0]);
  while ((t = strchr (ret, '-')) != NULL)
    {
      memmove (t, t + 1, strlen (t));
      *t = toupper (*t);
    }
  return ret;
}

/*
 * Converts names of the form "active-descendant-changed" to
 * "ActiveDescendantChanged"
 */
static gchar *
ensure_proper_format (const char *name)
{
  gchar *ret = (gchar *) g_malloc (strlen (name) * 2 + 2);
  gchar *p = ret;
  gboolean need_upper = TRUE;

  if (!ret)
    return NULL;
  while (*name)
    {
      if (need_upper)
        {
          *p++ = toupper (*name);
          need_upper = FALSE;
        }
      else if (*name == '-')
        need_upper = TRUE;
      else if (*name == ':')
        {
          need_upper = TRUE;
          *p++ = *name;
        }
      else
        *p++ = *name;
      name++;
    }
  *p = '\0';
  return ret;
}

void
append_properties (GArray *properties, event_data *evdata)
{
  GSList *ls;
  gint i;

  for (ls = evdata->properties; ls; ls = ls->next)
    {
      gboolean dup = FALSE;
      for (i = 0; i < properties->len; i++)
        {
          if (ls->data == g_array_index (properties, AtspiPropertyDefinition *, i))
            {
              dup = TRUE;
              break;
            }
        }
      if (!dup)
        g_array_append_val (properties, ls->data);
    }
}

static gboolean
signal_is_needed (AtkObject *obj, const gchar *klass, const gchar *major, const gchar *minor, GArray **properties)
{
  gchar *data[4];
  event_data *evdata;
  gboolean ret = FALSE;
  GList *list;
  GArray *props = NULL;

  if (!spi_global_app_data->events_initialized)
    return TRUE;

  data[0] = ensure_proper_format (klass[0] ? klass + 21 : klass);
  data[1] = ensure_proper_format (major);
  data[2] = ensure_proper_format (minor);
  data[3] = NULL;

  /* Hack: Always pass events that update the cache.
   * TODO: FOr 2.2, have at-spi2-core define a special "cache listener" for
   * this instead, so that we don't send these if no one is listening */
  if (!g_strcmp0 (data[1], "ChildrenChanged") ||
      ((!g_strcmp0 (data[1], "PropertyChange")) &&
       (!g_strcmp0 (data[2], "AccessibleName") ||
        !g_strcmp0 (data[2], "AccessibleDescription") ||
        !g_strcmp0 (data[2], "AccessibleParent") ||
        !g_strcmp0 (data[2], "AccessibleRole"))) ||
      !g_strcmp0 (data[1], "StateChanged"))
    {
      if (minor && !g_strcmp0 (minor, "defunct"))
        ret = TRUE;
      else
        {
          AtkStateSet *set = atk_object_ref_state_set (obj);
          AtkState state = ((!g_strcmp0 (data[1], "ChildrenChanged")) ? ATK_STATE_MANAGES_DESCENDANTS : ATK_STATE_TRANSIENT);
          ret = !atk_state_set_contains_state (set, state);
          g_object_unref (set);
        }
    }

  /* Hack: events such as "object::text-changed::insert:system" as
     generated by Gecko */
  data[2][strcspn (data[2], ":")] = '\0';

  for (list = spi_global_app_data->events; list; list = list->next)
    {
      evdata = list->data;
      if (spi_event_is_subtype (data, evdata->data))
        {
          ret = TRUE;
          if (!props)
            props = g_array_new (TRUE, TRUE, sizeof (AtspiPropertyDefinition *));
          append_properties (props, evdata);
        }
    }

  g_free (data[2]);
  g_free (data[1]);
  g_free (data[0]);
  *properties = props;
  return ret;
}

/* Convert a : to a / so that listeners can use arg0path to match only
 *  * the prefix */
static char *
adapt_minor_for_dbus (const char *source)
{
  gchar *ret = g_strdup (source);
  int i = strcspn (ret, ":");
  if (ret[i] == ':')
    ret[i] = '/';
  return ret;
}

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
static void
emit_event (AtkObject *obj,
            const char *klass,
            const char *major,
            const char *minor,
            dbus_int32_t detail1,
            dbus_int32_t detail2,
            const char *type,
            const void *val,
            void (*append_variant) (DBusMessageIter *, const char *, const void *))
{
  DBusConnection *bus = spi_global_app_data->bus;
  char *path;
  char *minor_dbus;

  gchar *cname;
  DBusMessage *sig;
  DBusMessageIter iter, iter_dict, iter_dict_entry;
  GArray *properties = NULL;

  if (!klass)
    klass = "";
  if (!major)
    major = "";
  if (!minor)
    minor = "";
  if (!type)
    type = "u";

  if (!signal_is_needed (obj, klass, major, minor, &properties))
    return;

  path = spi_register_object_to_path (spi_global_register, G_OBJECT (obj));
  g_return_if_fail (path != NULL);

  /*
   * This is very annoying, but as '-' isn't a legal signal
   * name in D-Bus (Why not??!?) The names need converting
   * on this side, and again on the client side.
   */
  cname = signal_name_to_dbus (major);
  sig = dbus_message_new_signal (path, klass, cname);

  dbus_message_iter_init_append (sig, &iter);

  minor_dbus = adapt_minor_for_dbus (minor);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &minor_dbus);
  g_free (minor_dbus);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &detail1);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &detail2);
  append_variant (&iter, type, val);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}", &iter_dict);
  /* Add requested properties, unless the object is being marked defunct, in
     which case it's safest not to touch it */
  if (minor == NULL || strcmp (minor, "defunct") != 0 || detail1 == 0)
    {
      if (properties)
        {
          gint i;
          for (i = 0; i < properties->len; i++)
            {
              AtspiPropertyDefinition *prop = g_array_index (properties, AtspiPropertyDefinition *, i);
              dbus_message_iter_open_container (&iter_dict, DBUS_TYPE_DICT_ENTRY, NULL,
                                                &iter_dict_entry);
              dbus_message_iter_append_basic (&iter_dict_entry, DBUS_TYPE_STRING, &prop->name);
              prop->func (&iter_dict_entry, obj);
              dbus_message_iter_close_container (&iter_dict, &iter_dict_entry);
            }
          g_array_free (properties, TRUE);
        }
    }
  dbus_message_iter_close_container (&iter, &iter_dict);

  dbus_connection_send (bus, sig, NULL);
  dbus_message_unref (sig);

  if (g_strcmp0 (cname, "ChildrenChanged") != 0)
    spi_object_lease_if_needed (G_OBJECT (obj));

  g_free (cname);
  g_free (path);
}

/*---------------------------------------------------------------------------*/

/*
 * The focus listener handles the ATK 'focus' signal and forwards it
 * as the AT-SPI event, 'focus:'
 */
static void
focus_tracker (AtkObject *accessible)
{
  emit_event (accessible, ITF_EVENT_FOCUS, "focus", "", 0, 0,
              DBUS_TYPE_INT32_AS_STRING, 0, append_basic);
}

/*---------------------------------------------------------------------------*/

#define PCHANGE "PropertyChange"

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
                         guint n_param_values,
                         const GValue *param_values,
                         gpointer data)
{
  AtkObject *accessible;
  AtkPropertyValues *values;

  const gchar *pname = NULL;

  AtkObject *otemp;
  const gchar *s1;
  gint i;

  accessible = g_value_get_object (&param_values[0]);
  values = (AtkPropertyValues *) g_value_get_pointer (&param_values[1]);

  pname = values[0].property_name;

  /* TODO Could improve this control statement by matching
   * on only the end of the signal names,
   */
  if (strcmp (pname, "accessible-name") == 0)
    {
      s1 = atk_object_get_name (accessible);
      if (s1 != NULL)
        emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                    DBUS_TYPE_STRING_AS_STRING, s1, append_basic);
    }
  else if (strcmp (pname, "accessible-description") == 0)
    {
      s1 = atk_object_get_description (accessible);
      if (s1 != NULL)
        emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                    DBUS_TYPE_STRING_AS_STRING, s1, append_basic);
    }
  else if (strcmp (pname, "accessible-help-text") == 0)
    {
      s1 = atk_object_get_help_text (accessible);
      if (s1 != NULL)
        emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                    DBUS_TYPE_STRING_AS_STRING, s1, append_basic);
    }
  else if (strcmp (pname, "accessible-parent") == 0)
    {
      otemp = atk_object_get_parent (accessible);
      if (otemp != NULL)
        emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                    "(so)", otemp, append_object);
    }
  else if (strcmp (pname, "accessible-role") == 0)
    {
      i = atk_object_get_role (accessible);
      emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                  DBUS_TYPE_UINT32_AS_STRING, GINT_TO_POINTER (i), append_basic);
    }
  else if (strcmp (pname, "accessible-table-summary") == 0)
    {
      otemp = atk_table_get_summary (ATK_TABLE (accessible));
      if (otemp != NULL)
        emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                    "(so)", otemp, append_object);
    }
  else if (strcmp (pname, "accessible-table-column-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      otemp = atk_table_get_column_header (ATK_TABLE (accessible), i);
      if (otemp != NULL)
        emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                    "(so)", otemp, append_object);
    }
  else if (strcmp (pname, "accessible-table-row-header") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      otemp = atk_table_get_row_header (ATK_TABLE (accessible), i);
      if (otemp != NULL)
        emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                    "(so)", otemp, append_object);
    }
  else if (strcmp (pname, "accessible-table-row-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      s1 = atk_table_get_row_description (ATK_TABLE (accessible), i);
      emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                  DBUS_TYPE_STRING_AS_STRING, s1, append_basic);
    }
  else if (strcmp (pname, "accessible-table-column-description") == 0)
    {
      i = g_value_get_int (&(values->new_value));
      s1 = atk_table_get_column_description (ATK_TABLE (accessible), i);
      emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                  DBUS_TYPE_STRING_AS_STRING, s1, append_basic);
    }
  else if (strcmp (pname, "accessible-table-caption-object") == 0)
    {
      otemp = atk_table_get_caption (ATK_TABLE (accessible));
      emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                  "(so)", otemp, append_object);
    }
  else
    {
      emit_event (accessible, ITF_EVENT_OBJECT, PCHANGE, pname, 0, 0,
                  DBUS_TYPE_INT32_AS_STRING, 0, append_basic);
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
  const gchar *pname;
  guint detail1;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  pname = g_value_get_string (&param_values[1]);

  detail1 = (g_value_get_boolean (&param_values[2])) ? 1 : 0;
  emit_event (accessible, ITF_EVENT_OBJECT, STATE_CHANGED, pname, detail1, 0,
              DBUS_TYPE_INT32_AS_STRING, 0, append_basic);

  if (!g_strcmp0 (pname, "defunct") && detail1)
    spi_register_deregister_object (spi_global_register, G_OBJECT (accessible),
                                    TRUE);
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

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  s = atk_object_get_name (accessible);
  emit_event (accessible, ITF_EVENT_WINDOW, name, "", 0, 0,
              DBUS_TYPE_STRING_AS_STRING, s, append_basic);

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
 * Gtk:AtkDocument:page-changed  ->  document:page-changed
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
  gint detail1 = 0;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  if (n_param_values > 0) // on the case of page-changed
    if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
      detail1 = g_value_get_int (&param_values[1]);

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  s = atk_object_get_name (accessible);
  emit_event (accessible, ITF_EVENT_DOCUMENT, name, "", detail1, 0,
              DBUS_TYPE_STRING_AS_STRING, s, append_basic);

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
  const gchar *name;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));

  if (G_VALUE_HOLDS_BOXED (param_values + 1))
    {
      atk_rect = g_value_get_boxed (param_values + 1);

      emit_event (accessible, ITF_EVENT_OBJECT, name, "", 0, 0,
                  "(iiii)", atk_rect, append_rect);
    }
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
  const gchar *name;
  gint detail1;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  child = ATK_OBJECT (g_value_get_pointer (&param_values[1]));
  g_return_val_if_fail (ATK_IS_OBJECT (child), TRUE);

  detail1 = atk_object_get_index_in_parent (child);

  emit_event (accessible, ITF_EVENT_OBJECT, name, "", detail1, 0,
              "(so)", child, append_object);
  return TRUE;
}

static gboolean
announcement_event_listener (GSignalInvocationHint *signal_hint,
                             guint n_param_values,
                             const GValue *param_values,
                             gpointer data)
{
  AtkObject *accessible;
  const gchar *text;
  GSignalQuery signal_query;
  const gchar *name;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  text = g_value_get_string (&param_values[1]);
  g_return_val_if_fail (text != NULL, TRUE);

  emit_event (accessible, ITF_EVENT_OBJECT, name, "", ATSPI_LIVE_POLITE, 0,
              "s", text, append_basic);
  return TRUE;
}

static gboolean
notification_event_listener (GSignalInvocationHint *signal_hint,
                             guint n_param_values,
                             const GValue *param_values,
                             gpointer data)
{
  AtkObject *accessible;
  const gchar *text;
  gint politeness;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  text = g_value_get_string (&param_values[1]);
  g_return_val_if_fail (text != NULL, TRUE);
  politeness = g_value_get_int (&param_values[2]);

  emit_event (accessible, ITF_EVENT_OBJECT, "announcement", "", politeness, 0,
              "s", text, append_basic);
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
  gint detail1 = 0;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  minor = g_quark_to_string (signal_hint->detail);

  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
    detail1 = g_value_get_int (&param_values[1]);

  emit_event (accessible, ITF_EVENT_OBJECT, name, minor, detail1, 0,
              DBUS_TYPE_INT32_AS_STRING, 0, append_basic);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * Handles the ATK signal 'Gtk:AtkText:text-changed' and
 * converts it to the AT-SPI signal - 'object:text-changed'
 * This signal is deprecated by Gtk:AtkText:text-insert
 * and Gtk:AtkText:text-remove
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
  gint detail1 = 0, detail2 = 0;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  minor = g_quark_to_string (signal_hint->detail);

  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
    detail1 = g_value_get_int (&param_values[1]);

  if (G_VALUE_TYPE (&param_values[2]) == G_TYPE_INT)
    detail2 = g_value_get_int (&param_values[2]);

  selected =
      atk_text_get_text (ATK_TEXT (accessible), detail1, detail1 + detail2);

  emit_event (accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2,
              DBUS_TYPE_STRING_AS_STRING, selected, append_basic);
  g_free (selected);

  return TRUE;
}

/*
 * Handles the ATK signal 'Gtk:AtkText:text-insert' and
 * converts it to the AT-SPI signal - 'object:text-changed'
 *
 */
static gboolean
text_insert_event_listener (GSignalInvocationHint *signal_hint,
                            guint n_param_values,
                            const GValue *param_values,
                            gpointer data)
{
  AtkObject *accessible;
  guint text_changed_signal_id;
  GSignalQuery signal_query;
  const gchar *name;
  const gchar *minor_raw, *text;
  gchar *minor;
  gint detail1 = 0, detail2 = 0;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  /* Get signal name for 'Gtk:AtkText:text-changed' so
   * we convert it to the AT-SPI signal - 'object:text-changed'
   */
  text_changed_signal_id = g_signal_lookup ("text-changed", G_OBJECT_TYPE (accessible));
  g_signal_query (text_changed_signal_id, &signal_query);
  name = signal_query.signal_name;

  /* Add the insert and keep any detail coming from atk */
  minor_raw = g_quark_to_string (signal_hint->detail);
  if (minor_raw)
    minor = g_strconcat ("insert:", minor_raw, NULL);
  else
    minor = g_strdup ("insert");

  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
    detail1 = g_value_get_int (&param_values[1]);

  if (G_VALUE_TYPE (&param_values[2]) == G_TYPE_INT)
    detail2 = g_value_get_int (&param_values[2]);

  if (G_VALUE_TYPE (&param_values[3]) == G_TYPE_STRING)
    text = g_value_get_string (&param_values[3]);
  else
    text = "";

  emit_event (accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2,
              DBUS_TYPE_STRING_AS_STRING, text, append_basic);
  g_free (minor);
  return TRUE;
}

/*
 * Handles the ATK signal 'Gtk:AtkText:text-remove' and
 * converts it to the AT-SPI signal - 'object:text-changed'
 *
 */
static gboolean
text_remove_event_listener (GSignalInvocationHint *signal_hint,
                            guint n_param_values,
                            const GValue *param_values,
                            gpointer data)
{
  AtkObject *accessible;
  guint text_changed_signal_id;
  GSignalQuery signal_query;
  const gchar *name;
  const gchar *minor_raw, *text;
  gchar *minor;
  gint detail1 = 0, detail2 = 0;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  /* Get signal name for 'Gtk:AtkText:text-changed' so
   * we convert it to the AT-SPI signal - 'object:text-changed'
   */
  text_changed_signal_id = g_signal_lookup ("text-changed", G_OBJECT_TYPE (accessible));
  g_signal_query (text_changed_signal_id, &signal_query);
  name = signal_query.signal_name;

  minor_raw = g_quark_to_string (signal_hint->detail);

  /* Add the delete and keep any detail coming from atk */
  if (minor_raw)
    minor = g_strconcat ("delete:", minor_raw, NULL);
  else
    minor = g_strdup ("delete");

  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
    detail1 = g_value_get_int (&param_values[1]);

  if (G_VALUE_TYPE (&param_values[2]) == G_TYPE_INT)
    detail2 = g_value_get_int (&param_values[2]);

  if (G_VALUE_TYPE (&param_values[3]) == G_TYPE_STRING)
    text = g_value_get_string (&param_values[3]);
  else
    text = "";

  emit_event (accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2,
              DBUS_TYPE_STRING_AS_STRING, text, append_basic);
  g_free (minor);
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

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  minor = g_quark_to_string (signal_hint->detail);

  emit_event (accessible, ITF_EVENT_OBJECT, name, minor, 0, 0,
              DBUS_TYPE_STRING_AS_STRING, "", append_basic);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * Children changed signal converter and forwarder.
 *
 * Klass (Interface) org.a11y.atspi.Event.Object
 * Major is the signal name.
 * Minor is 'add' or 'remove'
 * detail1 is the index.
 * detail2 is 0.
 * any_data is the child reference.
 */
static gboolean
children_changed_event_listener (GSignalInvocationHint *signal_hint,
                                 guint n_param_values,
                                 const GValue *param_values,
                                 gpointer data)
{
  GSignalQuery signal_query;
  const gchar *name, *minor;
  gint detail1 = 0, detail2 = 0;

  AtkObject *accessible, *ao = NULL;
  gpointer child;
  AtkStateSet *set;
  gboolean ret;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  /* If the accessible is on STATE_MANAGES_DESCENDANTS state,
     children-changed signal are not forwarded. */
  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  set = atk_object_ref_state_set (accessible);
  ret = atk_state_set_contains_state (set, ATK_STATE_MANAGES_DESCENDANTS);
  g_object_unref (set);

  if (ret)
    return TRUE;

  minor = g_quark_to_string (signal_hint->detail);

  detail1 = g_value_get_uint (param_values + 1);
  child = g_value_get_pointer (param_values + 2);

  if (ATK_IS_OBJECT (child))
    {
      ao = ATK_OBJECT (child);
      emit_event (accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2,
                  "(so)", ao, append_object);
    }
  else if ((minor != NULL) && (strcmp (minor, "add") == 0))
    {
      ao = atk_object_ref_accessible_child (accessible,
                                            detail1);
      emit_event (accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2,
                  "(so)", ao, append_object);
      g_object_unref (ao);
    }
  else
    {
      emit_event (accessible, ITF_EVENT_OBJECT, name, minor, detail1, detail2,
                  "(so)", ao, append_object);
    }

  return TRUE;
}

/*
 * Handles the ATK signal 'Gtk:AtkObject:attribute-changed' and
 * converts it to the AT-SPI signal - 'object:attributes-changed'
 *
 */
static gboolean
attribute_changed_event_listener (GSignalInvocationHint *signal_hint,
                                  guint n_param_values,
                                  const GValue *param_values,
                                  gpointer data)
{
  AtkObject *accessible;
  const gchar *key = NULL, *value = NULL;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_STRING)
    key = g_value_get_string (&param_values[1]);
  else
    key = "";
  if (G_VALUE_TYPE (&param_values[2]) == G_TYPE_STRING)
    value = g_value_get_string (&param_values[2]);
  else
    value = "";

  emit_event (accessible, ITF_EVENT_OBJECT, "attributes-changed", key, 0, 0,
              DBUS_TYPE_STRING_AS_STRING, value, append_basic);
  return TRUE;
}

/*
 * Handles the ATK signal 'Gtk:AtkDocument:attribute-changed' and
 * converts it to the AT-SPI signal - 'document:attributes-changed'
 *
 */
static gboolean
document_attribute_changed_event_listener (GSignalInvocationHint *signal_hint,
                                           guint n_param_values,
                                           const GValue *param_values,
                                           gpointer data)
{
  AtkObject *accessible;
  const gchar *key = NULL, *value = NULL;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));
  if (G_VALUE_TYPE (&param_values[1]) == G_TYPE_STRING)
    key = g_value_get_string (&param_values[1]);
  else
    key = "";
  if (G_VALUE_TYPE (&param_values[2]) == G_TYPE_STRING)
    value = g_value_get_string (&param_values[2]);
  else
    value = "";

  emit_event (accessible, ITF_EVENT_DOCUMENT, "attributes-changed", key, 0, 0,
              DBUS_TYPE_STRING_AS_STRING, value, append_basic);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * Generic signal converter and forwarder.
 *
 * Klass (Interface) org.a11y.atspi.Event.Object
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
  int detail1 = 0, detail2 = 0;

  g_signal_query (signal_hint->signal_id, &signal_query);
  name = signal_query.signal_name;

  accessible = ATK_OBJECT (g_value_get_object (&param_values[0]));

  if (n_param_values > 1 && G_VALUE_TYPE (&param_values[1]) == G_TYPE_INT)
    detail1 = g_value_get_int (&param_values[1]);

  if (n_param_values > 2 && G_VALUE_TYPE (&param_values[2]) == G_TYPE_INT)
    detail2 = g_value_get_int (&param_values[2]);

  emit_event (accessible, ITF_EVENT_OBJECT, name, "", detail1, detail2,
              DBUS_TYPE_INT32_AS_STRING, 0, append_basic);
  return TRUE;
}

/*---------------------------------------------------------------------------*/

/*
 * Registers the provided function as a handler for the given signal name
 * and stores the signal id returned so that the function may be
 * de-registered later.
 */
static guint
add_signal_listener (GSignalEmissionHook listener, const char *signal_name)
{
  guint id;

  id = atk_add_global_event_listener (listener, signal_name);

  if (id > 0) /* id == 0 is a failure */
    g_array_append_val (listener_ids, id);

  return id;
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
  GObject *ao = g_object_new (ATK_TYPE_OBJECT, NULL);
  AtkObject *bo = atk_no_op_object_new (ao);
  guint id = 0;

  g_object_unref (G_OBJECT (bo));
  g_object_unref (ao);

  if (listener_ids)
    {
      g_warning ("atk_bridge: spi_atk-register_event_listeners called multiple times");
      return;
    }

  /* Register for focus event notifications, and register app with central registry  */
  listener_ids = g_array_sized_new (FALSE, TRUE, sizeof (guint), 16);

  atk_bridge_focus_tracker_id = atk_add_focus_tracker (focus_tracker);

  add_signal_listener (property_event_listener,
                       "Gtk:AtkObject:property-change");

  /* window events: we tentative try to register using the old format */
  id = add_signal_listener (window_event_listener, "window:create");

  if (id != 0)
    {
      /* If we are able to register using the old format, we assume
       * that the ATK implementor is managing window events without
       * AtkWindow. We can't use the opposite test because after
       * including AtkWindow on ATK you would be able to register to
       * that event, although the ATK implementor could or not use it.
       */

      add_signal_listener (window_event_listener, "window:destroy");
      add_signal_listener (window_event_listener, "window:minimize");
      add_signal_listener (window_event_listener, "window:maximize");
      add_signal_listener (window_event_listener, "window:restore");
      add_signal_listener (window_event_listener, "window:activate");
      add_signal_listener (window_event_listener, "window:deactivate");
    }
  else
    {
      add_signal_listener (window_event_listener, "Atk:AtkWindow:create");
      add_signal_listener (window_event_listener, "Atk:AtkWindow:destroy");
      add_signal_listener (window_event_listener, "Atk:AtkWindow:minimize");
      add_signal_listener (window_event_listener, "Atk:AtkWindow:maximize");
      add_signal_listener (window_event_listener, "Atk:AtkWindow:restore");
      add_signal_listener (window_event_listener, "Atk:AtkWindow:activate");
      add_signal_listener (window_event_listener, "Atk:AtkWindow:deactivate");
    }

  add_signal_listener (document_event_listener,
                       "Gtk:AtkDocument:load-complete");
  add_signal_listener (document_event_listener, "Gtk:AtkDocument:reload");
  add_signal_listener (document_event_listener,
                       "Gtk:AtkDocument:load-stopped");
  add_signal_listener (document_event_listener,
                       "Gtk:AtkDocument:page-changed");
  add_signal_listener (document_attribute_changed_event_listener,
                       "Gtk:AtkDocument:document-attribute-changed");
  /* TODO Fake this event on the client side */
  add_signal_listener (state_event_listener, "Gtk:AtkObject:state-change");
  /* TODO */
  add_signal_listener (active_descendant_event_listener,
                       "Gtk:AtkObject:active-descendant-changed");
  add_signal_listener (announcement_event_listener,
                       "Gtk:AtkObject:announcement");
  add_signal_listener (notification_event_listener,
                       "Gtk:AtkObject:notification");
  add_signal_listener (attribute_changed_event_listener,
                       "Gtk:AtkObject:attribute-changed");
  add_signal_listener (bounds_event_listener,
                       "Gtk:AtkComponent:bounds-changed");
  add_signal_listener (text_selection_changed_event_listener,
                       "Gtk:AtkText:text-selection-changed");
  add_signal_listener (text_changed_event_listener,
                       "Gtk:AtkText:text-changed");
  add_signal_listener (text_insert_event_listener,
                       "Gtk:AtkText:text-insert");
  add_signal_listener (text_remove_event_listener,
                       "Gtk:AtkText:text-remove");
  add_signal_listener (link_selected_event_listener,
                       "Gtk:AtkHypertext:link-selected");
  add_signal_listener (generic_event_listener,
                       "Gtk:AtkObject:visible-data-changed");
  add_signal_listener (generic_event_listener,
                       "Gtk:AtkSelection:selection-changed");
  add_signal_listener (generic_event_listener,
                       "Gtk:AtkText:text-attributes-changed");
  add_signal_listener (generic_event_listener,
                       "Gtk:AtkText:text-caret-moved");
  add_signal_listener (generic_event_listener, "Gtk:AtkTable:row-inserted");
  add_signal_listener (generic_event_listener, "Gtk:AtkTable:row-reordered");
  add_signal_listener (generic_event_listener, "Gtk:AtkTable:row-deleted");
  add_signal_listener (generic_event_listener,
                       "Gtk:AtkTable:column-inserted");
  add_signal_listener (generic_event_listener,
                       "Gtk:AtkTable:column-reordered");
  add_signal_listener (generic_event_listener, "Gtk:AtkTable:column-deleted");
  add_signal_listener (generic_event_listener, "Gtk:AtkTable:model-changed");
  add_signal_listener (children_changed_event_listener, "Gtk:AtkObject:children-changed");

  /*
   * May add the following listeners to implement preemptive key listening for GTK+
   *
   * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-press-event");
   * atk_add_global_event_listener (spi_atk_bridge_widgetkey_listener, "Gtk:GtkWidget:key-release-event");
   */
  atk_bridge_key_event_listener_id =
      atk_add_key_event_listener (spi_atk_bridge_key_listener, NULL);
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
    {
      atk_remove_focus_tracker (atk_bridge_focus_tracker_id);
      atk_bridge_focus_tracker_id = 0;
    }

  if (ids)
    {
      for (i = 0; i < ids->len; i++)
        {
          atk_remove_global_event_listener (g_array_index (ids, guint, i));
        }
      g_array_free (ids, TRUE);
    }

  if (atk_bridge_key_event_listener_id)
    {
      atk_remove_key_event_listener (atk_bridge_key_event_listener_id);
      atk_bridge_key_event_listener_id = 0;
    }
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
          emit_event (child, ITF_EVENT_WINDOW, "deactivate", NULL, 0, 0,
                      DBUS_TYPE_STRING_AS_STRING, name, append_basic);
        }
      g_object_unref (stateset);

      emit_event (child, ITF_EVENT_WINDOW, "destroy", NULL, 0, 0,
                  DBUS_TYPE_STRING_AS_STRING, name, append_basic);
      g_object_unref (child);
    }
}

gboolean
spi_event_is_subtype (gchar **needle, gchar **haystack)
{
  while (*haystack && **haystack)
    {
      if (g_strcmp0 (*needle, *haystack))
        return FALSE;
      needle++;
      haystack++;
    }
  return TRUE;
}

/*END------------------------------------------------------------------------*/
