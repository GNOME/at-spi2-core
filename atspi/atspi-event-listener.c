/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian Inc.
 * Copyright 2002 Sun Microsystems, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#include "atspi-accessible-private.h"
#include "atspi-private.h"
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>

static struct timeval window_filter_time;

/**
 * AtspiEventListener:
 *
 * A generic interface implemented by objects for the receipt of event
 * notifications.
 *
 * A generic interface implemented by objects for the receipt of event
 * notifications. atspi-event-listener is the interface via which clients of
 * the atspi-registry receive notification of changes to an application's user
 * interface and content.
 */

typedef struct
{
  AtspiEventListenerCB callback;
  void *user_data;
  GDestroyNotify callback_destroyed;
  char *event_type;
  char *category;
  char *name;
  char *detail;
  GArray *properties;
  AtspiAccessible *app;
} EventListenerEntry;

G_DEFINE_TYPE (AtspiEventListener, atspi_event_listener, G_TYPE_OBJECT)

void
atspi_event_listener_init (AtspiEventListener *listener)
{
}

void
atspi_event_listener_class_init (AtspiEventListenerClass *klass)
{
}

static void
remove_datum (AtspiEvent *event, void *user_data)
{
  AtspiEventListenerSimpleCB cb = user_data;
  cb (event);
}

typedef struct
{
  gpointer callback;
  GDestroyNotify callback_destroyed;
  gint ref_count;
} CallbackInfo;
static GHashTable *callbacks;

void
callback_ref (void *callback, GDestroyNotify callback_destroyed)
{
  CallbackInfo *info;

  if (!callbacks)
    {
      callbacks = g_hash_table_new (g_direct_hash, g_direct_equal);
      if (!callbacks)
        return;
    }

  info = g_hash_table_lookup (callbacks, callback);
  if (!info)
    {
      info = g_new (CallbackInfo, 1);
      info->callback = callback;
      info->callback_destroyed = callback_destroyed;
      info->ref_count = 1;
      g_hash_table_insert (callbacks, callback, info);
    }
  else
    info->ref_count++;
}

void
callback_unref (gpointer callback)
{
  CallbackInfo *info;

  if (!callbacks)
    return;
  info = g_hash_table_lookup (callbacks, callback);
  if (!info)
    {
      g_warning ("AT-SPI: Dereferencing invalid callback %p\n", callback);
      return;
    }
  info->ref_count--;
  if (info->ref_count == 0)
    {
#if 0
    /* TODO: Figure out why this seg faults from Python */
    if (info->callback_destroyed)
      (*info->callback_destroyed) (info->callback);
#endif
      g_free (info);
      g_hash_table_remove (callbacks, callback);
    }
}

/**
 * atspi_event_listener_new:
 * @callback: (scope notified) (destroy callback_destroyed) (closure user_data): an
 *   #AtspiEventListenerCB to be called when an event is fired.
 * @user_data: data to pass to the callback.
 * @callback_destroyed: A #GDestroyNotify called when the listener is freed
 *   and data associated with the callback should be freed.  Can be NULL.
 *
 * Creates a new #AtspiEventListener associated with a specified @callback.
 *
 * Returns: (transfer full): A new #AtspiEventListener.
 */
AtspiEventListener *
atspi_event_listener_new (AtspiEventListenerCB callback,
                          gpointer user_data,
                          GDestroyNotify callback_destroyed)
{
  AtspiEventListener *listener = g_object_new (ATSPI_TYPE_EVENT_LISTENER, NULL);
  listener->callback = callback;
  callback_ref (callback, callback_destroyed);
  listener->user_data = user_data;
  listener->cb_destroyed = callback_destroyed;
  return listener;
}

/**
 * atspi_event_listener_new_simple: (skip)
 * @callback: (scope notified): An #AtspiEventListenerSimpleCB to be called
 * when an event is fired.
 * @callback_destroyed: A #GDestroyNotify called when the listener is freed
 * and data associated with the callback should be freed.  Can be NULL.
 *
 * Creates a new #AtspiEventListener associated with a specified @callback.
 * Returns: (transfer full): A new #AtspiEventListener.
 **/
AtspiEventListener *
atspi_event_listener_new_simple (AtspiEventListenerSimpleCB callback,
                                 GDestroyNotify callback_destroyed)
{
  AtspiEventListener *listener = g_object_new (ATSPI_TYPE_EVENT_LISTENER, NULL);
  listener->callback = remove_datum;
  callback_ref (remove_datum, callback_destroyed);
  listener->user_data = callback;
  listener->cb_destroyed = callback_destroyed;
  return listener;
}

static GList *event_listeners = NULL;
static GList *pending_removals = NULL;
static int in_send = 0;

static gchar *
convert_name_from_dbus (const char *name, gboolean path_hack)
{
  gchar *ret;
  const char *p = name;
  gchar *q;

  if (!name)
    return g_strdup ("");

  ret = g_malloc (g_utf8_strlen (name, -1) * 2 + 1);
  q = ret;

  while (*p)
    {
      if (isupper (*p))
        {
          if (q > ret)
            *q++ = '-';
          *q++ = tolower (*p++);
        }
      else if (path_hack && *p == '/')
        {
          *q++ = ':';
          p++;
        }
      else
        *q++ = *p++;
    }
  *q = '\0';
  return ret;
}

static void
cache_process_children_changed (AtspiEvent *event)
{
  AtspiAccessible *child;

  if (!G_VALUE_HOLDS (&event->any_data, ATSPI_TYPE_ACCESSIBLE) ||
      !(event->source->cached_properties & ATSPI_CACHE_CHILDREN) ||
      atspi_state_set_contains (event->source->states, ATSPI_STATE_MANAGES_DESCENDANTS))
    return;

  child = g_value_get_object (&event->any_data);
  if (child == NULL)
    return;

  if (!strncmp (event->type, "object:children-changed:add", 27))
    {
      g_ptr_array_remove (event->source->children, child); /* just to be safe */
      if (event->detail1 < 0 || event->detail1 > event->source->children->len)
        {
          event->source->cached_properties &= ~ATSPI_CACHE_CHILDREN;
          return;
        }
      g_ptr_array_insert (event->source->children, event->detail1, g_object_ref (child));
    }
  else
    {
      g_ptr_array_remove (event->source->children, child);
      if (child == child->parent.app->root)
        g_object_run_dispose (G_OBJECT (child->parent.app));
    }
}

static void
cache_process_property_change (AtspiEvent *event)
{
  if (!strcmp (event->type, "object:property-change:accessible-parent"))
    {
      if (event->source->accessible_parent)
        g_object_unref (event->source->accessible_parent);
      if (G_VALUE_HOLDS (&event->any_data, ATSPI_TYPE_ACCESSIBLE))
        {
          event->source->accessible_parent = g_value_dup_object (&event->any_data);
          _atspi_accessible_add_cache (event->source, ATSPI_CACHE_PARENT);
        }
      else
        {
          event->source->accessible_parent = NULL;
          event->source->cached_properties &= ~ATSPI_CACHE_PARENT;
        }
    }
  else if (!strcmp (event->type, "object:property-change:accessible-name"))
    {
      if (event->source->name)
        g_free (event->source->name);
      if (G_VALUE_HOLDS_STRING (&event->any_data))
        {
          event->source->name = g_value_dup_string (&event->any_data);
          _atspi_accessible_add_cache (event->source, ATSPI_CACHE_NAME);
        }
      else
        {
          event->source->name = NULL;
          event->source->cached_properties &= ~ATSPI_CACHE_NAME;
        }
    }
  else if (!strcmp (event->type, "object:property-change:accessible-description"))
    {
      if (event->source->description)
        g_free (event->source->description);
      if (G_VALUE_HOLDS_STRING (&event->any_data))
        {
          event->source->description = g_value_dup_string (&event->any_data);
          _atspi_accessible_add_cache (event->source, ATSPI_CACHE_DESCRIPTION);
        }
      else
        {
          event->source->description = NULL;
          event->source->cached_properties &= ~ATSPI_CACHE_DESCRIPTION;
        }
    }
  else if (!strcmp (event->type, "object:property-change:accessible-role"))
    {
      if (G_VALUE_HOLDS_INT (&event->any_data))
        {
          event->source->role = g_value_get_int (&event->any_data);
          _atspi_accessible_add_cache (event->source, ATSPI_CACHE_ROLE);
        }
      else
        {
          event->source->cached_properties &= ~ATSPI_CACHE_ROLE;
        }
    }
}

static void
cache_process_state_changed (AtspiEvent *event)
{
  if (event->source->states)
    atspi_state_set_set_by_name (event->source->states, event->type + 21,
                                 event->detail1);
}

static void
cache_process_attributes_changed (AtspiEvent *event)
{
  const gchar *name = NULL, *value;

  if (!event->source->attributes)
    return;

  if (event->type[25] == ':')
    name = event->type + 26;
  value = g_value_get_string (&event->any_data);

  if (name && name[0] && value && value[0])
    {
      g_hash_table_remove (event->source->attributes, name);
      g_hash_table_insert (event->source->attributes, g_strdup (name), g_strdup (value));
    }
  else
    {
      g_clear_pointer (&event->source->attributes, g_hash_table_unref);
      event->source->attributes = NULL;
    }
}

static dbus_bool_t
demarshal_rect (DBusMessageIter *iter, AtspiRect *rect)
{
  dbus_int32_t x, y, width, height;
  DBusMessageIter iter_struct;

  dbus_message_iter_recurse (iter, &iter_struct);
  if (dbus_message_iter_get_arg_type (&iter_struct) != DBUS_TYPE_INT32)
    return FALSE;
  dbus_message_iter_get_basic (&iter_struct, &x);
  dbus_message_iter_next (&iter_struct);
  if (dbus_message_iter_get_arg_type (&iter_struct) != DBUS_TYPE_INT32)
    return FALSE;
  dbus_message_iter_get_basic (&iter_struct, &y);
  dbus_message_iter_next (&iter_struct);
  if (dbus_message_iter_get_arg_type (&iter_struct) != DBUS_TYPE_INT32)
    return FALSE;
  dbus_message_iter_get_basic (&iter_struct, &width);
  dbus_message_iter_next (&iter_struct);
  if (dbus_message_iter_get_arg_type (&iter_struct) != DBUS_TYPE_INT32)
    return FALSE;
  dbus_message_iter_get_basic (&iter_struct, &height);
  rect->x = x;
  rect->y = y;
  rect->width = width;
  rect->height = height;
  return TRUE;
}

static gboolean
convert_event_type_to_dbus (const char *eventType, char **categoryp, char **namep, char **detailp, AtspiAccessible *app, GPtrArray **matchrule_array)
{
  gchar *tmp = _atspi_strdup_and_adjust_for_dbus (eventType);
  char *category = NULL, *name = NULL, *detail = NULL;
  char *saveptr = NULL;

  if (tmp == NULL)
    return FALSE;
  category = strtok_r (tmp, ":", &saveptr);
  if (category)
    category = g_strdup (category);
  name = strtok_r (NULL, ":", &saveptr);
  if (name)
    {
      name = g_strdup (name);
      detail = strtok_r (NULL, ":", &saveptr);
      if (detail)
        detail = g_strdup (detail);
    }
  if (matchrule_array)
    {
      gchar *matchrule;
      (*matchrule_array) = g_ptr_array_new ();
      matchrule = g_strdup_printf ("type='signal',interface='org.a11y.atspi.Event.%s'", category);
      if (app)
        {
          gchar *new_str = g_strconcat (matchrule, ",sender='",
                                        app->parent.app->bus_name, "'",
                                        NULL);
          g_free (matchrule);
          matchrule = new_str;
        }
      if (name && name[0])
        {
          gchar *new_str = g_strconcat (matchrule, ",member='", name, "'", NULL);
          g_free (matchrule);
          matchrule = new_str;
        }
      if (detail && detail[0])
        {
          gchar *new_str = g_strconcat (matchrule, ",arg0='", detail, "'", NULL);
          g_ptr_array_add (*matchrule_array, new_str);
          new_str = g_strconcat (matchrule, ",arg0path='", detail, "/'", NULL);
          g_ptr_array_add (*matchrule_array, new_str);
          g_free (matchrule);
        }
      else
        g_ptr_array_add (*matchrule_array, matchrule);
    }
  if (categoryp)
    *categoryp = category;
  else
    g_free (category);
  if (namep)
    *namep = name;
  else if (name)
    g_free (name);
  if (detailp)
    *detailp = detail;
  else if (detail)
    g_free (detail);
  g_free (tmp);
  return TRUE;
}

static void
listener_entry_free (EventListenerEntry *e)
{
  gpointer callback = (e->callback == remove_datum ? (gpointer) e->user_data : (gpointer) e->callback);
  g_free (e->event_type);
  g_free (e->category);
  g_free (e->name);
  if (e->detail)
    g_free (e->detail);
  callback_unref (callback);

  for (int i = 0; i < e->properties->len; i++)
    g_free (g_array_index (e->properties, char *, i));

  g_array_free (e->properties, TRUE);

  if (e->app)
    g_object_unref (e->app);

  g_free (e);
}

/**
 * atspi_event_listener_register:
 * @listener: The #AtspiEventListener to register against an event type.
 * @event_type: a character string indicating the type of events for which
 *            notification is requested.  Format is
 *            EventClass:major_type:minor_type:detail
 *            where all subfields other than EventClass are optional.
 *            EventClasses include "object", "window", "mouse",
 *            and toolkit events (e.g. "Gtk", "AWT").
 *            Examples: "focus:", "Gtk:GtkWidget:button_press_event".
 *
 * Adds an in-process callback function to an existing #AtspiEventListener.
 *
 * Legal object event types:
 *
 *    (property change events)
 *
 *            object:property-change
 *            object:property-change:accessible-name
 *            object:property-change:accessible-description
 *            object:property-change:accessible-help-text
 *            object:property-change:accessible-parent
 *            object:property-change:accessible-value
 *            object:property-change:accessible-role
 *            object:property-change:accessible-table-caption
 *            object:property-change:accessible-table-column-description
 *            object:property-change:accessible-table-column-header
 *            object:property-change:accessible-table-row-description
 *            object:property-change:accessible-table-row-header
 *            object:property-change:accessible-table-summary
 *
 *    (other object events)
 *
 *            object:state-changed
 *            object:children-changed
 *            object:visible-data-changed
 *            object:selection-changed
 *            object:text-selection-changed
 *            object:text-changed
 *            object:text-caret-moved
 *            object:row-inserted
 *            object:row-reordered
 *            object:row-deleted
 *            object:column-inserted
 *            object:column-reordered
 *            object:column-deleted
 *            object:model-changed
 *            object:active-descendant-changed
 *            object:announcement
 *
 *  (screen reader events)
 *             screen-reader:region-changed
 *
 *  (window events)
 *
 *            window:minimize
 *            window:maximize
 *            window:restore
 *            window:close
 *            window:create
 *            window:reparent
 *            window:desktop-create
 *            window:desktop-destroy
 *            window:activate
 *            window:deactivate
 *            window:raise
 *            window:lower
 *            window:move
 *            window:resize
 *            window:shade
 *            window:unshade
 *            window:restyle
 *
 *  (other events)
 *
 *            focus:
 *            mouse:abs
 *            mouse:rel
 *            mouse:b1p
 *            mouse:b1r
 *            mouse:b2p
 *            mouse:b2r
 *            mouse:b3p
 *            mouse:b3r
 *
 * NOTE: this character string may be UTF-8, but should not contain byte
 * value 56
 *            (ascii ':'), except as a delimiter, since non-UTF-8 string
 *            delimiting functions are used internally.
 *            In general, listening to
 *            toolkit-specific events is not recommended.
 *
 * Currently, object:text-reading-position needs to be specified explicitly
 * (it is not implied by object:text), since it is generated by the screen
 * reader and is thus a special case internally.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_event_listener_register (AtspiEventListener *listener,
                               const gchar *event_type,
                               GError **error)
{
  /* TODO: Keep track of which events have been registered, so that we
   * deregister all of them when the event listener is destroyed */

  return atspi_event_listener_register_from_callback (listener->callback,
                                                      listener->user_data,
                                                      listener->cb_destroyed,
                                                      event_type, error);
}

/**
 * atspi_event_listener_register_full:
 * @listener: The #AtspiEventListener to register against an event type.
 * @event_type: a character string indicating the type of events for which
 *            notification is requested.  See #atspi_event_listener_register
 * for a description of the format and legal event types.
 * @properties: (element-type gchar*) (transfer none) (allow-none): a list of
 *             properties that should be sent along with the event. The
 *             properties are valued for the duration of the event callback.
 *             TODO: Document.
 *
 * Adds an in-process callback function to an existing #AtspiEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_event_listener_register_full (AtspiEventListener *listener,
                                    const gchar *event_type,
                                    GArray *properties,
                                    GError **error)
{
  /* TODO: Keep track of which events have been registered, so that we
   * deregister all of them when the event listener is destroyed */

  return atspi_event_listener_register_from_callback_full (listener->callback,
                                                           listener->user_data,
                                                           listener->cb_destroyed,
                                                           event_type,
                                                           properties,
                                                           error);
}

/**
 * atspi_event_listener_register_with_app:
 * @listener: The #AtspiEventListener to register against an event type.
 * @event_type: a character string indicating the type of events for which
 *            notification is requested.  See #atspi_event_listener_register
 * for a description of the format and legal event types.
 * @properties: (element-type gchar*) (transfer none) (allow-none): a list of
 *             properties that should be sent along with the event. The
 *             properties are valued for the duration of the event callback.
 * @app: (allow-none): the application whose events should be reported, or
 *      %null for all applications.
 *
 * Adds an in-process callback function to an existing #AtspiEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_event_listener_register_with_app (AtspiEventListener *listener,
                                        const gchar *event_type,
                                        GArray *properties,
                                        AtspiAccessible *app,
                                        GError **error)
{
  return atspi_event_listener_register_from_callback_with_app (listener->callback,
                                                               listener->user_data,
                                                               listener->cb_destroyed,
                                                               event_type,
                                                               properties,
                                                               app,
                                                               error);
}

static gboolean
notify_event_registered (EventListenerEntry *e)
{
  const char *app_path = (e->app ? e->app->parent.app->bus_name : "");

  dbind_method_call_reentrant (_atspi_bus (), atspi_bus_registry,
                               atspi_path_registry,
                               atspi_interface_registry,
                               "RegisterEvent",
                               NULL, "sass", e->event_type,
                               e->properties, app_path);

  return TRUE;
}

/**
 * atspi_event_listener_register_from_callback:
 * @callback: (scope notified) (closure user_data) (destroy callback_destroyed): the
 *   #AtspiEventListenerCB to be registered against an event type.
 * @user_data: User data to be passed to the callback.
 * @callback_destroyed: A #GDestroyNotify called when the callback is destroyed.
 * @event_type: a character string indicating the type of events for which
 *    notification is requested.  See atspi_event_listener_register()
 *    for a description of the format.
 *
 * Registers an #AtspiEventListenerCB against an @event_type.
 *
 * Returns: #TRUE if successfull, otherwise #FALSE.
 *
 **/
gboolean
atspi_event_listener_register_from_callback (AtspiEventListenerCB callback,
                                             void *user_data,
                                             GDestroyNotify callback_destroyed,
                                             const gchar *event_type,
                                             GError **error)
{
  return atspi_event_listener_register_from_callback_with_app (callback,
                                                               user_data,
                                                               callback_destroyed,
                                                               event_type, NULL,
                                                               NULL, error);
}

static GArray *
copy_event_properties (GArray *src)
{
  gint i;

  GArray *dst = g_array_new (FALSE, FALSE, sizeof (char *));

  if (!src)
    return dst;
  for (i = 0; i < src->len; i++)
    {
      gchar *dup = g_strdup (g_array_index (src, char *, i));
      g_array_append_val (dst, dup);
    }
  return dst;
}

/**
 * atspi_event_listener_register_from_callback_full:
 * @callback: (scope notified) (closure user_data) (destroy callback_destroyed): an
 *   #AtspiEventListenerCB function pointer.
 * @user_data:
 * @callback_destroyed:
 * @event_type:
 * @properties: (element-type utf8):
 * @error:
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
gboolean
atspi_event_listener_register_from_callback_full (AtspiEventListenerCB callback,
                                                  void *user_data,
                                                  GDestroyNotify callback_destroyed,
                                                  const gchar *event_type,
                                                  GArray *properties,
                                                  GError **error)
{
  return atspi_event_listener_register_from_callback_with_app (callback,
                                                               user_data,
                                                               callback_destroyed,
                                                               event_type, NULL,
                                                               NULL, error);
}

/**
 * atspi_event_listener_register_from_callback_with_app:
 * @callback: (scope notified) (closure user_data) (destroy callback_destroyed): an
 *   #AtspiEventListenerCB function pointer.
 * @user_data:
 * @callback_destroyed:
 * @event_type:
 * @properties: (element-type utf8):
 * @app: (nullable):
 * @error:
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
gboolean
atspi_event_listener_register_from_callback_with_app (AtspiEventListenerCB callback,
                                                      void *user_data,
                                                      GDestroyNotify callback_destroyed,
                                                      const gchar *event_type,
                                                      GArray *properties,
                                                      AtspiAccessible *app,
                                                      GError **error)
{
  EventListenerEntry *e;
  DBusError d_error;
  GPtrArray *matchrule_array;
  gint i;

  if (!callback)
    {
      return FALSE;
    }

  if (!event_type)
    {
      g_warning ("called atspi_event_listener_register_from_callback with a NULL event_type");
      return FALSE;
    }

  e = g_new0 (EventListenerEntry, 1);
  e->event_type = g_strdup (event_type);
  e->callback = callback;
  e->user_data = user_data;
  e->callback_destroyed = callback_destroyed;
  callback_ref (callback == remove_datum ? (gpointer) user_data : (gpointer) callback,
                callback_destroyed);
  if (!convert_event_type_to_dbus (event_type, &e->category, &e->name, &e->detail, app, &matchrule_array))
    {
      g_free (e->event_type);
      g_free (e);
      return FALSE;
    }
  if (app)
    e->app = g_object_ref (app);
  e->properties = copy_event_properties (properties);
  event_listeners = g_list_prepend (event_listeners, e);
  for (i = 0; i < matchrule_array->len; i++)
    {
      char *matchrule = g_ptr_array_index (matchrule_array, i);
      dbus_error_init (&d_error);
      dbus_bus_add_match (_atspi_bus (), matchrule, &d_error);
      if (dbus_error_is_set (&d_error))
        {
          g_warning ("AT-SPI: Adding match: %s", d_error.message);
          dbus_error_free (&d_error);
          /* TODO: Set error */
        }

      g_free (matchrule);
    }
  g_ptr_array_free (matchrule_array, TRUE);

  notify_event_registered (e);
  return TRUE;
}

void
_atspi_reregister_event_listeners ()
{
  GList *l;
  EventListenerEntry *e;

  for (l = event_listeners; l; l = l->next)
    {
      e = l->data;
      notify_event_registered (e);
    }
}

/**
 * atspi_event_listener_register_no_data: (skip)
 * @callback: (scope notified): the #AtspiEventListenerSimpleCB to be
 *            registered against an event type.
 * @callback_destroyed: A #GDestroyNotify called when the callback is destroyed.
 * @event_type: a character string indicating the type of events for which
 *            notification is requested.  Format is
 *            EventClass:major_type:minor_type:detail
 *            where all subfields other than EventClass are optional.
 *            EventClasses include "object", "window", "mouse",
 *            and toolkit events (e.g. "Gtk", "AWT").
 *            Examples: "focus:", "Gtk:GtkWidget:button_press_event".
 *
 * Registers an #AtspiEventListenetSimpleCB. The method is similar to
 * #atspi_event_listener_register, but @callback takes no user_data.
 *
 * Returns: #TRUE if successfull, otherwise #FALSE.
 **/
gboolean
atspi_event_listener_register_no_data (AtspiEventListenerSimpleCB callback,
                                       GDestroyNotify callback_destroyed,
                                       const gchar *event_type,
                                       GError **error)
{
  return atspi_event_listener_register_from_callback (remove_datum, callback,
                                                      callback_destroyed,
                                                      event_type, error);
}

static gboolean
is_superset (const gchar *super, const gchar *sub)
{
  if (!super || !super[0])
    return TRUE;
  if (!sub || !sub[0])
    return FALSE;
  return (strcmp (super, sub) == 0);
}

/**
 * atspi_event_listener_deregister:
 * @listener: The #AtspiEventListener to deregister.
 * @event_type: a string specifying the event type for which this
 *             listener is to be deregistered.
 *
 * Deregisters an #AtspiEventListener from the registry, for a specific
 *             event type.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_event_listener_deregister (AtspiEventListener *listener,
                                 const gchar *event_type,
                                 GError **error)
{
  return atspi_event_listener_deregister_from_callback (listener->callback,
                                                        listener->user_data,
                                                        event_type, error);
}

/**
 * atspi_event_listener_deregister_from_callback:
 * @callback: (scope call) (closure user_data): the #AtspiEventListenerCB
 *   registered against an event type.
 * @user_data: User data that was passed in for this callback.
 * @event_type: a string specifying the event type for which this
 *   listener is to be deregistered.
 *
 * Deregisters an #AtspiEventListenerCB from the registry, for a specific
 * event type.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_event_listener_deregister_from_callback (AtspiEventListenerCB callback,
                                               void *user_data,
                                               const gchar *event_type,
                                               GError **error)
{
  char *category, *name, *detail;
  GPtrArray *matchrule_array;
  gint i;
  GList *l;

  if (!convert_event_type_to_dbus (event_type, &category, &name, &detail, NULL, &matchrule_array))
    {
      return FALSE;
    }
  if (!callback)
    {
      return FALSE;
    }

  for (l = event_listeners; l;)
    {
      EventListenerEntry *e = l->data;
      if (e->callback == callback &&
          e->user_data == user_data &&
          is_superset (category, e->category) &&
          is_superset (name, e->name) &&
          is_superset (detail, e->detail))
        {
          DBusMessage *message, *reply;
          l = g_list_next (l);
          if (in_send)
            {
              pending_removals = g_list_remove (pending_removals, e);
              pending_removals = g_list_append (pending_removals, e);
            }
          else
            event_listeners = g_list_remove (event_listeners, e);
          for (i = 0; i < matchrule_array->len; i++)
            {
              char *matchrule = g_ptr_array_index (matchrule_array, i);
              dbus_bus_remove_match (_atspi_bus (), matchrule, NULL);
            }
          message = dbus_message_new_method_call (atspi_bus_registry,
                                                  atspi_path_registry,
                                                  atspi_interface_registry,
                                                  "DeregisterEvent");
          if (!message)
            return FALSE;
          dbus_message_append_args (message, DBUS_TYPE_STRING, &event_type, DBUS_TYPE_INVALID);
          reply = _atspi_dbus_send_with_reply_and_block (message, error);
          if (reply)
            dbus_message_unref (reply);

          if (!in_send)
            listener_entry_free (e);
        }
      else
        l = g_list_next (l);
    }
  g_free (category);
  g_free (name);
  if (detail)
    g_free (detail);
  for (i = 0; i < matchrule_array->len; i++)
    g_free (g_ptr_array_index (matchrule_array, i));
  g_ptr_array_free (matchrule_array, TRUE);
  return TRUE;
}

/**
 * atspi_event_listener_deregister_no_data: (skip)
 * @callback: (scope call): the #AtspiEventListenerSimpleCB registered against
 *            an event type.
 * @event_type: a string specifying the event type for which this
 *             listener is to be deregistered.
 *
 * deregisters an #AtspiEventListenerSimpleCB from the registry, for a specific
 *             event type.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 **/
gboolean
atspi_event_listener_deregister_no_data (AtspiEventListenerSimpleCB callback,
                                         const gchar *event_type,
                                         GError **error)
{
  return atspi_event_listener_deregister_from_callback (remove_datum, callback,
                                                        event_type,
                                                        error);
}

static AtspiEvent *
atspi_event_copy (AtspiEvent *src)
{
  AtspiEvent *dst = g_new0 (AtspiEvent, 1);
  dst->type = g_strdup (src->type);
  dst->source = g_object_ref (src->source);
  dst->detail1 = src->detail1;
  dst->detail2 = src->detail2;
  g_value_init (&dst->any_data, G_VALUE_TYPE (&src->any_data));
  g_value_copy (&src->any_data, &dst->any_data);
  if (src->sender)
    dst->sender = g_object_ref (src->sender);
  return dst;
}

static void
atspi_event_free (AtspiEvent *event)
{
  g_object_unref (event->source);
  g_free (event->type);
  g_value_unset (&event->any_data);
  g_clear_object (&event->sender);
  g_free (event);
}

static gboolean
detail_matches_listener (const char *event_detail, const char *listener_detail)
{
  if (!listener_detail)
    return TRUE;

  if (!event_detail)
    return (listener_detail ? FALSE : TRUE);

  return !(listener_detail[strcspn (listener_detail, ":")] == '\0'
               ? strncmp (listener_detail, event_detail,
                          strcspn (event_detail, ":"))
               : strcmp (listener_detail, event_detail));
}

static void
resolve_pending_removal (gpointer data)
{
  event_listeners = g_list_remove (event_listeners, data);
  listener_entry_free (data);
}

static gboolean
should_filter_window_events ()
{
  struct timeval cur_time, elapsed_time;

  if (!window_filter_time.tv_sec && !window_filter_time.tv_usec)
    return FALSE;

  gettimeofday (&cur_time, NULL);
  timersub (&cur_time, &window_filter_time, &elapsed_time);

  return (elapsed_time.tv_sec == 0 && elapsed_time.tv_usec < 20000);
}

void
_atspi_send_event (AtspiEvent *e)
{
  char *category, *name, *detail;
  GList *l;
  GList *called_listeners = NULL;

  /* Ensure that the value is set to avoid a Python exception */
  /* TODO: Figure out how to do this without using a private field */
  if (e->any_data.g_type == 0)
    {
      g_value_init (&e->any_data, G_TYPE_INT);
      g_value_set_int (&e->any_data, 0);
    }

  if (!convert_event_type_to_dbus (e->type, &category, &name, &detail, NULL,
                                   NULL))
    {
      g_warning ("AT-SPI: Couldn't parse event: %s\n", e->type);
      return;
    }

  if (!strcmp (category, "Window") && should_filter_window_events ())
    return;

  in_send++;
  for (l = event_listeners; l; l = g_list_next (l))
    {
      EventListenerEntry *entry = l->data;
      if (!strcmp (category, entry->category) &&
          (entry->name == NULL || !strcmp (name, entry->name)) &&
          detail_matches_listener (detail, entry->detail) &&
          (entry->app == NULL || !strcmp (entry->app->parent.app->bus_name,
                                          e->source->parent.app->bus_name)))
        {
          GList *l2;
          for (l2 = called_listeners; l2; l2 = l2->next)
            {
              EventListenerEntry *e2 = l2->data;
              if (entry->callback == e2->callback && entry->user_data == e2->user_data)
                break;
            }
          if (!l2)
            {
              for (l2 = pending_removals; l2; l2 = l2->next)
                {
                  if (l2->data == entry)
                    break;
                }
            }
          if (!l2)
            {
              entry->callback (atspi_event_copy (e), entry->user_data);
              called_listeners = g_list_prepend (called_listeners, entry);
            }
        }
    }
  in_send--;
  if (detail)
    g_free (detail);
  g_free (name);
  g_free (category);
  g_list_free (called_listeners);

  g_list_free_full (pending_removals, resolve_pending_removal);
  pending_removals = NULL;
}

void
_atspi_dbus_handle_event (DBusMessage *message)
{
  char *detail = NULL;
  const char *category = dbus_message_get_interface (message);
  const char *sender = dbus_message_get_sender (message);
  const char *member = dbus_message_get_member (message);
  const char *signature = dbus_message_get_signature (message);
  gchar *name;
  gchar *converted_type;
  DBusMessageIter iter, iter_variant;
  dbus_message_iter_init (message, &iter);
  AtspiEvent e;
  dbus_int32_t detail1, detail2;
  char *p;
  GHashTable *cache = NULL;

  g_assert (strncmp (category, "org.a11y.atspi.Event.", 21) == 0);

  if (strcmp (signature, "siiv(so)") != 0 &&
      strcmp (signature, "siiva{sv}") != 0)
    {
      g_warning ("Got invalid signature %s for signal %s from interface %s\n", signature, member, category);
      return;
    }

  memset (&e, 0, sizeof (e));

  /* Find the plain interface name, e.g. "org.a11y.atspi.Event.ScreenReader" -> "ScreenReader" */
  category = g_utf8_strrchr (category, -1, '.');
  g_assert (category != NULL);
  category++;

  dbus_message_iter_get_basic (&iter, &detail);
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &detail1);
  e.detail1 = detail1;
  dbus_message_iter_next (&iter);
  dbus_message_iter_get_basic (&iter, &detail2);
  e.detail2 = detail2;
  dbus_message_iter_next (&iter);

  converted_type = convert_name_from_dbus (category, FALSE);
  name = convert_name_from_dbus (member, FALSE);
  detail = convert_name_from_dbus (detail, TRUE);

  if (strcasecmp (category, name) != 0)
    {
      p = g_strconcat (converted_type, ":", name, NULL);
      g_free (converted_type);
      converted_type = p;
    }
  else if (detail[0] == '\0')
    {
      p = g_strconcat (converted_type, ":", NULL);
      g_free (converted_type);
      converted_type = p;
    }

  if (detail[0] != '\0')
    {
      p = g_strconcat (converted_type, ":", detail, NULL);
      g_free (converted_type);
      converted_type = p;
    }
  e.type = converted_type;
  if (strcmp (category, "ScreenReader") != 0)
    {
      e.source = _atspi_ref_accessible (sender, dbus_message_get_path (message));
      if (e.source == NULL)
        {
          g_warning ("Got no valid source accessible for signal %s from interface %s\n", member, category);
          g_free (converted_type);
          g_free (name);
          g_free (detail);
          return;
        }
    }

  dbus_message_iter_recurse (&iter, &iter_variant);
  switch (dbus_message_iter_get_arg_type (&iter_variant))
    {
    case DBUS_TYPE_STRUCT:
      {
        AtspiRect rect;
        if (demarshal_rect (&iter_variant, &rect))
          {
            g_value_init (&e.any_data, ATSPI_TYPE_RECT);
            g_value_set_boxed (&e.any_data, &rect);
          }
        else
          {
            AtspiAccessible *accessible;
            accessible = _atspi_dbus_consume_accessible (&iter_variant);
            if (!strcmp (category, "ScreenReader"))
              {
                g_object_unref (e.source);
                e.source = accessible;
              }
            else
              {
                g_value_init (&e.any_data, ATSPI_TYPE_ACCESSIBLE);
                g_value_set_instance (&e.any_data, accessible);
                if (accessible)
                  g_object_unref (accessible); /* value now owns it */
              }
          }
        break;
      }
    case DBUS_TYPE_STRING:
      {
        dbus_message_iter_get_basic (&iter_variant, &p);
        g_value_init (&e.any_data, G_TYPE_STRING);
        g_value_set_string (&e.any_data, p);
        break;
      }
    default:
      break;
    }

  g_assert (e.source != NULL);

  dbus_message_iter_next (&iter);
  if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_ARRAY)
    {
      /* new form -- parse properties sent with event */
      cache = _atspi_dbus_update_cache_from_dict (e.source, &iter);
    }

  e.sender = _atspi_ref_accessible (sender, ATSPI_DBUS_PATH_ROOT);

  if (!strncmp (e.type, "object:children-changed", 23))
    {
      cache_process_children_changed (&e);
    }
  else if (!strncmp (e.type, "object:property-change", 22))
    {
      cache_process_property_change (&e);
    }
  else if (!strncmp (e.type, "object:state-changed", 20))
    {
      cache_process_state_changed (&e);
    }
  else if (!strncmp (e.type, "object:attributes-changed", 25))
    {
      cache_process_attributes_changed (&e);
    }
  else if (!strncmp (e.type, "focus", 5))
    {
      /* BGO#663992 - TODO: figure out the real problem */
      e.source->cached_properties &= ~(ATSPI_CACHE_STATES);
    }

  _atspi_send_event (&e);

  if (cache)
    _atspi_accessible_unref_cache (e.source);

  g_free (converted_type);
  g_free (name);
  g_free (detail);
  g_object_unref (e.source);
  g_object_unref (e.sender);
  g_value_unset (&e.any_data);
}

void
_atspi_update_window_filter_time ()
{
  gettimeofday (&window_filter_time, NULL);
}

G_DEFINE_BOXED_TYPE (AtspiEvent, atspi_event, atspi_event_copy, atspi_event_free)
