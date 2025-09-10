/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2020 SUSE LLC.
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

#include "atspi-device-x11.h"
#include "atspi-private.h"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>

#define ATSPI_VIRTUAL_MODIFIER_MASK 0x0000f000

typedef struct _AtspiDeviceX11Private AtspiDeviceX11Private;
struct _AtspiDeviceX11Private
{
  Display *display;
  Window root_window;
  Window focused_window;
  GSource *source;
  int xi_opcode;
  GSList *modifiers;
  GSList *key_grabs;
  guint virtual_mods_enabled;
  gboolean keyboard_grabbed;
  unsigned int numlock_physical_mask;
  AtspiEventListener *event_listener;
};

GObjectClass *device_x11_parent_class;

typedef struct _DisplaySource
{
  GSource source;

  Display *display;
  GPollFD event_poll_fd;
} DisplaySource;

typedef struct
{
  guint keycode;
  guint modifier;
} AtspiX11KeyModifier;

typedef struct
{
  AtspiKeyDefinition *kd;
  gboolean enabled;
  Window window;
} AtspiX11KeyGrab;

static gboolean
event_prepare (GSource *source, gint *timeout)
{
  Display *display = ((DisplaySource *) source)->display;
  gboolean retval;

  *timeout = -1;
  retval = XPending (display);

  return retval;
}

static gboolean
event_check (GSource *source)
{
  DisplaySource *display_source = (DisplaySource *) source;
  gboolean retval;

  if (display_source->event_poll_fd.revents & G_IO_IN)
    retval = XPending (display_source->display);
  else
    retval = FALSE;

  return retval;
}

static void
xi2keyevent (XIDeviceEvent *xievent, XEvent *xkeyevent)
{
                    XkbStateRec st= {};

  memset (xkeyevent, 0, sizeof (*xkeyevent));
                    XkbGetState (xievent->display, XkbUseCoreKbd, &st);

  switch (xievent->evtype)
    {
    case XI_KeyPress:
      xkeyevent->type = KeyPress;
      break;
    case XI_KeyRelease:
      xkeyevent->type = KeyRelease;
      break;
    default:
      break;
    }
  xkeyevent->xkey.serial = xievent->serial;
  xkeyevent->xkey.send_event = xievent->send_event;
  xkeyevent->xkey.display = xievent->display;
  xkeyevent->xkey.window = xievent->event;
  xkeyevent->xkey.root = xievent->root;
  xkeyevent->xkey.subwindow = xievent->child;
  xkeyevent->xkey.time = xievent->time;
  xkeyevent->xkey.x = xievent->event_x;
  xkeyevent->xkey.y = xievent->event_y;
  xkeyevent->xkey.x_root = xievent->root_x;
  xkeyevent->xkey.y_root = xievent->root_y;
  xkeyevent->xkey.state = xievent->mods.effective | (st.group << 13);
  xkeyevent->xkey.keycode = xievent->detail;
  xkeyevent->xkey.same_screen = 1;
}

G_DEFINE_TYPE_WITH_CODE (AtspiDeviceX11, atspi_device_x11, ATSPI_TYPE_DEVICE, G_ADD_PRIVATE (AtspiDeviceX11))

static guint
find_virtual_mapping (AtspiDeviceX11 *x11_device, gint keycode)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  GSList *l;

  for (l = priv->modifiers; l; l = l->next)
    {
      AtspiX11KeyModifier *entry = l->data;
      if (entry->keycode == keycode)
        return entry->modifier;
    }

  return 0;
}

static gboolean
grab_should_be_enabled (AtspiDeviceX11 *x11_device, AtspiX11KeyGrab *grab)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);

  /* If the whole keyboard is grabbed, then all keys are grabbed elsewhere */
  if (priv->keyboard_grabbed)
    return FALSE;

  guint virtual_mods_used = grab->kd->modifiers & ATSPI_VIRTUAL_MODIFIER_MASK;
  return ((priv->virtual_mods_enabled & virtual_mods_used) == virtual_mods_used);
}

static gboolean
grab_has_active_duplicate (AtspiDeviceX11 *x11_device, AtspiX11KeyGrab *grab)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  GSList *l;

  for (l = priv->key_grabs; l; l = l->next)
    {
      AtspiX11KeyGrab *other = l->data;
      if (other != grab && other->enabled && other->kd->keycode == grab->kd->keycode && other->kd->keysym == grab->kd->keysym && (other->kd->modifiers & ~ATSPI_VIRTUAL_MODIFIER_MASK) == (grab->kd->modifiers & ~ATSPI_VIRTUAL_MODIFIER_MASK))
        return TRUE;
    }
  return FALSE;
}

static gboolean
grab_key_aux (AtspiDeviceX11 *x11_device, Window window, int keycode, int modmask)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  XIGrabModifiers xi_modifiers;
  XIEventMask eventmask;
  unsigned char mask[XIMaskLen (XI_LASTEVENT)] = { 0 };
  int ret;

  xi_modifiers.modifiers = modmask;
  xi_modifiers.status = 0;

  eventmask.deviceid = XIAllDevices;
  eventmask.mask_len = sizeof (mask);
  eventmask.mask = mask;

  XISetMask (mask, XI_KeyPress);
  XISetMask (mask, XI_KeyRelease);

  ret = XIGrabKeycode (priv->display, XIAllMasterDevices, keycode, window, XIGrabModeSync, XIGrabModeAsync, False, &eventmask, 1, &xi_modifiers);
  return (ret == 0);
}

static gboolean
grab_key (AtspiDeviceX11 *x11_device, Window window, int keycode, int modmask)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  gboolean include_numlock = !_atspi_key_is_on_keypad (keycode);
  gboolean ret = FALSE;

  ret |= grab_key_aux (x11_device, window, keycode, modmask);
  if (!(modmask & LockMask))
    ret |= grab_key_aux (x11_device, window, keycode, modmask | LockMask);
  if (include_numlock && !(modmask & priv->numlock_physical_mask))
    {
      ret |= grab_key_aux (x11_device, window, keycode, modmask | priv->numlock_physical_mask);
      if (!(modmask & LockMask))
        ret |= grab_key_aux (x11_device, window, keycode, modmask | LockMask | priv->numlock_physical_mask);
    }
  return ret;
}

static gboolean
enable_key_grab (AtspiDeviceX11 *x11_device, AtspiX11KeyGrab *grab)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  gboolean ret;
  gint grab_keycode;

  g_return_val_if_fail (priv->display != NULL, FALSE);

  if (grab_has_active_duplicate (x11_device, grab))
    ret = TRUE;
  else
    {
      if (grab->kd->keysym != 0)
        grab_keycode = XKeysymToKeycode (priv->display, grab->kd->keysym);
      else
        grab_keycode = grab->kd->keycode;

      ret = grab_key (x11_device, priv->focused_window, grab_keycode, grab->kd->modifiers & ~ATSPI_VIRTUAL_MODIFIER_MASK);
    }
  grab->enabled = TRUE;
  grab->window = priv->focused_window;
  return ret;
}

static void
ungrab_key_aux (AtspiDeviceX11 *x11_device, Window window, int keycode, int modmask)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  XIGrabModifiers xi_modifiers;

  xi_modifiers.modifiers = modmask;
  xi_modifiers.status = 0;

  XIUngrabKeycode (priv->display, XIAllMasterDevices, keycode, window, 1, &xi_modifiers);
}

static void
ungrab_key (AtspiDeviceX11 *x11_device, Window window, int keycode, int modmask)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);

  ungrab_key_aux (x11_device, window, keycode, modmask);
  if (!(modmask & LockMask))
    ungrab_key_aux (x11_device, window, keycode, modmask | LockMask);
  if (!(modmask & priv->numlock_physical_mask))
    {
      ungrab_key_aux (x11_device, window, keycode, modmask | priv->numlock_physical_mask);
      if (!(modmask & LockMask))
        ungrab_key_aux (x11_device, window, keycode, modmask | LockMask | priv->numlock_physical_mask);
    }
}

static void
disable_key_grab (AtspiDeviceX11 *x11_device, AtspiX11KeyGrab *grab)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  gint grab_keycode;

  g_return_if_fail (priv->display != NULL);

  if (!grab->enabled)
    return;

  grab->enabled = FALSE;

  if (grab_has_active_duplicate (x11_device, grab))
    return;

  if (grab->kd->keysym != 0)
    grab_keycode = XKeysymToKeycode (priv->display, grab->kd->keysym);
  else
    grab_keycode = grab->kd->keycode;
  ungrab_key (x11_device, grab->window, grab_keycode, grab->kd->modifiers & ~ATSPI_VIRTUAL_MODIFIER_MASK);
}

static void
refresh_key_grabs (AtspiDeviceX11 *x11_device)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  GSList *l;
  int focus_revert;

  XGetInputFocus (priv->display, &priv->focused_window, &focus_revert);

  for (l = priv->key_grabs; l; l = l->next)
    {
      AtspiX11KeyGrab *grab = l->data;
      if (grab->window != priv->focused_window)
        disable_key_grab (x11_device, grab);
    }

  for (l = priv->key_grabs; l; l = l->next)
    {
      AtspiX11KeyGrab *grab = l->data;
      gboolean new_enabled = grab_should_be_enabled (x11_device, grab);
      if (new_enabled && !grab->enabled)
        enable_key_grab (x11_device, grab);
      else if (grab->enabled && !new_enabled)
        disable_key_grab (x11_device, grab);
    }
}

static void
set_virtual_modifier (AtspiDeviceX11 *x11_device, gint keycode, gboolean enabled)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  guint modifier = find_virtual_mapping (x11_device, keycode);

  if (!modifier)
    return;

  if (enabled)
    {
      if (priv->virtual_mods_enabled & modifier)
        return;
      priv->virtual_mods_enabled |= modifier;
    }
  else
    {
      if (!(priv->virtual_mods_enabled & modifier))
        return;
      priv->virtual_mods_enabled &= ~modifier;
    }

  refresh_key_grabs (x11_device);
}

static gboolean
should_suppress_keygrab_window_events ()
{
  /* The goal here is to only enable this hack for i3 and ratpoison, so,
     as an imperfect proxy, check that XDG_CURRENT_DESKTOP is unset. */
  return (getenv ("XDG_CURRENT_DESKTOP") == NULL);
}

static gboolean
do_event_dispatch (gpointer user_data)
{
  AtspiDeviceX11 *device = user_data;
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (device);
  Display *display = priv->display;
  XEvent xevent;
  char text[10];
  KeySym keysym;
  XComposeStatus status;
  guint modifiers;

  g_object_ref (device);
  while (XPending (display))
    {
      XNextEvent (display, &xevent);
      XEvent keyevent;

      switch (xevent.type)
        {
        case KeyPress:
        case KeyRelease:
          XLookupString (&xevent.xkey, text, sizeof (text), &keysym, &status);
          modifiers = xevent.xkey.state | priv->virtual_mods_enabled;
          if (modifiers & priv->numlock_physical_mask)
            {
              modifiers |= (1 << ATSPI_MODIFIER_NUMLOCK);
              modifiers &= ~priv->numlock_physical_mask;
            }
          atspi_device_notify_key (ATSPI_DEVICE (device), (xevent.type == KeyPress), xevent.xkey.keycode, keysym, modifiers, text);
          break;
        case FocusIn:
          refresh_key_grabs (device);
          break;
        case GenericEvent:
          if (xevent.xcookie.extension == priv->xi_opcode)
            {
              XGetEventData (priv->display, &xevent.xcookie);
              XIRawEvent *xiRawEv = (XIRawEvent *) xevent.xcookie.data;
              XIDeviceEvent *xiDevEv = (XIDeviceEvent *) xevent.xcookie.data;
              switch (xevent.xcookie.evtype)
                {
                case XI_KeyPress:
                case XI_KeyRelease:
                  xi2keyevent (xiDevEv, &keyevent);
                  XLookupString ((XKeyEvent *) &keyevent, text, sizeof (text), &keysym, &status);
                  if (text[0] < ' ')
                    text[0] = '\0';
                  set_virtual_modifier (device, xiRawEv->detail, xevent.xcookie.evtype == XI_KeyPress);
                  modifiers = keyevent.xkey.state | priv->virtual_mods_enabled;
                  if (modifiers & priv->numlock_physical_mask)
                    modifiers |= (1 << ATSPI_MODIFIER_NUMLOCK);
                  if (xiDevEv->deviceid == xiDevEv->sourceid)
                    atspi_device_notify_key (ATSPI_DEVICE (device), (xevent.xcookie.evtype == XI_KeyPress), xiRawEv->detail, keysym, modifiers, text);
                  else if (should_suppress_keygrab_window_events ())
                    _atspi_update_window_filter_time ();
                  XFreeEventData (priv->display, &xevent.xcookie);
                  break;
                case FocusIn:
                  refresh_key_grabs (device);
                  break;
                }
            }
        default:
          if (XFilterEvent (&xevent, None))
            continue;
        }
    }

  g_object_unref (device);
  return TRUE;
}

static gboolean
event_dispatch (GSource *source, GSourceFunc callback, gpointer user_data)
{
  if (callback)
    callback (user_data);
  return G_SOURCE_CONTINUE;
}

static GSourceFuncs event_funcs = {
  event_prepare,
  event_check,
  event_dispatch,
  NULL
};

static GSource *
display_source_new (Display *display)
{
  GSource *source = g_source_new (&event_funcs, sizeof (DisplaySource));
  DisplaySource *display_source = (DisplaySource *) source;
  g_source_set_name (source, "[at-spi2-core] display_source_funcs");

  display_source->display = display;

  return source;
}

static void
create_event_source (AtspiDeviceX11 *device)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (device);
  DisplaySource *display_source;

  int connection_number = ConnectionNumber (priv->display);

  priv->source = display_source_new (priv->display);
  display_source = (DisplaySource *) priv->source;

  g_source_set_priority (priv->source, G_PRIORITY_DEFAULT);

  display_source->event_poll_fd.fd = connection_number;
  display_source->event_poll_fd.events = G_IO_IN;

  g_source_add_poll (priv->source, &display_source->event_poll_fd);
  g_source_set_can_recurse (priv->source, TRUE);
  g_source_set_callback (priv->source, do_event_dispatch, device, NULL);
  g_source_attach (priv->source, NULL);
}

static gboolean
check_virtual_modifier (AtspiDeviceX11 *x11_device, guint modifier)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  GSList *l;

  if (modifier == (1 << ATSPI_MODIFIER_NUMLOCK))
    return TRUE;

  for (l = priv->modifiers; l; l = l->next)
    {
      AtspiX11KeyModifier *entry = l->data;
      if (entry->modifier == modifier)
        return TRUE;
    }

  return FALSE;
}

static guint
get_unused_virtual_modifier (AtspiDeviceX11 *x11_device)
{
  guint ret = 0x1000;

  while (ret < 0x10000)
    {
      if (!check_virtual_modifier (x11_device, ret))
        return ret;
      ret <<= 1;
    }

  return 0;
}

static guint
atspi_device_x11_map_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  XkbDescPtr desc;
  guint ret;
  AtspiX11KeyModifier *entry;

  desc = XkbGetMap (priv->display, XkbModifierMapMask, XkbUseCoreKbd);

  if (keycode < desc->min_key_code || keycode >= desc->max_key_code)
    {
      XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
      g_warning ("Passed invalid keycode %d", keycode);
      return 0;
    }

  ret = desc->map->modmap[keycode];
  XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
  if (ret & (ShiftMask | ControlMask))
    return ret;

  ret = find_virtual_mapping (x11_device, keycode);
  if (ret)
    return ret;

  ret = get_unused_virtual_modifier (x11_device);

  entry = g_new (AtspiX11KeyModifier, 1);
  entry->keycode = keycode;
  entry->modifier = ret;
  priv->modifiers = g_slist_append (priv->modifiers, entry);

  return ret;
}

static void
atspi_device_x11_unmap_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  GSList *l;

  for (l = priv->modifiers; l; l = l->next)
    {
      AtspiX11KeyModifier *entry = l->data;
      if (entry->keycode == keycode)
        {
          priv->modifiers = g_slist_remove (priv->modifiers, entry);
          g_free (entry);
          return;
        }
    }
}

static guint
atspi_device_x11_get_modifier (AtspiDevice *device, gint keycode)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  XkbDescPtr desc;
  guint ret;

  desc = XkbGetMap (priv->display, XkbModifierMapMask, XkbUseCoreKbd);

  if (keycode < desc->min_key_code || keycode >= desc->max_key_code)
    {
      XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
      g_warning ("Passed invalid keycode %d", keycode);
      return 0;
    }

  ret = desc->map->modmap[keycode];
  XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
  if (ret)
    return ret;

  return find_virtual_mapping (x11_device, keycode);
}

static void
event_listener_cb (AtspiEvent *event, void *user_data)
{
  AtspiDeviceX11 *x11_device = user_data;

  refresh_key_grabs (x11_device);
  g_boxed_free (ATSPI_TYPE_EVENT, event);
}

static void
atspi_device_x11_init (AtspiDeviceX11 *device)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (device);
  int first_event, first_error;
  int focus_revert;

  priv->display = XOpenDisplay ("");
  g_return_if_fail (priv->display != NULL);
  priv->root_window = DefaultRootWindow (priv->display);
  XGetInputFocus (priv->display, &priv->focused_window, &focus_revert);

  if (XQueryExtension (priv->display, "XInputExtension", &priv->xi_opcode, &first_event, &first_error))
    {
      int major = 2;
      int minor = 1;
      if (XIQueryVersion (priv->display, &major, &minor) != BadRequest)
        {
          XIEventMask eventmask;
          unsigned char mask[XIMaskLen (XI_LASTEVENT)] = { 0 };

          eventmask.deviceid = XIAllDevices;
          eventmask.mask_len = sizeof (mask);
          eventmask.mask = mask;

          XISetMask (mask, XI_KeyPress);
          XISetMask (mask, XI_KeyRelease);
          XISetMask (mask, XI_ButtonPress);
          XISetMask (mask, XI_ButtonRelease);
          XISetMask (mask, XI_Motion);
          XISelectEvents (priv->display, priv->root_window, &eventmask, 1);
          XSelectInput (priv->display, priv->root_window, FocusChangeMask);
          create_event_source (device);
        }
    }

  priv->numlock_physical_mask = XkbKeysymToModifiers (priv->display,
                                                      XK_Num_Lock);

  priv->event_listener = atspi_event_listener_new (event_listener_cb, device, NULL);
  atspi_event_listener_register (priv->event_listener, "window:activate", NULL);
  atspi_event_listener_register (priv->event_listener, "window:deactivate", NULL);
}

static void
atspi_device_x11_finalize (GObject *object)
{
  AtspiDeviceX11 *device = ATSPI_DEVICE_X11 (object);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (device);

  while (priv->key_grabs)
    {
      AtspiX11KeyGrab *grab = priv->key_grabs->data;
      disable_key_grab (device, grab);
      g_boxed_free (ATSPI_TYPE_KEY_DEFINITION, grab->kd);
      g_free (grab);
      priv->key_grabs = g_slist_remove (priv->key_grabs, grab);
    }

  g_slist_free_full (priv->modifiers, g_free);
  priv->modifiers = NULL;

  if (priv->source)
    {
      g_source_destroy ((GSource *) priv->source);
      g_source_unref ((GSource *) priv->source);
      priv->source = NULL;
    }

  /* TODO: deregister event listeners automatically when the listener is finalized */
  atspi_event_listener_deregister (priv->event_listener, "window:deactivate", NULL);
  atspi_event_listener_deregister (priv->event_listener, "window:activate", NULL);
  g_object_unref (priv->event_listener);

  device_x11_parent_class->finalize (object);
}

static gboolean
atspi_device_x11_add_key_grab (AtspiDevice *device, AtspiKeyDefinition *kd)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  AtspiX11KeyGrab *grab;
  gboolean ret;

  grab = g_new0 (AtspiX11KeyGrab, 1);
  grab->kd = g_boxed_copy (ATSPI_TYPE_KEY_DEFINITION, kd);
  grab->enabled = FALSE;
  if (grab_should_be_enabled (x11_device, grab))
    ret = enable_key_grab (x11_device, grab);
  else
    ret = TRUE;

  if (ret)
    priv->key_grabs = g_slist_append (priv->key_grabs, grab);
  else
    {
      g_boxed_free (ATSPI_TYPE_KEY_DEFINITION, grab->kd);
      g_free (grab);
    }

  return ret;
}

static void
atspi_device_x11_remove_key_grab (AtspiDevice *device, guint id)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  AtspiKeyDefinition *kd;
  GSList *l;

  kd = atspi_device_get_grab_by_id (device, id);

  for (l = priv->key_grabs; l; l = g_slist_next (l))
    {
      AtspiX11KeyGrab *other = l->data;
      if (((kd->keycode && other->kd->keycode == kd->keycode) || (kd->keysym && other->kd->keysym == kd->keysym)) && other->kd->modifiers == kd->modifiers)
        {
          disable_key_grab (x11_device, other);
          priv->key_grabs = g_slist_remove (priv->key_grabs, other);
          break;
        }
    }

  g_free (kd);
}

static guint
atspi_device_x11_get_locked_modifiers (AtspiDevice *device)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  XkbStateRec state_rec;

  memset (&state_rec, 0, sizeof (state_rec));
  XkbGetState (priv->display, XkbUseCoreKbd, &state_rec);
  return state_rec.locked_mods;
}

static void
get_keycode_range (AtspiDeviceX11 *x11_device, int *min, int *max)
{
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  XkbDescPtr desc;

  desc = XkbGetMap (priv->display, XkbModifierMapMask, XkbUseCoreKbd);
  *min = desc->min_key_code;
  *max = desc->max_key_code;
  XkbFreeKeyboard (desc, XkbModifierMapMask, TRUE);
}

static gboolean
atspi_device_x11_grab_keyboard (AtspiDevice *device)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  int min, max;
  gint i;

  g_return_val_if_fail (priv->display != NULL, FALSE);
#if 0
  /* THis seems like the right thing to do, but it fails for me */
  return (XGrabKeyboard (priv->display, priv->focused_window, TRUE, GrabModeAsync, GrabModeSync, CurrentTime)) == 0;
#else
  if (priv->keyboard_grabbed)
    return TRUE;
  priv->keyboard_grabbed = TRUE;
  refresh_key_grabs (x11_device);

  get_keycode_range (x11_device, &min, &max);
  for (i = min; i < max; i++)
    grab_key (x11_device, priv->focused_window, i, 0);
  return TRUE;
#endif
}

static void
atspi_device_x11_ungrab_keyboard (AtspiDevice *device)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);
  int min, max;
  gint i;

  g_return_if_fail (priv->display != NULL);
#if 0
  XUngrabKeyboard (priv->display, CurrentTime);
#else
  if (!priv->keyboard_grabbed)
    return;
  priv->keyboard_grabbed = FALSE;

  get_keycode_range (x11_device, &min, &max);
  for (i = min; i < max; i++)
    ungrab_key (x11_device, priv->focused_window, i, 0);

  refresh_key_grabs (x11_device);
#endif
}

static void
atspi_device_x11_generate_mouse_event (AtspiDevice *device, AtspiAccessible *obj, gint x, gint y, const gchar *name, GError **error)
{
  AtspiPoint *p;

  p = atspi_component_get_position (ATSPI_COMPONENT (obj), ATSPI_COORD_TYPE_SCREEN, error);
  if (p->y == -1 && atspi_accessible_get_role (obj, NULL) == ATSPI_ROLE_APPLICATION)
    {
      g_clear_error (error);
      g_free (p);
      AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, 0, NULL);
      if (child)
        {
          p = atspi_component_get_position (ATSPI_COMPONENT (child), ATSPI_COORD_TYPE_SCREEN, error);
          g_object_unref (child);
        }
    }

  if (p->y == -1 || p->x == -1)
    return;

  x += p->x;
  y += p->y;
  g_free (p);

  /* TODO: do this in process */
  atspi_generate_mouse_event (x, y, name, error);
}

static guint
atspi_device_x11_map_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);

  gint keycode = XKeysymToKeycode (priv->display, keysym);
  return atspi_device_x11_map_modifier (device, keycode);
}

static void
atspi_device_x11_unmap_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);

  gint keycode = XKeysymToKeycode (priv->display, keysym);
  atspi_device_x11_unmap_modifier (device, keycode);
}

static guint
atspi_device_x11_get_keysym_modifier (AtspiDevice *device, guint keysym)
{
  AtspiDeviceX11 *x11_device = ATSPI_DEVICE_X11 (device);
  AtspiDeviceX11Private *priv = atspi_device_x11_get_instance_private (x11_device);

  gint keycode = XKeysymToKeycode (priv->display, keysym);
  return atspi_device_x11_get_modifier (device, keycode);
}

static void
atspi_device_x11_class_init (AtspiDeviceX11Class *klass)
{
  AtspiDeviceClass *device_class = ATSPI_DEVICE_CLASS (klass);
  GObjectClass *object_class = (GObjectClass *) klass;

  device_x11_parent_class = g_type_class_peek_parent (klass);
  device_class->add_key_grab = atspi_device_x11_add_key_grab;
  device_class->map_modifier = atspi_device_x11_map_modifier;
  device_class->unmap_modifier = atspi_device_x11_unmap_modifier;
  device_class->get_modifier = atspi_device_x11_get_modifier;
  device_class->remove_key_grab = atspi_device_x11_remove_key_grab;
  device_class->get_locked_modifiers = atspi_device_x11_get_locked_modifiers;
  device_class->grab_keyboard = atspi_device_x11_grab_keyboard;
  device_class->ungrab_keyboard = atspi_device_x11_ungrab_keyboard;
  device_class->generate_mouse_event = atspi_device_x11_generate_mouse_event;
  device_class->map_keysym_modifier = atspi_device_x11_map_keysym_modifier;
  device_class->unmap_keysym_modifier = atspi_device_x11_unmap_keysym_modifier;
  device_class->get_keysym_modifier = atspi_device_x11_get_keysym_modifier;
  object_class->finalize = atspi_device_x11_finalize;
}

/**
 * atspi_device_x11_new_full:
 * @app_id: (nullable): the application id.
 *
 * Creates a new #AtspiDeviceX11 with the given app id.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceX11.
 *
 * Since: 2.55
 */
AtspiDeviceX11 *
atspi_device_x11_new_full (const gchar *app_id)
{
  AtspiDeviceX11 *device = g_object_new (atspi_device_x11_get_type (), "app-id", app_id, NULL);

  return device;
}

/**
 * atspi_device_x11_new:
 *
 * Creates a new #AtspiDeviceX11.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceX11.
 *
 **/
AtspiDeviceX11 *
atspi_device_x11_new ()
{
  return atspi_device_x11_new_full (NULL);
}
