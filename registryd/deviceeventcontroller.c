/* AT-SPI - Assistive Technology Service Provider Interface
 *
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2003 Sun Microsystems Inc.,
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

/* deviceeventcontroller.c: implement the DeviceEventController interface */

#include <config.h>

#undef  SPI_XKB_DEBUG
#undef  SPI_DEBUG
#undef  SPI_KEYEVENT_DEBUG

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>

#include <glib.h>

#include <dbus/dbus.h>

#include "paths.h"
#include "keymasks.h"
#include "de-types.h"
#include "de-marshaller.h"
#include "display.h"
#include "event-source.h"

#include "deviceeventcontroller.h"
#include "reentrant-list.h"

#include "introspection.h"

#define CHECK_RELEASE_DELAY 20
#define BIT(c, x)       (c[x/8]&(1<<(x%8)))
static SpiDEController *saved_controller;

/* Our parent Gtk object type */
#define PARENT_TYPE G_TYPE_OBJECT

/* A pointer to our parent object class */
static int spi_error_code = 0;
struct _SpiPoint {
    gint x;
    gint y;
};
typedef struct _SpiPoint SpiPoint;
static unsigned int mouse_mask_state = 0;
static unsigned int key_modifier_mask =
  Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask | ShiftMask | LockMask | ControlMask | SPI_KEYMASK_NUMLOCK;
static unsigned int _numlock_physical_mask = Mod2Mask; /* a guess, will be reset */

static gboolean have_mouse_listener = FALSE;
static gboolean have_mouse_event_listener = FALSE;


typedef struct {
  guint                             ref_count : 30;
  guint                             pending_add : 1;
  guint                             pending_remove : 1;

  Accessibility_ControllerEventMask mod_mask;
  dbus_uint32_t               key_val;  /* KeyCode */
} DEControllerGrabMask;


gboolean spi_controller_update_key_grabs               (SpiDEController           *controller,
							       Accessibility_DeviceEvent *recv);

static gboolean eventtype_seq_contains_event (dbus_uint32_t types,
					      const Accessibility_DeviceEvent *event);
static gboolean spi_dec_poll_mouse_moving (gpointer data);
static gboolean spi_dec_poll_mouse_idle (gpointer data);

G_DEFINE_TYPE(SpiDEController, spi_device_event_controller, G_TYPE_OBJECT)

static gint
spi_dec_plat_get_keycode (SpiDEController *controller,
                          gint keysym,
                          gchar *key_str,
                          gboolean fix,
                          guint *modmask)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.get_keycode)
    return klass->plat.get_keycode (controller, keysym, key_str, fix, modmask);
  else
    return keysym;
}

static guint
spi_dec_plat_mouse_check (SpiDEController *controller, 
		     int *x, int *y, gboolean *moved)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.mouse_check)
    return klass->plat.mouse_check (controller, x, y, moved);
  else
    return 0;
}

static gboolean
spi_dec_plat_grab_key (SpiDEController *controller, guint key_val, Accessibility_ControllerEventMask mod_mask)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.grab_key)
    return klass->plat.grab_key (controller, key_val, mod_mask);
  else
    return FALSE;
}

static void
spi_dec_plat_ungrab_key (SpiDEController *controller, guint key_val, Accessibility_ControllerEventMask mod_mask)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.ungrab_key)
    klass->plat.ungrab_key (controller, key_val, mod_mask);
}

static gboolean
spi_dec_plat_synth_keycode_press (SpiDEController *controller,
			 unsigned int keycode)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.synth_keycode_press)
    return klass->plat.synth_keycode_press (controller, keycode);
  else
    return FALSE;
}

static gboolean
spi_dec_plat_synth_keycode_release (SpiDEController *controller,
			   unsigned int keycode)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.synth_keycode_release)
    return klass->plat.synth_keycode_release (controller, keycode);
  else
    return FALSE;
}

static gboolean
spi_dec_plat_lock_modifiers (SpiDEController *controller, unsigned modifiers)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.lock_modifiers)
    return klass->plat.lock_modifiers (controller, modifiers);
  else
    return FALSE;
}

static gboolean
spi_dec_plat_unlock_modifiers (SpiDEController *controller, unsigned modifiers)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.unlock_modifiers)
    return klass->plat.unlock_modifiers (controller, modifiers);
  else
    return FALSE;
}

static gboolean
spi_dec_plat_synth_keystring (SpiDEController *controller, guint synth_type, gint keycode, const char *keystring)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.synth_keystring)
    return klass->plat.synth_keystring (controller, synth_type, keycode, keystring);
  else
    return FALSE;
}

static void
spi_dec_plat_emit_modifier_event (SpiDEController *controller, guint prev_mask, 
			     guint current_mask)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.emit_modifier_event)
    return klass->plat.emit_modifier_event (controller, prev_mask, current_mask);
}

static void
spi_dec_plat_generate_mouse_event (SpiDEController *controller,
                                   gint x,
                                   gint y,
                                   const char *eventName)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
  if (klass->plat.generate_mouse_event)
    return klass->plat.generate_mouse_event (controller, x, y, eventName);
}

DBusMessage *
invalid_arguments_error (DBusMessage *message)
{
    DBusMessage *reply;
    gchar       *errmsg;

    errmsg= g_strdup_printf (
            "Method \"%s\" with signature \"%s\" on interface \"%s\" was supplied with invalid arguments\n",
            dbus_message_get_member (message),
            dbus_message_get_signature (message),
            dbus_message_get_interface (message));
    reply = dbus_message_new_error (message,
                                    DBUS_ERROR_INVALID_ARGS,
                                    errmsg);
    g_free (errmsg);
    return reply;
}

/* Private methods */
static dbus_bool_t
spi_dbus_add_disconnect_match (DBusConnection *bus, const char *name)
{
  char *match = g_strdup_printf ("interface=%s,member=NameOwnerChanged,arg0=%s", DBUS_INTERFACE_DBUS, name);
  if (match)
  {
    DBusError error;
    dbus_error_init (&error);
    dbus_bus_add_match (bus, match, &error);
    g_free (match);
    if (dbus_error_is_set (&error))
      {
        dbus_error_free (&error);
        return FALSE;
      }
    else
      {
        return TRUE;
      }
  }
  else return FALSE;
}

static dbus_bool_t
spi_dbus_remove_disconnect_match (DBusConnection *bus, const char *name)
{
  char *match = g_strdup_printf ("interface=%s,member=NameOwnerChanged,arg0=%s", DBUS_INTERFACE_DBUS, name);
  if (match)
  {
    DBusError error;
    dbus_error_init (&error);
    dbus_bus_remove_match (bus, match, &error);
    g_free (match);
    if (dbus_error_is_set (&error))
      {
        dbus_error_free (&error);
        return FALSE;
      }
    else
      {
        return TRUE;
      }
  }
  else return FALSE;
}

static DEControllerGrabMask *
spi_grab_mask_clone (DEControllerGrabMask *grab_mask)
{
  DEControllerGrabMask *clone = g_new (DEControllerGrabMask, 1);

  memcpy (clone, grab_mask, sizeof (DEControllerGrabMask));

  clone->ref_count = 1;
  clone->pending_add = TRUE;
  clone->pending_remove = FALSE;

  return clone;
}

static void
spi_grab_mask_free (DEControllerGrabMask *grab_mask)
{
  g_free (grab_mask);
}

static gint
spi_grab_mask_compare_values (gconstpointer p1, gconstpointer p2)
{
  DEControllerGrabMask *l1 = (DEControllerGrabMask *) p1;
  DEControllerGrabMask *l2 = (DEControllerGrabMask *) p2;

  if (p1 == p2)
    {
      return 0;
    }
  else
    { 
      return ((l1->mod_mask != l2->mod_mask) || (l1->key_val != l2->key_val));
    }
}

void
spi_dec_dbus_emit (SpiDEController *controller, const char *interface,
                   const char *name, const char *minor, int a1, int a2)
{
  DBusMessage *signal = NULL;
  DBusMessageIter iter, iter_struct, iter_variant;
  int nil = 0;
  const char *path = SPI_DBUS_PATH_ROOT;
  const char *bus_name = dbus_bus_get_unique_name (controller->bus);

  signal = dbus_message_new_signal (path, interface, name);

  dbus_message_iter_init_append (signal, &iter);

  dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &minor);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &a1);
  dbus_message_iter_append_basic (&iter, DBUS_TYPE_INT32, &a2);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_VARIANT, "i", &iter_variant);
      dbus_message_iter_append_basic (&iter_variant, DBUS_TYPE_INT32, &nil);
  dbus_message_iter_close_container (&iter, &iter_variant);

  dbus_message_iter_open_container (&iter, DBUS_TYPE_STRUCT, NULL,
                                    &iter_struct);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING, &bus_name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH, &path);
  dbus_message_iter_close_container (&iter, &iter_struct);

  dbus_connection_send (controller->bus, signal, NULL);
  dbus_message_unref (signal);
}

static gboolean
spi_dec_poll_mouse_moved (gpointer data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(data);
  int x, y;
  gboolean moved;
  guint mask_return;

  mask_return = spi_dec_plat_mouse_check (controller, &x, &y, &moved);

  if ((mask_return & key_modifier_mask) !=
      (mouse_mask_state & key_modifier_mask)) 
    {
      spi_dec_plat_emit_modifier_event (controller, mouse_mask_state, mask_return);
      mouse_mask_state = mask_return;
    }

  return moved;
}

static gboolean
spi_dec_poll_mouse_idle (gpointer data)
{
  if (!have_mouse_event_listener && !have_mouse_listener)
    return FALSE;
  else if (!spi_dec_poll_mouse_moved (data))
    return TRUE;
  else
    {
      guint id;
      id = g_timeout_add (20, spi_dec_poll_mouse_moving, data);
      g_source_set_name_by_id (id, "[at-spi2-core] spi_dec_poll_mouse_moving");
      return FALSE;	    
    }
}

static gboolean
spi_dec_poll_mouse_moving (gpointer data)
{
  if (!have_mouse_event_listener && !have_mouse_listener)
    return FALSE;
  else if (spi_dec_poll_mouse_moved (data))
    return TRUE;
  else
    {
      guint id;
      id = g_timeout_add (100, spi_dec_poll_mouse_idle, data);
      g_source_set_name_by_id (id, "[at-spi2-core] check_release");
      return FALSE;
    }
}

/**
 * Eventually we can use this to make the marshalling of mask types
 * more sane, but for now we just use this to detect 
 * the use of 'virtual' masks such as numlock and convert them to
 * system-specific mask values (i.e. ModMask).
 * 
 **/
static Accessibility_ControllerEventMask
spi_dec_translate_mask (Accessibility_ControllerEventMask mask)
{
  Accessibility_ControllerEventMask tmp_mask;
  gboolean has_numlock;

  has_numlock = (mask & SPI_KEYMASK_NUMLOCK);
  tmp_mask = mask;
  if (has_numlock)
    {
      tmp_mask = mask ^ SPI_KEYMASK_NUMLOCK;
      tmp_mask |= _numlock_physical_mask;
    }
 
  return tmp_mask;
}

static DEControllerKeyListener *
spi_dec_key_listener_new (const char *bus_name,
			  const char *path,
			  GSList *keys,
			  const Accessibility_ControllerEventMask mask,
			  const dbus_uint32_t types,
			  const Accessibility_EventListenerMode  *mode)
{
  DEControllerKeyListener *key_listener = g_new0 (DEControllerKeyListener, 1);
  key_listener->listener.bus_name = g_strdup(bus_name);
  key_listener->listener.path = g_strdup(path);
  key_listener->listener.type = SPI_DEVICE_TYPE_KBD;
  key_listener->keys = keys;
  key_listener->mask = spi_dec_translate_mask (mask);
  key_listener->listener.types = types;
  if (mode)
  {
    key_listener->mode = (Accessibility_EventListenerMode *) g_malloc(sizeof(Accessibility_EventListenerMode));
    memcpy(key_listener->mode, mode, sizeof(*mode));
  }
  else
    key_listener->mode = NULL;

#ifdef SPI_DEBUG
  g_print ("new listener, with mask %x, is_global %d, keys %p (%d)\n",
	   (unsigned int) key_listener->mask,
           (int) (mode ? mode->global : 0),
	   (void *) key_listener->keys,
	   (int) (key_listener->keys ? g_slist_length(key_listener->keys) : 0));
#endif

  return key_listener;	
}

static DEControllerListener *
spi_dec_listener_new (const char *bus_name,
		      const char *path,
		      dbus_uint32_t types)
{
  DEControllerListener *listener = g_new0 (DEControllerListener, 1);
  listener->bus_name = g_strdup(bus_name);
  listener->path = g_strdup(path);
  listener->type = SPI_DEVICE_TYPE_MOUSE;
  listener->types = types;
  return listener;	
}

static DEControllerListener *
spi_listener_clone (DEControllerListener *listener)
{
  DEControllerListener *clone = g_new0 (DEControllerListener, 1);
  clone->bus_name = g_strdup (listener->bus_name);
  clone->path = g_strdup (listener->path);
  clone->type = listener->type;
  clone->types = listener->types;
  return clone;
}

static GSList *keylist_clone (GSList *s)
{
  GSList *d = NULL;
  GSList *l;

  for (l = s; l; l = g_slist_next(l))
  {
    Accessibility_KeyDefinition *kd = (Accessibility_KeyDefinition *)g_malloc(sizeof(Accessibility_KeyDefinition));
    if (kd)
    {
      Accessibility_KeyDefinition *kds = (Accessibility_KeyDefinition *)l->data;
      kd->keycode = kds->keycode;
      kd->keysym = kds->keysym;
      kd->keystring = g_strdup(kds->keystring);
      d = g_slist_append(d, kd);
    }
  }
  return d;
}

static DEControllerKeyListener *
spi_key_listener_clone (DEControllerKeyListener *key_listener)
{
  DEControllerKeyListener *clone = g_new0 (DEControllerKeyListener, 1);
  clone->listener.bus_name = g_strdup (key_listener->listener.bus_name);
  clone->listener.path = g_strdup (key_listener->listener.path);
  clone->listener.type = SPI_DEVICE_TYPE_KBD;
  clone->keys = keylist_clone (key_listener->keys);
  clone->mask = key_listener->mask;
  clone->listener.types = key_listener->listener.types;
  if (key_listener->mode)
  {
    clone->mode = (Accessibility_EventListenerMode *)g_malloc(sizeof(Accessibility_EventListenerMode));
    if (clone->mode) memcpy(clone->mode, key_listener->mode, sizeof(Accessibility_EventListenerMode));
  }
  else
    clone->mode = NULL;
  return clone;
}

static void keylist_free(GSList *keys)
{
  GSList *l;

  for (l = keys; l; l = g_slist_next(l))
  {
    Accessibility_KeyDefinition *kd = (Accessibility_KeyDefinition *)l->data;
    g_free(kd->keystring);
    g_free(kd);
  }
  g_slist_free (keys);
}

static void
spi_key_listener_data_free (DEControllerKeyListener *key_listener)
{
  keylist_free(key_listener->keys);
  if (key_listener->mode) g_free(key_listener->mode);
  g_free (key_listener->listener.bus_name);
  g_free (key_listener->listener.path);
  g_free (key_listener);
}

static void
spi_key_listener_clone_free (DEControllerKeyListener *clone)
{
  spi_key_listener_data_free (clone);
}

static void
spi_listener_clone_free (DEControllerListener *clone)
{
  g_free (clone->path);
  g_free (clone->bus_name);
  g_free (clone);
}

static void
spi_dec_listener_free (DEControllerListener    *listener)
{
  if (listener->type == SPI_DEVICE_TYPE_KBD) 
    spi_key_listener_data_free ((DEControllerKeyListener *) listener);
  else
  {
    g_free (listener->bus_name);
    g_free (listener->path);
  }
}

static void
_register_keygrab (SpiDEController      *controller,
		   DEControllerGrabMask *grab_mask)
{
  GList *l;

  l = g_list_find_custom (controller->keygrabs_list, grab_mask,
			  spi_grab_mask_compare_values);
  if (l)
    {
      DEControllerGrabMask *cur_mask = l->data;

      cur_mask->ref_count++;
      if (cur_mask->pending_remove)
        {
          cur_mask->pending_remove = FALSE;
	}
    }
  else
    {
      controller->keygrabs_list =
        g_list_prepend (controller->keygrabs_list,
			spi_grab_mask_clone (grab_mask));
    }
}

static void
_deregister_keygrab (SpiDEController      *controller,
		     DEControllerGrabMask *grab_mask)
{
  GList *l;

  l = g_list_find_custom (controller->keygrabs_list, grab_mask,
			  spi_grab_mask_compare_values);

  if (l)
    {
      DEControllerGrabMask *cur_mask = l->data;

      cur_mask->ref_count--;
      if (cur_mask->ref_count <= 0)
        {
          cur_mask->pending_remove = TRUE;
	}
    }
}

static void
handle_keygrab (SpiDEController         *controller,
		DEControllerKeyListener *key_listener,
		void                   (*process_cb) (SpiDEController *controller,
						      DEControllerGrabMask *grab_mask))
{
  DEControllerGrabMask grab_mask = { 0 };

  grab_mask.mod_mask = key_listener->mask;
  if (g_slist_length (key_listener->keys) == 0) /* special case means AnyKey/AllKeys */
    {
      grab_mask.key_val = AnyKey;
#ifdef SPI_DEBUG
      fprintf (stderr, "AnyKey grab!");
#endif
      process_cb (controller, &grab_mask);
    }
  else
    {
      GSList *l;

      for (l = key_listener->keys; l; l = g_slist_next(l))
        {
	  Accessibility_KeyDefinition *keydef = l->data;
	  long key_val;
	  key_val = spi_dec_plat_get_keycode (controller, keydef->keysym, keydef->keystring, FALSE, NULL);
	  if (!key_val)
	    key_val = keydef->keycode;
	  grab_mask.key_val = key_val;
	  process_cb (controller, &grab_mask);
	}
    }
}

static gboolean
spi_controller_register_global_keygrabs (SpiDEController         *controller,
					 DEControllerKeyListener *key_listener)
{
  handle_keygrab (controller, key_listener, _register_keygrab);
  return spi_controller_update_key_grabs (controller, NULL);
}

static void
spi_controller_deregister_global_keygrabs (SpiDEController         *controller,
					   DEControllerKeyListener *key_listener)
{
  handle_keygrab (controller, key_listener, _deregister_keygrab);
  spi_controller_update_key_grabs (controller, NULL);
}

static void
append_keystroke_listener (DBusMessageIter *iter, DEControllerKeyListener *listener)
{
  dbus_uint32_t d_uint;
  DBusMessageIter iter_struct, iter_subarray, iter_substruct;
  GSList *kl;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                         &iter_struct))
    return;

  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING,
                                  &listener->listener.bus_name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH,
                                  &listener->listener.path);
  d_uint = listener->listener.type;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &d_uint);
  d_uint = listener->listener.types;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &d_uint);
  if (!dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_ARRAY,
                                         "(iisi)", &iter_subarray))
  {
    dbus_message_iter_close_container (iter, &iter_struct);
    return;
  }
  for (kl = listener->keys; kl; kl = kl->next)
  {
    Accessibility_KeyDefinition *kd = kl->data;
    if (!dbus_message_iter_open_container (&iter_subarray, DBUS_TYPE_STRUCT,
                                         NULL, &iter_substruct))
      break;
    dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_INT32, &kd->keycode);
    dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_INT32, &kd->keysym);
    dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_STRING, &kd->keystring);
    dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_INT32, &kd->unused);
    dbus_message_iter_close_container (&iter_subarray, &iter_substruct);
  }
  dbus_message_iter_close_container (&iter_struct, &iter_subarray);
  d_uint = listener->mask;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &d_uint);
  if (dbus_message_iter_open_container (&iter_struct, DBUS_TYPE_STRUCT,
                                         NULL, &iter_substruct))
  {
    if (listener->mode)
    {
      dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_BOOLEAN,
                                      &listener->mode->synchronous);
      dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_BOOLEAN,
                                      &listener->mode->preemptive);
      dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_BOOLEAN,
                                      &listener->mode->global);
    }
    else
    {
      dbus_bool_t dummy_val = FALSE;
      dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_BOOLEAN,
                                      &dummy_val);
      dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_BOOLEAN,
                                      &dummy_val);
      dbus_message_iter_append_basic (&iter_substruct, DBUS_TYPE_BOOLEAN,
                                      &dummy_val);
    }
    dbus_message_iter_close_container (&iter_struct, &iter_substruct);
  }
  dbus_message_iter_close_container (iter, &iter_struct);
}

static void
notify_keystroke_listener (SpiDEController *controller,
                           DEControllerKeyListener *listener,
                           gboolean enable)
{
  const char *path = SPI_DBUS_PATH_DEC;
  const char *interface = SPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER;
  const char *name = (enable
                      ? "KeystrokeListenerRegistered"
                      : "KeystrokeListenerDeregistered");
  DBusMessage *signal;
  DBusMessageIter iter;

  signal = dbus_message_new_signal (path, interface, name);
  if (!signal)
    return;
  dbus_message_iter_init_append (signal, &iter);
  append_keystroke_listener (&iter, listener);
  dbus_connection_send (controller->bus, signal, NULL);
  dbus_message_unref (signal);
}

static void
append_mouse_listener (DBusMessageIter *iter, DEControllerListener *listener)
{
  DBusMessageIter iter_struct;
  dbus_uint32_t d_uint;

  if (!dbus_message_iter_open_container (iter, DBUS_TYPE_STRUCT, NULL,
                                         &iter_struct))
    return;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_STRING,
                                  &listener->bus_name);
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_OBJECT_PATH,
                                  &listener->path);
  d_uint = listener->types;
  dbus_message_iter_append_basic (&iter_struct, DBUS_TYPE_UINT32, &d_uint);
  dbus_message_iter_close_container (iter, &iter_struct);
}

static void
notify_mouse_listener (SpiDEController *controller,
                       DEControllerListener *listener,
                       gboolean enable)
{
  const char *path = SPI_DBUS_PATH_DEC;
  const char *interface = SPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER;
  const char *name = (enable
                      ? "DeviceListenerRegistered"
                      : "DeviceListenerDeregistered");
  DBusMessage *signal;
  DBusMessageIter iter;

  signal = dbus_message_new_signal (path, interface, name);
  if (!signal)
    return;
  dbus_message_iter_init_append (signal, &iter);
  append_mouse_listener (&iter, listener);
  dbus_connection_send (controller->bus, signal, NULL);
  dbus_message_unref (signal);
}

static gboolean
spi_controller_register_device_listener (SpiDEController      *controller,
					 DEControllerListener *listener)
{
  DEControllerKeyListener *key_listener;
  gboolean retval;
  
  switch (listener->type) {
  case SPI_DEVICE_TYPE_KBD:
      key_listener = (DEControllerKeyListener *) listener;

      controller->key_listeners = g_list_prepend (controller->key_listeners,
						  key_listener);
      spi_dbus_add_disconnect_match (controller->bus, key_listener->listener.bus_name);
      if (key_listener->mode->global)
        {
	  retval = spi_controller_register_global_keygrabs (controller, key_listener);
	}
      else
	  retval = TRUE;
      if (retval)
	notify_keystroke_listener (controller, key_listener, TRUE);
      break;
  case SPI_DEVICE_TYPE_MOUSE:
      controller->mouse_listeners = g_list_prepend (controller->mouse_listeners, listener);
      if (!have_mouse_listener)
        {
          have_mouse_listener = TRUE;
          if (!have_mouse_event_listener) {
            guint id;
            id = g_timeout_add (100, spi_dec_poll_mouse_idle, controller);
            g_source_set_name_by_id (id, "[at-spi2-core] spi_dec_poll_mouse_idle");
          }
        }
      spi_dbus_add_disconnect_match (controller->bus, listener->bus_name);
      notify_mouse_listener (controller, listener, TRUE);
      break;
  default:
      break;
  }
  return FALSE;
}

static void
set_reply (DBusPendingCall *pending, void *user_data)
{
    void **replyptr = (void **)user_data;

    *replyptr = dbus_pending_call_steal_reply (pending);
}

static GSList *hung_processes = NULL;

static void
reset_hung_process (DBusPendingCall *pending, void *data)
{
  DBusMessage *message = data;
  const char *dest = dbus_message_get_destination (message);
  GSList *l;

  /* At this point we don't care about the result */
  dbus_pending_call_unref (pending);

  for (l = hung_processes; l; l = l->next)
  {
    if (!strcmp (l->data, dest))
    {
      g_free (l->data);
      hung_processes = g_slist_remove (hung_processes, l->data);
      break;
    }
  }
}

static gint
time_elapsed (struct timeval *origin)
{
  struct timeval tv;

  gettimeofday (&tv, NULL);
  return (tv.tv_sec - origin->tv_sec) * 1000 + (tv.tv_usec - origin->tv_usec) / 1000;
}

static void
reset_hung_process_from_ping (DBusPendingCall *pending, void *data)
{
  GSList *l;

  for (l = hung_processes; l; l = l->next)
  {
    if (!strcmp (l->data, data))
    {
      g_free (l->data);
      hung_processes = g_slist_remove (hung_processes, l->data);
      break;
    }
  }
  g_free (data);
  dbus_pending_call_unref (pending);
}

static DBusMessage *
send_and_allow_reentry (DBusConnection *bus, DBusMessage *message, int timeout, DBusError *error)
{
    DBusPendingCall *pending;
    DBusMessage *reply = NULL;
  struct timeval tv;

    if (!dbus_connection_send_with_reply (bus, message, &pending, -1))
    {
        return NULL;
    }
    dbus_pending_call_set_notify (pending, set_reply, (void *)&reply, NULL);
    gettimeofday (&tv, NULL);
    while (!reply)
    {
      if (!dbus_connection_read_write_dispatch (bus, timeout) ||
          time_elapsed (&tv) > timeout)
      {
        const char *dest = dbus_message_get_destination (message);
        GSList *l;
        gchar *bus_name_dup;
        dbus_message_ref (message);
        dbus_pending_call_set_notify (pending, reset_hung_process, message,
                                      (DBusFreeFunction) dbus_message_unref);
        message = dbus_message_new_method_call (dest, "/",
                                                "org.freedesktop.DBus.Peer",
                                                "Ping");
        if (!message)
          return NULL;
        dbus_connection_send_with_reply (bus, message, &pending, -1);
        dbus_message_unref (message);
        if (!pending)
          return NULL;
        bus_name_dup = g_strdup (dest);
        dbus_pending_call_set_notify (pending, reset_hung_process_from_ping,
                                      bus_name_dup, NULL);
        for (l = hung_processes; l; l = l->next)
          if (!strcmp (l->data, dest))
            return NULL;
        hung_processes = g_slist_prepend (hung_processes, g_strdup (dest));
        return NULL;
      }
    }
    dbus_pending_call_unref (pending);
    return reply;
}
static gboolean
Accessibility_DeviceEventListener_NotifyEvent(SpiDEController *controller,
                                              SpiRegistry *registry,
                                              DEControllerListener *listener,
                                              const Accessibility_DeviceEvent *key_event)
{
  DBusMessage *message = dbus_message_new_method_call(listener->bus_name,
                                                      listener->path,
                                                      SPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER,
                                                      "NotifyEvent");
  dbus_bool_t consumed = FALSE;
  GSList *l;
  gboolean hung = FALSE;

  for (l = hung_processes; l; l = l->next)
  {
    if (!strcmp (l->data, listener->bus_name))
    {
      dbus_message_set_no_reply (message, TRUE);
      hung = TRUE;
      break;
    }
  }

  if (spi_dbus_marshal_deviceEvent(message, key_event))
  {
    DBusMessage *reply;

    if (hung)
    {
      dbus_connection_send (controller->bus, message, NULL);
      dbus_message_unref (message);
      return FALSE;
    }

    reply = send_and_allow_reentry (controller->bus, message, 3000, NULL);
    if (reply)
    {
      dbus_message_get_args(reply, NULL, DBUS_TYPE_BOOLEAN, &consumed, DBUS_TYPE_INVALID);
      dbus_message_unref(reply);
    }
  }
  dbus_message_unref(message);
  return consumed;
}

gboolean
spi_controller_notify_mouselisteners (SpiDEController                 *controller,
				      const Accessibility_DeviceEvent *event)
{
  GList   *l;
  GSList  *notify = NULL, *l2;
  GList  **listeners = &controller->mouse_listeners;
  gboolean is_consumed;
#ifdef SPI_KEYEVENT_DEBUG
  gboolean found = FALSE;
#endif
  if (!listeners)
    {
      return FALSE;
    }

  for (l = *listeners; l; l = l->next)
    {
       DEControllerListener *listener = l->data;

       if (eventtype_seq_contains_event (listener->types, event))
         {
	   /* we clone (don't dup) the listener, to avoid refcount inc. */
	   notify = g_slist_prepend (notify,
				     spi_listener_clone (listener));
#ifdef SPI_KEYEVENT_DEBUG
           found = TRUE;
#endif
         }
    }

#ifdef SPI_KEYEVENT_DEBUG
  if (!found)
    {
      g_print ("no match for event\n");
    }
#endif

  is_consumed = FALSE;
  for (l2 = notify; l2 && !is_consumed; l2 = l2->next)
    {
      DEControllerListener *listener = l2->data;

      is_consumed = Accessibility_DeviceEventListener_NotifyEvent (controller, controller->registry, listener, event);

      spi_listener_clone_free ((DEControllerListener *) l2->data);
    }

  for (; l2; l2 = l2->next)
    {
      DEControllerListener *listener = l2->data;
      spi_listener_clone_free (listener);
      /* clone doesn't have its own ref, so don't use spi_device_listener_free */
    }

  g_slist_free (notify);

#ifdef SPI_DEBUG
  if (is_consumed) g_message ("consumed\n");
#endif
  return is_consumed;
}

static gboolean
key_set_contains_key (GSList                          *key_set,
			  const Accessibility_DeviceEvent *key_event)
{
  gint i;
  gint len;
  GSList *l;

  if (!key_set)
    {
#ifdef SPI_DEBUG
      g_print ("null key set!\n");
#endif
      return TRUE;
    }

  len = g_slist_length (key_set);
  
  if (len == 0) /* special case, means "all keys/any key" */
    {
#ifdef SPI_DEBUG
      g_print ("anykey\n");	    
#endif
      return TRUE;
    }

  for (l = key_set,i = 0; l; l = g_slist_next(l),i++)
    {
      Accessibility_KeyDefinition *kd = l->data;
#ifdef SPI_KEYEVENT_DEBUG	    
      g_print ("key_set[%d] event = %d, code = %d; key_event %d, code %d, string %s\n",
                i,
                (int) kd->keysym,
                (int) kd->keycode,
                (int) key_event->id,
                (int) key_event->hw_code,
                key_event->event_string); 
#endif
      if (kd->keysym == (dbus_uint32_t) key_event->id)
        {
          return TRUE;
	}
      if (kd->keycode == (dbus_uint32_t) key_event->hw_code)
        {
          return TRUE;
	}
      if (key_event->event_string && key_event->event_string[0] &&
	  !strcmp (kd->keystring, key_event->event_string))
        {
          return TRUE;
	}
    }

  return FALSE;
}

static gboolean
eventtype_seq_contains_event (dbus_uint32_t types,
				  const Accessibility_DeviceEvent *event)
{
  if (types == 0) /* special case, means "all events/any event" */
    {
      return TRUE;
    }

  return (types & (1 << event->type));
}

static gboolean
spi_key_event_matches_listener (const Accessibility_DeviceEvent *key_event,
				DEControllerKeyListener         *listener,
				dbus_bool_t                    is_system_global)
{
  if (((key_event->modifiers & 0xFF) == (dbus_uint16_t) (listener->mask & 0xFF)) &&
       key_set_contains_key (listener->keys, key_event) &&
       eventtype_seq_contains_event (listener->listener.types, key_event) && 
      (is_system_global == listener->mode->global))
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

gboolean
spi_controller_notify_keylisteners (SpiDEController                 *controller,
				    Accessibility_DeviceEvent       *key_event,
				    dbus_bool_t                    is_system_global)
{
  GList   *l;
  GSList  *notify = NULL, *l2;
  GList  **key_listeners = &controller->key_listeners;
  gboolean is_consumed;

  if (!key_listeners)
    {
      return FALSE;
    }

  /* set the NUMLOCK event mask bit if appropriate: see bug #143702 */
  if (key_event->modifiers & _numlock_physical_mask)
      key_event->modifiers |= SPI_KEYMASK_NUMLOCK;

  for (l = *key_listeners; l; l = l->next)
    {
       DEControllerKeyListener *key_listener = l->data;

       if (spi_key_event_matches_listener (key_event, key_listener, is_system_global))
         {
	   /* we clone (don't dup) the listener, to avoid refcount inc. */
	   notify = g_slist_prepend (notify,
				     spi_key_listener_clone (key_listener));
         }
    }

#ifdef SPI_KEYEVENT_DEBUG
  if (!notify)
    {
      g_print ("no match for event\n");
    }
#endif

  is_consumed = FALSE;
  for (l2 = notify; l2 && !is_consumed; l2 = l2->next)
    {
      DEControllerKeyListener *key_listener = l2->data;	    

      is_consumed = Accessibility_DeviceEventListener_NotifyEvent (controller, controller->registry, &key_listener->listener, key_event) &&
	            key_listener->mode->preemptive;

      spi_key_listener_clone_free (key_listener);
    }

  for (; l2; l2 = l2->next)
    {
      DEControllerKeyListener *key_listener = l2->data;	    
      spi_key_listener_clone_free (key_listener);
      /* clone doesn't have its own ref, so don't use spi_dec_listener_free */
    }

  g_slist_free (notify);

#ifdef SPI_DEBUG
  if (is_consumed) g_message ("consumed\n");
#endif
  return is_consumed;
}

gboolean
spi_clear_error_state (void)
{
	gboolean retval = spi_error_code != 0;
	spi_error_code = 0;
	return retval;
}

gboolean
spi_controller_update_key_grabs (SpiDEController           *controller,
				 Accessibility_DeviceEvent *recv)
{
  GList *l, *next;
  gboolean   update_failed = FALSE;
  long keycode = 0;
  
  g_return_val_if_fail (controller != NULL, FALSE);

  /*
   * masks known to work with default RH 7.1+:
   * 0 (no mods), LockMask, Mod1Mask, Mod2Mask, ShiftMask,
   * ShiftMask|LockMask, Mod1Mask|LockMask, Mod2Mask|LockMask,
   * ShiftMask|Mod1Mask, ShiftMask|Mod2Mask, Mod1Mask|Mod2Mask,
   * ShiftMask|LockMask|Mod1Mask, ShiftMask|LockMask|Mod2Mask,
   *
   * ControlMask grabs are broken, must be in use already
   */
  if (recv)
    keycode = spi_dec_plat_get_keycode (controller, recv->id, NULL, TRUE, NULL);
  for (l = controller->keygrabs_list; l; l = next)
    {
      gboolean do_remove;
      gboolean re_issue_grab;
      DEControllerGrabMask *grab_mask = l->data;

      next = l->next;

      re_issue_grab = recv &&
	      (recv->modifiers & grab_mask->mod_mask) &&
	      (grab_mask->key_val == keycode);

#ifdef SPI_DEBUG
      fprintf (stderr, "mask=%lx %lx (%c%c) %s\n",
	       (long int) grab_mask->key_val,
	       (long int) grab_mask->mod_mask,
	       grab_mask->pending_add ? '+' : '.',
	       grab_mask->pending_remove ? '-' : '.',
	       re_issue_grab ? "re-issue": "");
#endif

      do_remove = FALSE;

      if (grab_mask->pending_add && grab_mask->pending_remove)
        {
          do_remove = TRUE;
	}
      else if (grab_mask->pending_remove)
        {
#ifdef SPI_DEBUG
      fprintf (stderr, "ungrabbing, mask=%x\n", grab_mask->mod_mask);
#endif
	  spi_dec_plat_ungrab_key (controller,
		               grab_mask->key_val,
		               grab_mask->mod_mask);

          do_remove = TRUE;
	}
      else if (grab_mask->pending_add || re_issue_grab)
        {

#ifdef SPI_DEBUG
	  fprintf (stderr, "grab %d with mask %x\n", grab_mask->key_val, grab_mask->mod_mask);
#endif
	  update_failed = spi_dec_plat_grab_key (controller,
		                               grab_mask->key_val,
		                               grab_mask->mod_mask);
	  if (update_failed) {
		  while (grab_mask->ref_count > 0) --grab_mask->ref_count;
		  do_remove = TRUE;
	  }
	}

      grab_mask->pending_add = FALSE;
      grab_mask->pending_remove = FALSE;

      if (do_remove)
        {
          g_assert (grab_mask->ref_count <= 0);

	  controller->keygrabs_list = g_list_delete_link (
	    controller->keygrabs_list, l);

	  spi_grab_mask_free (grab_mask);
	}

    } 

  return ! update_failed;
}

/*
 * Implemented GObject::finalize
 */
static void
spi_device_event_controller_object_finalize (GObject *object)
{
  SpiDEController *controller;
  GObjectClass *parent_class = G_OBJECT_CLASS(spi_device_event_controller_parent_class);
  SpiDEControllerClass *klass;

  controller = SPI_DEVICE_EVENT_CONTROLLER (object);
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (controller);
#ifdef SPI_DEBUG
  fprintf(stderr, "spi_device_event_controller_object_finalize called\n");
#endif
  if (klass->plat.finalize)
    klass->plat.finalize (controller);

  parent_class->finalize (object);
}

/*
 * DBus Accessibility::DEController::RegisterKeystrokeListener
 *     method implementation
 */
static DBusMessage *
impl_register_keystroke_listener (DBusConnection *bus,
				  DBusMessage *message,
				  void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  DEControllerKeyListener *dec_listener;
  DBusMessageIter iter, iter_array;
  const char *path;
  GSList *keys = NULL;
  dbus_int32_t mask, type;
  Accessibility_EventListenerMode *mode;
 dbus_bool_t ret;
  DBusMessage *reply = NULL;
  char *keystring;

  if (strcmp (dbus_message_get_signature (message), "oa(iisi)uu(bbb)") != 0)
    return invalid_arguments_error (message);

  dbus_message_iter_init(message, &iter);
  dbus_message_iter_get_basic(&iter, &path);
  dbus_message_iter_next(&iter);
  dbus_message_iter_recurse(&iter, &iter_array);
  while (dbus_message_iter_get_arg_type(&iter_array) != DBUS_TYPE_INVALID)
  {
    Accessibility_KeyDefinition *kd = (Accessibility_KeyDefinition *)g_malloc(sizeof(Accessibility_KeyDefinition));
    if (!spi_dbus_message_iter_get_struct(&iter_array, DBUS_TYPE_INT32, &kd->keycode, DBUS_TYPE_INT32, &kd->keysym, DBUS_TYPE_STRING, &keystring, DBUS_TYPE_INVALID))
    {
      break;
    }
    kd->keystring = g_strdup (keystring);
    keys = g_slist_append(keys, kd);
  }
  dbus_message_iter_next(&iter);
  dbus_message_iter_get_basic(&iter, &mask);
  dbus_message_iter_next(&iter);
  dbus_message_iter_get_basic(&iter, &type);
  dbus_message_iter_next(&iter);
  mode = (Accessibility_EventListenerMode *)g_malloc(sizeof(Accessibility_EventListenerMode));
  if (mode)
  {
    spi_dbus_message_iter_get_struct(&iter, DBUS_TYPE_BOOLEAN, &mode->synchronous, DBUS_TYPE_BOOLEAN, &mode->preemptive, DBUS_TYPE_BOOLEAN, &mode->global, DBUS_TYPE_INVALID);
  }
#ifdef SPI_DEBUG
  fprintf (stderr, "registering keystroke listener %s:%s with maskVal %lu\n",
	   dbus_message_get_sender(message), path, (unsigned long) mask);
#endif
  dec_listener = spi_dec_key_listener_new (dbus_message_get_sender(message), path, keys, mask, type, mode);
  g_free (mode);
  ret = spi_controller_register_device_listener (
	  controller, (DEControllerListener *) dec_listener);
  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);
  }
  return reply;
}

/*
 * DBus Accessibility::DEController::RegisterDeviceEventListener
 *     method implementation
 */
static DBusMessage *
impl_register_device_event_listener (DBusConnection *bus,
				  DBusMessage *message,
				  void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  DEControllerListener *dec_listener;
  const char *path;
  dbus_int32_t event_types;
  dbus_bool_t ret;
  DBusMessage *reply = NULL;

  if (!dbus_message_get_args(message, NULL, DBUS_TYPE_OBJECT_PATH, &path, DBUS_TYPE_UINT32, &event_types, DBUS_TYPE_INVALID))
  {
    return invalid_arguments_error (message);
  }
  dec_listener = spi_dec_listener_new (dbus_message_get_sender(message), path, event_types);
  ret =  spi_controller_register_device_listener (
	  controller, (DEControllerListener *) dec_listener);
  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);
  }
  return reply;
}

typedef struct {
	DBusConnection *bus;
	DEControllerListener    *listener;
} RemoveListenerClosure;

static SpiReEntrantContinue
remove_listener_cb (GList * const *list,
		    gpointer       user_data)
{
  DEControllerListener  *listener = (*list)->data;
  RemoveListenerClosure *ctx = user_data;

  if (!strcmp(ctx->listener->bus_name, listener->bus_name) &&
      !strcmp(ctx->listener->path, listener->path))
    {
      spi_re_entrant_list_delete_link (list);
      spi_dbus_remove_disconnect_match (ctx->bus, listener->bus_name);
      spi_dec_listener_free (listener);
    }

  return SPI_RE_ENTRANT_CONTINUE;
}

static SpiReEntrantContinue
copy_key_listener_cb (GList * const *list,
		      gpointer       user_data)
{
  DEControllerKeyListener  *key_listener = (*list)->data;
  RemoveListenerClosure    *ctx = user_data;

  if (!strcmp(ctx->listener->bus_name, key_listener->listener.bus_name) &&
      !strcmp(ctx->listener->path, key_listener->listener.path))
    {
      /* TODO: FIXME aggregate keys in case the listener is registered twice */
      DEControllerKeyListener *ctx_key_listener = 
	(DEControllerKeyListener *) ctx->listener; 
      keylist_free (ctx_key_listener->keys);	    
      ctx_key_listener->keys = keylist_clone(key_listener->keys);
    }

  return SPI_RE_ENTRANT_CONTINUE;
}

static void
spi_controller_deregister_device_listener (SpiDEController            *controller,
					   DEControllerListener *listener)
{
  RemoveListenerClosure  ctx;

  ctx.bus = controller->bus;
  ctx.listener = listener;

  notify_mouse_listener (controller, listener, FALSE);

  spi_re_entrant_list_foreach (&controller->mouse_listeners,
			       remove_listener_cb, &ctx);
  if (!controller->mouse_listeners)
    have_mouse_listener = FALSE;
}

static void
spi_deregister_controller_key_listener (SpiDEController            *controller,
					DEControllerKeyListener    *key_listener)
{
  RemoveListenerClosure  ctx;

  ctx.bus = controller->bus;
  ctx.listener = (DEControllerListener *) spi_key_listener_clone (key_listener);

  notify_keystroke_listener (controller, key_listener, FALSE);

  /* special case, copy keyset from existing controller list entry */
  if (g_slist_length(key_listener->keys) == 0)
    {
      spi_re_entrant_list_foreach (&controller->key_listeners,
				  copy_key_listener_cb, &ctx);
    }

  spi_controller_deregister_global_keygrabs (controller, key_listener);

  spi_re_entrant_list_foreach (&controller->key_listeners,
				remove_listener_cb, &ctx);

  spi_key_listener_clone_free ((DEControllerKeyListener *) ctx.listener);
}

void
spi_remove_device_listeners (SpiDEController *controller, const char *bus_name)
{
  GList *l, *tmp;

  for (l = controller->mouse_listeners; l; l = tmp)
  {
    DEControllerListener *listener = l->data;
    tmp = l->next;
    if (!strcmp (listener->bus_name, bus_name))
    {
      spi_controller_deregister_device_listener (controller, listener);
      tmp = controller->mouse_listeners;
    }
  }
  for (l = controller->key_listeners; l; l = tmp)
  {
    DEControllerKeyListener *key_listener = l->data;
    tmp = l->next;
    if (!strcmp (key_listener->listener.bus_name, bus_name))
    {
      /* TODO: untangle the below line(s) */
      spi_deregister_controller_key_listener (controller, key_listener);
      tmp = controller->key_listeners;
    }
  }
}

/*
 * DBus Accessibility::DEController::DeregisterKeystrokeListener
 *     method implementation
 */
static DBusMessage *
impl_deregister_keystroke_listener (DBusConnection *bus,
				  DBusMessage *message,
				  void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  DEControllerKeyListener *key_listener;
  DBusMessageIter iter, iter_array;
  const char *path;
  GSList *keys = NULL;
  dbus_int32_t mask, type;
  DBusMessage *reply = NULL;

  dbus_message_iter_init(message, &iter);
  if (strcmp (dbus_message_get_signature (message), "oa(iisi)uu") != 0)
  {
    g_warning ("Received DeregisterKeystrokeListener with strange signature '%s'", dbus_message_get_signature (message));
    return invalid_arguments_error (message);
  }

  dbus_message_iter_get_basic(&iter, &path);
  dbus_message_iter_next(&iter);
  dbus_message_iter_recurse(&iter, &iter_array);
  while (dbus_message_iter_get_arg_type(&iter_array) != DBUS_TYPE_INVALID)
  {
    Accessibility_KeyDefinition *kd = (Accessibility_KeyDefinition *)g_malloc(sizeof(Accessibility_KeyDefinition));
  char *keystring;

    if (!spi_dbus_message_iter_get_struct(&iter_array, DBUS_TYPE_INT32, &kd->keycode, DBUS_TYPE_INT32, &kd->keysym, DBUS_TYPE_STRING, &keystring, DBUS_TYPE_INVALID))
    {
      break;
    }
    kd->keystring = g_strdup (keystring);
    keys = g_slist_append(keys, kd);
  }
  dbus_message_iter_next(&iter);
  dbus_message_iter_get_basic(&iter, &mask);
  dbus_message_iter_next(&iter);
  dbus_message_iter_get_basic(&iter, &type);
  dbus_message_iter_next(&iter);
  key_listener = spi_dec_key_listener_new (dbus_message_get_sender(message), path, keys, mask, type, NULL);
#ifdef SPI_DEREGISTER_DEBUG
  fprintf (stderr, "deregistering keystroke listener %p with maskVal %lu\n",
	   (void *) l, (unsigned long) mask->value);
#endif

  spi_deregister_controller_key_listener (controller, key_listener);

  spi_dec_listener_free ((DEControllerListener *) key_listener);
  reply = dbus_message_new_method_return (message);
  return reply;
}

/*
 * DBus Accessibility::DEController::DeregisterDeviceEventListener
 *     method implementation
 */
static DBusMessage *
impl_deregister_device_event_listener (DBusConnection *bus,
				  DBusMessage *message,
				  void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  DEControllerListener *listener;
  const char *path;
  dbus_int32_t event_types;
  DBusMessage *reply = NULL;

  if (!dbus_message_get_args(message, NULL, DBUS_TYPE_OBJECT_PATH, &path, DBUS_TYPE_UINT32, &event_types, DBUS_TYPE_INVALID))
  {
    return invalid_arguments_error (message);
  }
  listener = spi_dec_listener_new (dbus_message_get_sender(message), path, event_types);
  spi_controller_deregister_device_listener (
	  controller, listener);
  reply = dbus_message_new_method_return (message);
  return reply;
}

static DBusMessage *
impl_get_keystroke_listeners (DBusConnection *bus,
				  DBusMessage *message,
				  void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  DBusMessageIter iter, iter_array;
  DBusMessage *reply = dbus_message_new_method_return (message);
  GList *l;

  if (!reply)
    return NULL;

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY,
                                    "(souua(iisi)u(bbb))", &iter_array);
  for (l = controller->key_listeners; l; l = l->next)
  {
    append_keystroke_listener (&iter_array, l->data);
  }
  dbus_message_iter_close_container (&iter, &iter_array);
  return reply;
}

static DBusMessage *
impl_get_device_event_listeners (DBusConnection *bus,
				  DBusMessage *message,
				  void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  DBusMessageIter iter, iter_array;
  GList *l;
  DBusMessage *reply = dbus_message_new_method_return (message);

  if (!reply)
    return NULL;

  dbus_message_iter_init_append (reply, &iter);
  dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY,
                                    "(sou)", &iter_array);
  for (l = controller->key_listeners; l; l = l->next)
  {
    append_mouse_listener (&iter_array, l->data);
  }
  dbus_message_iter_close_container (&iter, &iter_array);
  return reply;
}

static unsigned
get_modifier_state (SpiDEController *controller)
{
	return mouse_mask_state;
}

gboolean
spi_dec_synth_keysym (SpiDEController *controller, long keysym)
{
	long key_synth_code;
	unsigned int modifiers, synth_mods, lock_mods;

	key_synth_code = spi_dec_plat_get_keycode (controller, keysym, NULL, TRUE, &synth_mods);

	if ((key_synth_code == 0) || (synth_mods == 0xFF)) return FALSE;

	/* TODO: set the modifiers accordingly! */
	modifiers = get_modifier_state (controller);
	/* side-effect; we may unset mousebutton modifiers here! */

	lock_mods = 0;
	if (synth_mods != modifiers) {
		lock_mods = synth_mods & ~modifiers;
		spi_dec_plat_lock_modifiers (controller, lock_mods);
	}
	spi_dec_plat_synth_keycode_press (controller, key_synth_code);
	spi_dec_plat_synth_keycode_release (controller, key_synth_code);

	if (synth_mods != modifiers) 
		spi_dec_plat_unlock_modifiers (controller, lock_mods);
	return TRUE;
}



/*
 * DBus Accessibility::DEController::RegisterKeystrokeListener
 *     method implementation
 */
static DBusMessage *
impl_generate_keyboard_event (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  dbus_int32_t keycode;
  char *keystring;
  dbus_uint32_t synth_type;
  DBusMessage *reply = NULL;

  if (!dbus_message_get_args(message, NULL, DBUS_TYPE_INT32, &keycode, DBUS_TYPE_STRING, &keystring, DBUS_TYPE_UINT32, &synth_type, DBUS_TYPE_INVALID))
  {
    return invalid_arguments_error (message);
  }

#ifdef SPI_DEBUG
	fprintf (stderr, "synthesizing keystroke %ld, type %d\n",
		 (long) keycode, (int) synth_type);
#endif
  /* TODO: hide/wrap/remove X dependency */

  /*
   * TODO: when initializing, query for XTest extension before using,
   * and fall back to XSendEvent() if XTest is not available.
   */
  
  switch (synth_type)
    {
      case Accessibility_KEY_PRESS:
	      spi_dec_plat_synth_keycode_press (controller, keycode);
	      break;
      case Accessibility_KEY_PRESSRELEASE:
	      spi_dec_plat_synth_keycode_press (controller, keycode);
      case Accessibility_KEY_RELEASE:
	      spi_dec_plat_synth_keycode_release (controller, keycode);
	      break;
      case Accessibility_KEY_SYM:
#ifdef SPI_XKB_DEBUG	      
	      fprintf (stderr, "KeySym synthesis\n");
#endif
	      /* 
	       * note: we are using long for 'keycode'
	       * in our arg list; it can contain either
	       * a keycode or a keysym.
	       */
	      spi_dec_synth_keysym (controller, (KeySym) keycode);
	      break;
      case Accessibility_KEY_STRING:
	      if (!spi_dec_plat_synth_keystring (controller, synth_type, keycode, keystring))
		      fprintf (stderr, "Keystring synthesis failure, string=%s\n",
			       keystring);
	      break;
    }
  reply = dbus_message_new_method_return (message);
  return reply;
}

/* Accessibility::DEController::GenerateMouseEvent */
static DBusMessage *
impl_generate_mouse_event (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  dbus_int32_t       x;
  dbus_int32_t       y;
  char *eventName;
  DBusMessage *reply = NULL;

  if (!dbus_message_get_args(message, NULL, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y, DBUS_TYPE_STRING, &eventName, DBUS_TYPE_INVALID))
  {
    return invalid_arguments_error (message);
  }

#ifdef SPI_DEBUG
  fprintf (stderr, "generating mouse %s event at %ld, %ld\n",
	   eventName, (long int) x, (long int) y);
#endif
  spi_dec_plat_generate_mouse_event (saved_controller, x, y, eventName);
  reply = dbus_message_new_method_return (message);
  return reply;
}

/* Accessibility::DEController::NotifyListenersSync */
static DBusMessage *
impl_notify_listeners_sync (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  Accessibility_DeviceEvent event;
  dbus_bool_t ret;
  DBusMessage *reply = NULL;

  if (!spi_dbus_demarshal_deviceEvent(message, &event))
  {
    return invalid_arguments_error (message);
  }
#ifdef SPI_DEBUG
  g_print ("notifylistening listeners synchronously: controller %p, event id %d\n",
	   controller, (int) event.id);
#endif
  ret = spi_controller_notify_keylisteners (controller,
					     (Accessibility_DeviceEvent *) 
					     &event, FALSE) ?
	  TRUE : FALSE; 
  reply = dbus_message_new_method_return (message);
  if (reply)
  {
    dbus_message_append_args (reply, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID);
  }
  return reply;
}

static DBusMessage *
impl_notify_listeners_async (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER(user_data);
  Accessibility_DeviceEvent event;
  DBusMessage *reply = NULL;

  if (!spi_dbus_demarshal_deviceEvent(message, &event))
  {
    return invalid_arguments_error (message);
  }
#ifdef SPI_DEBUG
  g_print ("notifylistening listeners asynchronously: controller %p, event id %d\n",
	   controller, (int) event.id);
#endif
  spi_controller_notify_keylisteners (controller, (Accessibility_DeviceEvent *)
				      &event, FALSE); 
  reply = dbus_message_new_method_return (message);
  return reply;
}

static void
spi_device_event_controller_class_init (SpiDEControllerClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;

  spi_device_event_controller_parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_device_event_controller_object_finalize;

#ifdef HAVE_X11
  if (g_getenv ("DISPLAY"))
    spi_dec_setup_x11 (klass);
  else
#endif
  g_type_class_add_private (object_class, sizeof (long)); /* dummy */
}

static void
spi_device_event_controller_init (SpiDEController *device_event_controller)
{
  SpiDEControllerClass *klass;
  klass = SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS (device_event_controller);

  /* TODO: shouldn't be gpointer below */
  device_event_controller->priv = G_TYPE_INSTANCE_GET_PRIVATE (device_event_controller,
                                              SPI_DEVICE_EVENT_CONTROLLER_TYPE,
                                              gpointer);
  device_event_controller->message_queue = g_queue_new ();
  saved_controller = device_event_controller;

  if (klass->plat.init)
    klass->plat.init (device_event_controller);
}


/*---------------------------------------------------------------------------*/

static const char *introspection_header =
"<?xml version=\"1.0\"?>\n";

static const char *introspection_node_element =
"<node name=\"%s\">\n";

static const char *introspection_footer =
"</node>";

static DBusMessage *
impl_Introspect (DBusConnection * bus,
                 DBusMessage * message, void *user_data)
{
  GString *output;
  gchar *final;

  const gchar *pathstr = SPI_DBUS_PATH_DEC;

  DBusMessage *reply;

  output = g_string_new(introspection_header);

  g_string_append_printf(output, introspection_node_element, pathstr);

  g_string_append (output, spi_org_a11y_atspi_DeviceEventController);

  g_string_append(output, introspection_footer);
  final = g_string_free(output, FALSE);

  reply = dbus_message_new_method_return (message);
  dbus_message_append_args(reply, DBUS_TYPE_STRING, &final, DBUS_TYPE_INVALID);

  g_free(final);
  return reply;
}

/*---------------------------------------------------------------------------*/

static void
handle_dec_method_from_idle (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  const gchar *iface   = dbus_message_get_interface (message);
  const gchar *member  = dbus_message_get_member (message);
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  DBusMessage *reply = NULL;

  if (!strcmp (iface, SPI_DBUS_INTERFACE_DEC))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "RegisterKeystrokeListener"))
          reply = impl_register_keystroke_listener (bus, message, user_data);
      else if (!strcmp (member, "RegisterDeviceEventListener"))
          reply = impl_register_device_event_listener (bus, message, user_data);
      else if (!strcmp (member, "DeregisterKeystrokeListener"))
          reply = impl_deregister_keystroke_listener (bus, message, user_data);
      else if (!strcmp (member, "DeregisterDeviceEventListener"))
          reply = impl_deregister_device_event_listener (bus, message, user_data);
      else if (!strcmp (member, "GetKeystrokeListeners"))
          reply = impl_get_keystroke_listeners (bus, message, user_data);
      else if (!strcmp (member, "GetDeviceEventListeners"))
          reply = impl_get_device_event_listeners (bus, message, user_data);
      else if (!strcmp (member, "GenerateKeyboardEvent"))
          reply = impl_generate_keyboard_event (bus, message, user_data);
      else if (!strcmp (member, "GenerateMouseEvent"))
          reply = impl_generate_mouse_event (bus, message, user_data);
      else if (!strcmp (member, "NotifyListenersSync"))
          reply = impl_notify_listeners_sync (bus, message, user_data);
      else if (!strcmp (member, "NotifyListenersAsync"))
          reply = impl_notify_listeners_async (bus, message, user_data);
      else
          result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (!strcmp (iface, "org.freedesktop.DBus.Introspectable"))
    {
      result = DBUS_HANDLER_RESULT_HANDLED;
      if      (!strcmp (member, "Introspect"))
          reply = impl_Introspect (bus, message, user_data);
      else
          result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

  if (result == DBUS_HANDLER_RESULT_HANDLED)
    {
      if (!reply)
        {
          reply = dbus_message_new_method_return (message);
        }

      dbus_connection_send (bus, reply, NULL);
      dbus_message_unref (reply);
    }
}

static gboolean
message_queue_dispatch (gpointer data)
{
  saved_controller->message_queue_idle = 0;
  while (!g_queue_is_empty (saved_controller->message_queue))
    {
      DBusMessage *message = g_queue_pop_head (saved_controller->message_queue);
      data = g_queue_pop_head (saved_controller->message_queue);
      handle_dec_method_from_idle (saved_controller->bus, message, data);
      dbus_message_unref (message);
    }
  return FALSE;
}

static DBusHandlerResult
handle_dec_method (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  const gchar *iface   = dbus_message_get_interface (message);
  const gchar *member  = dbus_message_get_member (message);
  const gint   type    = dbus_message_get_type (message);

  /* Check for basic reasons not to handle */
  if (type   != DBUS_MESSAGE_TYPE_METHOD_CALL ||
      member == NULL ||
      iface  == NULL)
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  dbus_message_ref (message);
  g_queue_push_tail (saved_controller->message_queue, message);
  g_queue_push_tail (saved_controller->message_queue, user_data);
  if (!saved_controller->message_queue_idle) {
    saved_controller->message_queue_idle = g_idle_add (message_queue_dispatch, NULL);
    g_source_set_name_by_id (saved_controller->message_queue_idle, "[at-spi2-core] message_queue_dispatch");
  }
  return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusObjectPathVTable dec_vtable =
{
  NULL,
  &handle_dec_method,
  NULL, NULL, NULL, NULL
};

SpiDEController *
spi_registry_dec_new (SpiRegistry *reg, DBusConnection *bus)
{
  SpiDEController *dec = g_object_new (SPI_DEVICE_EVENT_CONTROLLER_TYPE, NULL);

  dec->registry = g_object_ref (reg);
  reg->dec = g_object_ref (dec);
  dec->bus = bus;

  dbus_connection_register_object_path (bus, SPI_DBUS_PATH_DEC, &dec_vtable, dec);

  return dec;
}

void
spi_device_event_controller_start_poll_mouse (SpiRegistry *registry)
{
  if (!have_mouse_event_listener)
    {
      have_mouse_event_listener = TRUE;
      if (!have_mouse_listener) {
        guint id;
        id = g_timeout_add (100, spi_dec_poll_mouse_idle, registry->dec);
        g_source_set_name_by_id (id, "[at-spi2-core] spi_dec_poll_mouse_idle");
      }
    }
}

void
spi_device_event_controller_stop_poll_mouse (void)
{
  have_mouse_event_listener = FALSE;
}
