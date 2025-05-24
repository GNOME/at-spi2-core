/* AT-SPI - Assistive Technology Service Provider Interface
 *
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

/* deviceeventcontroller-x11.c: X-specific DeviceEventController support */

#include <config.h>

#undef SPI_XKB_DEBUG
#undef SPI_DEBUG
#undef SPI_KEYEVENT_DEBUG

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

#include <glib.h>

#include <dbus/dbus.h>

#include "de-types.h"
#include "display.h"
#include "event-source.h"
#include "keymasks.h"
#include "marshal-dbus.h"
#include "paths.h"

#include "deviceeventcontroller.h"

#include "deviceeventcontroller-x11.h"
#include "reentrant-list.h"

static void spi_dec_x11_emit_modifier_event (SpiDEController *controller,
                                             guint prev_mask,
                                             guint current_mask);

#define CHECK_RELEASE_DELAY 20
#define BIT(c, x) (c[x / 8] & (1 << (x % 8)))
static guint check_release_handler = 0;
static Accessibility_DeviceEvent pressed_event;
static void wait_for_release_event (XEvent *event, SpiDEController *controller);

static int spi_error_code = 0;
typedef struct
{
  gint x;
  gint y;
} SpiPoint;
static SpiPoint last_mouse_pos_static = { 0, 0 };
static SpiPoint *last_mouse_pos = &last_mouse_pos_static;
static unsigned int mouse_mask_state = 0;
static unsigned int mouse_button_mask =
    Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask;
static unsigned int key_modifier_mask =
    Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask | ShiftMask | LockMask | ControlMask | SPI_KEYMASK_NUMLOCK;
extern unsigned int _numlock_physical_mask;

static XModifierKeymap *xmkeymap = NULL;

static int (*x_default_error_handler) (Display *display, XErrorEvent *error_event);

static void spi_controller_register_with_devices (SpiDEController *controller);
static gboolean spi_device_event_controller_forward_key_event (SpiDEController *controller,
                                                               const XEvent *event);

static SpiDEController *saved_controller;

/* Normally this function would be provided by the macro call in deviceeventcontroller.c:
 *   G_DEFINE_TYPE_WITH_CODE (..., G_ADD_PRIVATE (SpiDEController))
 *
 * However, that machinery creates a static function for
 * _get_instance_private, so it is only visible in that file.  Here
 * we'll re-define it by hand, using the same name as that generated
 * function in case we can later merge the implementations together.
 */
static SpiDEControllerPrivate *
spi_device_event_controller_get_instance_private (SpiDEController *controller)
{
  return g_type_instance_get_private ((GTypeInstance *) controller, SPI_DEVICE_EVENT_CONTROLLER_TYPE);
}

static unsigned int
keysym_mod_mask (KeySym keysym, KeyCode keycode)
{
  /* we really should use XKB and look directly at the keymap */
  /* this is very inelegant */
  Display *display = spi_get_display ();
  unsigned int mods_rtn = 0;
  unsigned int retval = 0;
  KeySym sym_rtn;

  if (XkbLookupKeySym (display, keycode, 0, &mods_rtn, &sym_rtn) &&
      (sym_rtn == keysym))
    {
      retval = 0;
    }
  else if (XkbLookupKeySym (display, keycode, ShiftMask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = ShiftMask;
    }
  else if (XkbLookupKeySym (display, keycode, Mod2Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = Mod2Mask;
    }
  else if (XkbLookupKeySym (display, keycode, Mod3Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = Mod3Mask;
    }
  else if (XkbLookupKeySym (display, keycode, Mod4Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = Mod4Mask;
    }
  else if (XkbLookupKeySym (display, keycode, Mod5Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = Mod5Mask;
    }
  else if (XkbLookupKeySym (display, keycode,
                            ShiftMask | Mod2Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = (Mod2Mask | ShiftMask);
    }
  else if (XkbLookupKeySym (display, keycode,
                            ShiftMask | Mod3Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = (Mod3Mask | ShiftMask);
    }
  else if (XkbLookupKeySym (display, keycode,
                            ShiftMask | Mod4Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = (Mod4Mask | ShiftMask);
    }
  else if (XkbLookupKeySym (display, keycode,
                            ShiftMask | Mod5Mask, &mods_rtn, &sym_rtn) &&
           (sym_rtn == keysym))
    {
      retval = (Mod5Mask | ShiftMask);
    }
  else
    retval = 0xFFFF;
  return retval;
}

static gboolean
replace_map_keysym (SpiDEControllerPrivate *priv, KeyCode keycode, KeySym keysym)
{
#ifdef HAVE_XKB
  Display *dpy = spi_get_display ();
  XkbDescPtr desc;
  if (!(desc = XkbGetMap (dpy, XkbAllMapComponentsMask, XkbUseCoreKbd)))
    {
      fprintf (stderr, "ERROR getting map\n");
    }
  XFlush (dpy);
  XSync (dpy, False);
  if (desc && desc->map)
    {
      gint offset = desc->map->key_sym_map[keycode].offset;
      desc->map->syms[offset] = keysym;
    }
  else
    {
      fprintf (stderr, "Error changing key map: empty server structure\n");
    }
  XkbSetMap (dpy, XkbAllMapComponentsMask, desc);
  /**
   *  FIXME: the use of XkbChangeMap, and the reuse of the priv->xkb_desc structure,
   * would be far preferable.
   * HOWEVER it does not seem to work using XFree 4.3.
   **/
  /*	    XkbChangeMap (dpy, priv->xkb_desc, priv->changes); */
  XFlush (dpy);
  XSync (dpy, False);
  XkbFreeKeyboard (desc, 0, TRUE);

  return TRUE;
#else
  return FALSE;
#endif
}

static gboolean
spi_dec_reset_reserved (gpointer data)
{
  SpiDEControllerPrivate *priv = data;
  replace_map_keysym (priv, priv->reserved_keycode, priv->reserved_keysym);
  priv->reserved_reset_timeout = 0;
  return FALSE;
}

static gint
spi_dec_x11_get_keycode (SpiDEController *controller,
                         gint keysym,
                         gchar *key_str,
                         gboolean fix,
                         guint *modmask)
{
  KeyCode keycode = 0;
  if (key_str && key_str[0])
    keysym = XStringToKeysym (key_str);
  keycode = XKeysymToKeycode (spi_get_display (), (KeySym) keysym);
  if (!keycode && fix)
    {
      SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);
      /* if there's no keycode available, fix it */
      if (replace_map_keysym (priv, priv->reserved_keycode, keysym))
        {
          keycode = priv->reserved_keycode;
          /*
           * queue a timer to restore the old keycode.  Ugly, but required
           * due to races / asynchronous X delivery.
           * Long-term fix is to extend the X keymap here instead of replace entries.
           */
          priv->reserved_reset_timeout = g_timeout_add (500, spi_dec_reset_reserved, priv);
          g_source_set_name_by_id (priv->reserved_reset_timeout, "[at-spi2-core] spi_dec_reset_reserved");
        }
      if (modmask)
        *modmask = 0;
      return keycode;
    }
  if (modmask)
    *modmask = keysym_mod_mask (keysym, keycode);
  return keycode;
}

static gboolean
spi_dec_button_update_and_emit (SpiDEController *controller,
                                guint mask_return)
{
  gchar event_detail[3];

  if ((mask_return & mouse_button_mask) !=
      (mouse_mask_state & mouse_button_mask))
    {
      int button_number = 0;
      gboolean is_down = False;

      if (!(mask_return & Button1Mask) &&
          (mouse_mask_state & Button1Mask))
        {
          mouse_mask_state &= ~Button1Mask;
          button_number = 1;
        }
      else if ((mask_return & Button1Mask) &&
               !(mouse_mask_state & Button1Mask))
        {
          mouse_mask_state |= Button1Mask;
          button_number = 1;
          is_down = True;
        }
      else if (!(mask_return & Button2Mask) &&
               (mouse_mask_state & Button2Mask))
        {
          mouse_mask_state &= ~Button2Mask;
          button_number = 2;
        }
      else if ((mask_return & Button2Mask) &&
               !(mouse_mask_state & Button2Mask))
        {
          mouse_mask_state |= Button2Mask;
          button_number = 2;
          is_down = True;
        }
      else if (!(mask_return & Button3Mask) &&
               (mouse_mask_state & Button3Mask))
        {
          mouse_mask_state &= ~Button3Mask;
          button_number = 3;
        }
      else if ((mask_return & Button3Mask) &&
               !(mouse_mask_state & Button3Mask))
        {
          mouse_mask_state |= Button3Mask;
          button_number = 3;
          is_down = True;
        }
      else if (!(mask_return & Button4Mask) &&
               (mouse_mask_state & Button4Mask))
        {
          mouse_mask_state &= ~Button4Mask;
          button_number = 4;
        }
      else if ((mask_return & Button4Mask) &&
               !(mouse_mask_state & Button4Mask))
        {
          mouse_mask_state |= Button4Mask;
          button_number = 4;
          is_down = True;
        }
      else if (!(mask_return & Button5Mask) &&
               (mouse_mask_state & Button5Mask))
        {
          mouse_mask_state &= ~Button5Mask;
          button_number = 5;
        }
      else if ((mask_return & Button5Mask) &&
               !(mouse_mask_state & Button5Mask))
        {
          mouse_mask_state |= Button5Mask;
          button_number = 5;
          is_down = True;
        }
      if (button_number)
        {
#ifdef SPI_DEBUG
          fprintf (stderr, "Button %d %s\n",
                   button_number, (is_down) ? "Pressed" : "Released");
#endif
          snprintf (event_detail, 3, "%d%c", button_number,
                    (is_down) ? 'p' : 'r');
          dbus_uint32_t x = last_mouse_pos->x, y = last_mouse_pos->y;
          spi_dec_dbus_emit (controller, SPI_DBUS_INTERFACE_EVENT_MOUSE, "Button", event_detail, x, y);
        }
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

static guint
spi_dec_x11_mouse_check (SpiDEController *controller,
                         int *x,
                         int *y,
                         gboolean *moved)
{
  int win_x_return, win_y_return;
  unsigned int mask_return;
  Window root_return, child_return;
  Display *display = spi_get_display ();

  if (display == NULL)
    {
      return 0;
    }

  XQueryPointer (display, DefaultRootWindow (display),
                 &root_return, &child_return,
                 x, y,
                 &win_x_return, &win_y_return, &mask_return);

  if (*x != last_mouse_pos->x || *y != last_mouse_pos->y)
    {
      // TODO: combine these two signals?
      dbus_uint32_t ix = *x, iy = *y;
      spi_dec_dbus_emit (controller, SPI_DBUS_INTERFACE_EVENT_MOUSE, "Abs", "", ix, iy);
      ix -= last_mouse_pos->x;
      iy -= last_mouse_pos->y;
      spi_dec_dbus_emit (controller, SPI_DBUS_INTERFACE_EVENT_MOUSE, "Rel", "", ix, iy);
      last_mouse_pos->x = *x;
      last_mouse_pos->y = *y;
      *moved = True;
    }
  else
    {
      *moved = False;
    }

  /*
   * Since many clients grab the pointer, and X goes an automatic
   * pointer grab on mouse-down, we often must detect mouse button events
   * by polling rather than via a button grab.
   * The while loop (rather than if) is used since it's possible that
   * multiple buttons have changed state since we last checked.
   */
  if (mask_return != mouse_mask_state)
    {
      while (spi_dec_button_update_and_emit (controller, mask_return))
        ;
    }

  return mask_return;
}

#ifdef WE_NEED_UGRAB_MOUSE
static int
spi_dec_ungrab_mouse (gpointer data)
{
  Display *display = spi_get_display ();
  if (display)
    {
      XUngrabButton (spi_get_display (), AnyButton, AnyModifier,
                     XDefaultRootWindow (spi_get_display ()));
    }
  return FALSE;
}
#endif

static void
spi_dec_clear_unlatch_pending (SpiDEController *controller)
{
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);
  priv->xkb_latch_mask = 0;
}

static void
spi_device_event_controller_forward_mouse_event (SpiDEController *controller,
                                                 XEvent *xevent)
{
  gchar event_detail[3];
  XButtonEvent *xbutton_event = (XButtonEvent *) xevent;
  dbus_uint32_t ix, iy;

  int button = xbutton_event->button;

  unsigned int mouse_button_state = xbutton_event->state;

  switch (button)
    {
    case 1:
      mouse_button_state |= Button1Mask;
      break;
    case 2:
      mouse_button_state |= Button2Mask;
      break;
    case 3:
      mouse_button_state |= Button3Mask;
      break;
    case 4:
      mouse_button_state |= Button4Mask;
      break;
    case 5:
      mouse_button_state |= Button5Mask;
      break;
    }

  last_mouse_pos->x = ((XButtonEvent *) xevent)->x_root;
  last_mouse_pos->y = ((XButtonEvent *) xevent)->y_root;

#ifdef SPI_DEBUG
  fprintf (stderr, "mouse button %d %s (%x)\n",
           xbutton_event->button,
           (xevent->type == ButtonPress) ? "Press" : "Release",
           mouse_button_state);
#endif
  snprintf (event_detail, 3, "%d%c", button,
            (xevent->type == ButtonPress) ? 'p' : 'r');

  if ((mouse_button_state & mouse_button_mask) !=
      (mouse_mask_state & mouse_button_mask))
    {
      if ((mouse_mask_state & key_modifier_mask) !=
          (mouse_button_state & key_modifier_mask))
        spi_dec_x11_emit_modifier_event (controller,
                                         mouse_mask_state, mouse_button_state);
      mouse_mask_state = mouse_button_state;
      ix = last_mouse_pos->x;
      iy = last_mouse_pos->y;
      spi_dec_dbus_emit (controller, SPI_DBUS_INTERFACE_EVENT_MOUSE, "Button", event_detail, ix, iy);
    }

  XAllowEvents (spi_get_display (),
                ReplayPointer,
                CurrentTime);
}

static void
global_filter_fn (XEvent *xevent, void *data)
{
  SpiDEController *controller = SPI_DEVICE_EVENT_CONTROLLER (data);
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);
  Display *display = spi_get_display ();

  if (xevent->type == MappingNotify)
    xmkeymap = NULL;

  if (xevent->type == KeyPress || xevent->type == KeyRelease)
    {
      if (priv->xevie_display == NULL)
        {
          gboolean is_consumed;

          is_consumed =
              spi_device_event_controller_forward_key_event (controller, xevent);

          if (is_consumed)
            {
              int n_events;
              int i;
              XEvent next_event;
              n_events = XPending (display);

#ifdef SPI_KEYEVENT_DEBUG
              g_print ("Number of events pending: %d\n", n_events);
#endif
              for (i = 0; i < n_events; i++)
                {
                  XNextEvent (display, &next_event);
                  if (next_event.type != KeyPress &&
                      next_event.type != KeyRelease)
                    g_warning ("Unexpected event type %d in queue", next_event.type);
                }

              XAllowEvents (display, AsyncKeyboard, CurrentTime);
              if (n_events)
                XUngrabKeyboard (display, CurrentTime);
            }
          else
            {
              if (xevent->type == KeyPress)
                wait_for_release_event (xevent, controller);
              XAllowEvents (display, ReplayKeyboard, CurrentTime);
            }
        }

      return;
    }
  if (xevent->type == ButtonPress || xevent->type == ButtonRelease)
    {
      spi_device_event_controller_forward_mouse_event (controller, xevent);
    }
  if (xevent->type == priv->xkb_base_event_code)
    {
      XkbAnyEvent *xkb_ev = (XkbAnyEvent *) xevent;
      /* ugly but probably necessary...*/
      XSynchronize (display, TRUE);

      if (xkb_ev->xkb_type == XkbStateNotify)
        {
          XkbStateNotifyEvent *xkb_snev =
              (XkbStateNotifyEvent *) xkb_ev;
          /* check the mouse, to catch mouse events grabbed by
           * another client; in case we should revert this XKB delatch
           */
          if (!priv->pending_xkb_mod_relatch_mask)
            {
              int x, y;
              gboolean moved;
              spi_dec_x11_mouse_check (controller, &x, &y, &moved);
            }
          /* we check again, since the previous call may have
             changed this flag */
          if (priv->pending_xkb_mod_relatch_mask)
            {
              unsigned int feedback_mask;
#ifdef SPI_XKB_DEBUG
              fprintf (stderr, "relatching %x\n",
                       priv->pending_xkb_mod_relatch_mask);
#endif
              /* temporarily turn off the latch bell, if it's on */
              XkbGetControls (display,
                              XkbAccessXFeedbackMask,
                              priv->xkb_desc);
              feedback_mask = priv->xkb_desc->ctrls->ax_options;
              if (feedback_mask & XkbAX_StickyKeysFBMask)
                {
                  XkbControlsChangesRec changes = { XkbAccessXFeedbackMask,
                                                    0, False };
                  priv->xkb_desc->ctrls->ax_options &= ~(XkbAX_StickyKeysFBMask);
                  XkbChangeControls (display, priv->xkb_desc, &changes);
                }
              /* TODO: account for lock as well as latch */
              XkbLatchModifiers (display,
                                 XkbUseCoreKbd,
                                 priv->pending_xkb_mod_relatch_mask,
                                 priv->pending_xkb_mod_relatch_mask);
              if (feedback_mask & XkbAX_StickyKeysFBMask)
                {
                  XkbControlsChangesRec changes = { XkbAccessXFeedbackMask,
                                                    0, False };
                  priv->xkb_desc->ctrls->ax_options = feedback_mask;
                  XkbChangeControls (display, priv->xkb_desc, &changes);
                }
#ifdef SPI_XKB_DEBUG
              fprintf (stderr, "relatched %x\n",
                       priv->pending_xkb_mod_relatch_mask);
#endif
              priv->pending_xkb_mod_relatch_mask = 0;
            }
          else
            {
              priv->xkb_latch_mask = xkb_snev->latched_mods;
            }
        }
      XSynchronize (display, FALSE);
    }

  return;
}

static gboolean
spi_clear_error_state (void)
{
  gboolean retval = spi_error_code != 0;
  spi_error_code = 0;
  return retval;
}

static int
_spi_controller_device_error_handler (Display *display, XErrorEvent *error)
{
  if (error->error_code == BadAccess)
    {
      g_message ("Could not complete key grab: grab already in use.\n");
      spi_error_code = BadAccess;
      return 0;
    }
  else
    {
      return (*x_default_error_handler) (display, error);
    }
}

static void
spi_controller_register_with_devices (SpiDEController *controller)
{
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);
  int event_base, error_base, major_version, minor_version;

  if (XTestQueryExtension (spi_get_display (), &event_base, &error_base, &major_version, &minor_version))
    {
      XTestGrabControl (spi_get_display (), True);
    }

  /* calls to device-specific implementations and routines go here */
  /* register with: keyboard hardware code handler */
  /* register with: (translated) keystroke handler */

  priv->have_xkb = XkbQueryExtension (spi_get_display (),
                                      &priv->xkb_major_extension_opcode,
                                      &priv->xkb_base_event_code,
                                      &priv->xkb_base_error_code, NULL, NULL);
  if (priv->have_xkb)
    {
      gint i;
      guint64 reserved = 0;
      priv->xkb_desc = XkbGetMap (spi_get_display (), XkbKeySymsMask, XkbUseCoreKbd);
      XkbSelectEvents (spi_get_display (),
                       XkbUseCoreKbd,
                       XkbStateNotifyMask, XkbStateNotifyMask);
      _numlock_physical_mask = XkbKeysymToModifiers (spi_get_display (),
                                                     XK_Num_Lock);
      for (i = priv->xkb_desc->max_key_code; i >= priv->xkb_desc->min_key_code; --i)
        {
          if (priv->xkb_desc->map->key_sym_map[i].kt_index[0] == XkbOneLevelIndex)
            {
              if (XkbKeycodeToKeysym (spi_get_display (), i, 0, 0) != 0)
                {
                  /* don't use this one if there's a grab client! */

                  /* Runtime errors are generated from these functions,
                   * that are then quashed. Equivalent to:
                   * try
                   *   {Blah}
                   * except
                   *   {;}
                   */

                  spi_x_error_trap ();
                  XGrabKey (spi_get_display (), i, 0,
                            spi_get_root_window (),
                            TRUE,
                            GrabModeSync, GrabModeSync);
                  XSync (spi_get_display (), TRUE);
                  XUngrabKey (spi_get_display (), i, 0,
                              spi_get_root_window ());
                  if (!spi_x_error_release ())
                    {
                      reserved = i;
                      break;
                    }
                }
            }
        }
      if (reserved)
        {
          priv->reserved_keycode = reserved;
          priv->reserved_keysym = XkbKeycodeToKeysym (spi_get_display (), reserved, 0, 0);
        }
      else
        {
          priv->reserved_keycode = XKeysymToKeycode (spi_get_display (), XK_numbersign);
          priv->reserved_keysym = XK_numbersign;
        }
#ifdef SPI_RESERVED_DEBUG
      unsigned sym = 0;
      sym = XKeycodeToKeysym (spi_get_display (), reserved, 0);
      fprintf (stderr, "%x\n", sym);
      fprintf (stderr, "setting the reserved keycode to %d (%s)\n",
               reserved,
               XKeysymToString (XKeycodeToKeysym (spi_get_display (),
                                                  reserved, 0)));
#endif
    }

  spi_set_filter (global_filter_fn, controller);
  spi_set_events (KeyPressMask | KeyReleaseMask);

  x_default_error_handler = XSetErrorHandler (_spi_controller_device_error_handler);
}

static Accessibility_DeviceEvent
spi_keystroke_from_x_key_event (XKeyEvent *x_key_event)
{
  Accessibility_DeviceEvent key_event;
  KeySym keysym;
  const int cbuf_bytes = 20;
  char cbuf[21];
  int nbytes;

  nbytes = XLookupString (x_key_event, cbuf, cbuf_bytes, &keysym, NULL);
  key_event.id = (dbus_int32_t) (keysym);
  key_event.hw_code = (dbus_int16_t) x_key_event->keycode;
  if (((XEvent *) x_key_event)->type == KeyPress)
    {
      key_event.type = Accessibility_KEY_PRESSED_EVENT;
    }
  else
    {
      key_event.type = Accessibility_KEY_RELEASED_EVENT;
    }
  key_event.modifiers = (dbus_uint16_t) (x_key_event->state);
  key_event.is_text = FALSE;
  switch (keysym)
    {
    case ' ':
      key_event.event_string = g_strdup ("space");
      break;
    case XK_Tab:
      key_event.event_string = g_strdup ("Tab");
      break;
    case XK_BackSpace:
      key_event.event_string = g_strdup ("Backspace");
      break;
    case XK_Return:
      key_event.event_string = g_strdup ("Return");
      break;
    case XK_Home:
      key_event.event_string = g_strdup ("Home");
      break;
    case XK_Page_Down:
      key_event.event_string = g_strdup ("Page_Down");
      break;
    case XK_Page_Up:
      key_event.event_string = g_strdup ("Page_Up");
      break;
    case XK_F1:
      key_event.event_string = g_strdup ("F1");
      break;
    case XK_F2:
      key_event.event_string = g_strdup ("F2");
      break;
    case XK_F3:
      key_event.event_string = g_strdup ("F3");
      break;
    case XK_F4:
      key_event.event_string = g_strdup ("F4");
      break;
    case XK_F5:
      key_event.event_string = g_strdup ("F5");
      break;
    case XK_F6:
      key_event.event_string = g_strdup ("F6");
      break;
    case XK_F7:
      key_event.event_string = g_strdup ("F7");
      break;
    case XK_F8:
      key_event.event_string = g_strdup ("F8");
      break;
    case XK_F9:
      key_event.event_string = g_strdup ("F9");
      break;
    case XK_F10:
      key_event.event_string = g_strdup ("F10");
      break;
    case XK_F11:
      key_event.event_string = g_strdup ("F11");
      break;
    case XK_F12:
      key_event.event_string = g_strdup ("F12");
      break;
    case XK_End:
      key_event.event_string = g_strdup ("End");
      break;
    case XK_Escape:
      key_event.event_string = g_strdup ("Escape");
      break;
    case XK_Up:
      key_event.event_string = g_strdup ("Up");
      break;
    case XK_Down:
      key_event.event_string = g_strdup ("Down");
      break;
    case XK_Left:
      key_event.event_string = g_strdup ("Left");
      break;
    case XK_Right:
      key_event.event_string = g_strdup ("Right");
      break;
    default:
      if (nbytes > 0)
        {
          gunichar c;
          cbuf[nbytes] = '\0'; /* OK since length is cbuf_bytes+1 */
          key_event.event_string = g_strdup (cbuf);
          c = keysym2ucs (keysym);
          if (c > 0 && !g_unichar_iscntrl (c))
            {
              key_event.is_text = TRUE;
              /* incorrect for some composed chars? */
            }
        }
      else
        {
          key_event.event_string = g_strdup ("");
        }
    }

  key_event.timestamp = (dbus_uint32_t) x_key_event->time;
#ifdef SPI_KEYEVENT_DEBUG
  {
    char *pressed_str = "pressed";
    char *released_str = "released";
    char *state_ptr;

    if (key_event.type == Accessibility_KEY_PRESSED_EVENT)
      state_ptr = pressed_str;
    else
      state_ptr = released_str;

    fprintf (stderr,
             "Key %lu %s (%c), modifiers %d; string=%s [%x] %s\n",
             (unsigned long) keysym,
             state_ptr,
             keysym ? (int) keysym : '*',
             (int) x_key_event->state,
             key_event.event_string,
             key_event.event_string[0],
             (key_event.is_text == TRUE) ? "(text)" : "(not text)");
  }
#endif
#ifdef SPI_DEBUG
  fprintf (stderr, "%s%c\n",
           (x_key_event->state & Mod1Mask) ? "Alt-" : "",
           ((x_key_event->state & ShiftMask) ^ (x_key_event->state & LockMask)) ? g_ascii_toupper (keysym) : g_ascii_tolower (keysym));
  fprintf (stderr, "serial: %x Time: %x\n", x_key_event->serial, x_key_event->time);
#endif /* SPI_DEBUG */
  return key_event;
}

static gboolean
spi_dec_x11_grab_key (SpiDEController *controller,
                      guint key_val,
                      Accessibility_ControllerEventMask mod_mask)
{
  XGrabKey (spi_get_display (),
            key_val,
            mod_mask,
            spi_get_root_window (),
            True,
            GrabModeSync,
            GrabModeSync);
  XSync (spi_get_display (), False);
  return spi_clear_error_state ();
}

static void
spi_dec_x11_ungrab_key (SpiDEController *controller,
                        guint key_val,
                        Accessibility_ControllerEventMask mod_mask)
{
  XUngrabKey (spi_get_display (),
              key_val,
              mod_mask,
              spi_get_root_window ());
}

static unsigned int
xkb_get_slowkeys_delay (SpiDEController *controller)
{
  unsigned int retval = 0;
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);
#ifdef HAVE_XKB
#ifdef XKB_HAS_GET_SLOW_KEYS_DELAY
  retval = XkbGetSlowKeysDelay (spi_get_display (),
                                XkbUseCoreKbd, &bounce_delay);
#else
  if (!(priv->xkb_desc == (XkbDescPtr) BadAlloc || priv->xkb_desc == NULL))
    {
      Status s = XkbGetControls (spi_get_display (),
                                 XkbAllControlsMask, priv->xkb_desc);
      if (s == Success)
        {
          if (priv->xkb_desc->ctrls->enabled_ctrls & XkbSlowKeysMask)
            retval = priv->xkb_desc->ctrls->slow_keys_delay;
        }
    }
#endif
#endif
#ifdef SPI_XKB_DEBUG
  fprintf (stderr, "SlowKeys delay: %d\n", (int) retval);
#endif
  return retval;
}

static unsigned int
xkb_get_bouncekeys_delay (SpiDEController *controller)
{
  unsigned int retval = 0;
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);
#ifdef HAVE_XKB
#ifdef XKB_HAS_GET_BOUNCE_KEYS_DELAY
  retval = XkbGetBounceKeysDelay (spi_get_display (),
                                  XkbUseCoreKbd, &bounce_delay);
#else
  if (!(priv->xkb_desc == (XkbDescPtr) BadAlloc || priv->xkb_desc == NULL))
    {
      Status s = XkbGetControls (spi_get_display (),
                                 XkbAllControlsMask, priv->xkb_desc);
      if (s == Success)
        {
          if (priv->xkb_desc->ctrls->enabled_ctrls & XkbBounceKeysMask)
            retval = priv->xkb_desc->ctrls->debounce_delay;
        }
    }
#endif
#endif
#ifdef SPI_XKB_DEBUG
  fprintf (stderr, "BounceKeys delay: %d\n", (int) retval);
#endif
  return retval;
}

static gboolean
spi_dec_x11_synth_keycode_press (SpiDEController *controller,
                                 unsigned int keycode)
{
  unsigned int time = CurrentTime;
  unsigned int bounce_delay;
#ifdef THIS_IS_BROKEN
  unsigned int elapsed_msec;
  struct timeval tv;
#endif
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);

  spi_x_error_trap ();
  if (keycode == priv->last_release_keycode)
    {
      bounce_delay = xkb_get_bouncekeys_delay (controller);
      if (bounce_delay)
        {
#ifdef THIS_IS_BROKEN
          gettimeofday (&tv, NULL);
          elapsed_msec =
              (tv.tv_sec - priv->last_release_time.tv_sec) * 1000 + (tv.tv_usec - priv->last_release_time.tv_usec) / 1000;
#ifdef SPI_XKB_DEBUG
          fprintf (stderr, "%d ms elapsed (%ld usec)\n", elapsed_msec,
                   (long) (tv.tv_usec - priv->last_release_time.tv_usec));
#endif
          if (elapsed_msec < bounce_delay)
            time = bounce_delay - elapsed_msec + 1;
#else
          time = bounce_delay + 10;
          /* fudge for broken XTest */
#endif
#ifdef SPI_XKB_DEBUG
          fprintf (stderr, "waiting %d ms\n", time);
#endif
        }
    }
  XTestFakeKeyEvent (spi_get_display (), keycode, True, time);
  priv->last_press_keycode = keycode;
  XFlush (spi_get_display ());
  XSync (spi_get_display (), False);
  gettimeofday (&priv->last_press_time, NULL);
  return TRUE;
}

static gboolean
spi_dec_x11_synth_keycode_release (SpiDEController *controller,
                                   unsigned int keycode)
{
  unsigned int time = CurrentTime;
  unsigned int slow_delay;
#ifdef THIS_IS_BROKEN_DUNNO_WHY
  unsigned int elapsed_msec;
  struct timeval tv;
#endif
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);

  spi_x_error_trap ();
  if (keycode == priv->last_press_keycode)
    {
      slow_delay = xkb_get_slowkeys_delay (controller);
      if (slow_delay)
        {
#ifdef THIS_IS_BROKEN_DUNNO_WHY
          gettimeofday (&tv, NULL);
          elapsed_msec =
              (tv.tv_sec - priv->last_press_time.tv_sec) * 1000 + (tv.tv_usec - priv->last_press_time.tv_usec) / 1000;
#ifdef SPI_XKB_DEBUG
          fprintf (stderr, "%d ms elapsed (%ld usec)\n", elapsed_msec,
                   (long) (tv.tv_usec - priv->last_press_time.tv_usec));
#endif
          if (elapsed_msec < slow_delay)
            time = slow_delay - elapsed_msec + 1;
#else
          time = slow_delay + 10;
          /* our XTest seems broken, we have to add slop as above */
#endif
#ifdef SPI_XKB_DEBUG
          fprintf (stderr, "waiting %d ms\n", time);
#endif
        }
    }
  XTestFakeKeyEvent (spi_get_display (), keycode, False, time);
  priv->last_release_keycode = keycode;
  XSync (spi_get_display (), False);
  gettimeofday (&priv->last_release_time, NULL);
  return TRUE;
}

static gboolean
spi_dec_x11_lock_modifiers (SpiDEController *controller, unsigned modifiers)
{
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);

  if (priv->have_xkb)
    {
      return XkbLockModifiers (spi_get_display (), XkbUseCoreKbd,
                               modifiers, modifiers);
    }
  else
    {
      int mod_index;
      if (xmkeymap == NULL)
        xmkeymap = XGetModifierMapping (spi_get_display ());
      for (mod_index = 0; mod_index < 8; mod_index++)
        if (modifiers & (1 << mod_index))
          spi_dec_x11_synth_keycode_press (controller, xmkeymap->modifiermap[mod_index]);
      return TRUE;
    }
}

static gboolean
spi_dec_x11_unlock_modifiers (SpiDEController *controller, unsigned modifiers)
{
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);

  if (priv->have_xkb)
    {
      return XkbLockModifiers (spi_get_display (), XkbUseCoreKbd,
                               modifiers, 0);
    }
  else
    {
      int mod_index;
      if (xmkeymap == NULL)
        xmkeymap = XGetModifierMapping (spi_get_display ());

      for (mod_index = 0; mod_index < 8; mod_index++)
        if (modifiers & (1 << mod_index))
          spi_dec_x11_synth_keycode_release (controller, xmkeymap->modifiermap[mod_index]);
      return TRUE;
    }
}

static KeySym
keysym_for_unichar (SpiDEController *controller, gunichar unichar)
{
  return (KeySym) ucs2keysym ((long) unichar);
}

static gboolean
spi_dec_x11_synth_keystring (SpiDEController *controller, guint synth_type, gint keycode, const char *keystring)
{
  /* probably we need to create and inject an XIM handler eventually. */
  /* for now, try to match the string to existing
   * keycode+modifier states.
   */
  KeySym *keysyms;
  gint maxlen = 0;
  gunichar unichar = 0;
  gint i = 0;
  gboolean retval = TRUE;
  const gchar *c;
  KeySym keysym;

  if (!(keystring && *keystring && g_utf8_validate (keystring, -1, &c)))
    {
      retval = FALSE;
    }
  else
    {
#ifdef SPI_DEBUG
      fprintf (stderr, "[keystring synthesis attempted on %s]\n", keystring);
#endif
      maxlen = strlen (keystring) + 1;
      keysyms = g_new0 (KeySym, maxlen);

      while (keystring && (unichar = g_utf8_get_char (keystring)))
        {
          char bytes[6];
          gint mbytes;

          mbytes = g_unichar_to_utf8 (unichar, bytes);
          bytes[mbytes] = '\0';
#ifdef SPI_DEBUG
          fprintf (stderr, "[unichar %s]", bytes);
#endif
          keysym = keysym_for_unichar (controller, unichar);
          if (keysym == NoSymbol)
            {
#ifdef SPI_DEBUG
              fprintf (stderr, "no keysym for %s", bytes);
#endif
              retval = FALSE;
              break;
            }
          keysyms[i++] = keysym;
          keystring = g_utf8_next_char (keystring);
        }
      keysyms[i++] = 0;
      XSynchronize (spi_get_display (), TRUE);
      for (i = 0; keysyms[i]; ++i)
        {
          if (!spi_dec_synth_keysym (controller, keysyms[i]))
            {
#ifdef SPI_DEBUG
              fprintf (stderr, "could not synthesize %c\n",
                       (int) keysyms[i]);
#endif
              retval = FALSE;
              break;
            }
        }
      XSynchronize (spi_get_display (), FALSE);

      g_free (keysyms);
    }

  if (synth_type == Accessibility_KEY_SYM)
    {
      keysym = keycode;
    }
  else
    {
      keysym = XkbKeycodeToKeysym (spi_get_display (), keycode, 0, 0);
    }
  if (XkbKeysymToModifiers (spi_get_display (), keysym) == 0)
    {
      spi_dec_clear_unlatch_pending (controller);
    }
  return retval;
}

static void
spi_dec_x11_init (SpiDEController *controller)
{
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);

  spi_events_init (spi_get_display ());

  gettimeofday (&priv->last_press_time, NULL);
  gettimeofday (&priv->last_release_time, NULL);
  spi_controller_register_with_devices (controller);

  saved_controller = controller;
}

static void
spi_dec_x11_finalize (SpiDEController *controller)
{
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);

  /* disconnect any special listeners, get rid of outstanding keygrabs */
  XUngrabKey (spi_get_display (), AnyKey, AnyModifier, DefaultRootWindow (spi_get_display ()));

  if (priv->xkb_desc)
    XkbFreeKeyboard (priv->xkb_desc, 0, True);
  /* TODO: Should free the keymap */
}

static gboolean
spi_device_event_controller_forward_key_event (SpiDEController *controller,
                                               const XEvent *event)
{
  SpiDEControllerPrivate *priv = spi_device_event_controller_get_instance_private (controller);
  Accessibility_DeviceEvent key_event;
  gboolean ret;

  g_assert (event->type == KeyPress || event->type == KeyRelease);

  key_event = spi_keystroke_from_x_key_event ((XKeyEvent *) event);

  if (priv->xevie_display == NULL)
    spi_controller_update_key_grabs (controller, &key_event);

  /* relay to listeners, and decide whether to consume it or not */
  ret = spi_controller_notify_keylisteners (controller, &key_event, TRUE);
  g_free (key_event.event_string);
  return ret;
}

static gboolean
is_key_released (long code)
{
  char keys[32];
  int down;

  XQueryKeymap (spi_get_display (), keys);
  down = BIT (keys, code);
  return (down == 0);
}

static gboolean
check_release (gpointer data)
{
  gboolean released;
  Accessibility_DeviceEvent *event = (Accessibility_DeviceEvent *) data;
  KeyCode code = event->hw_code;

  released = is_key_released (code);

  if (released)
    {
      check_release_handler = 0;
      event->type = Accessibility_KEY_RELEASED_EVENT;
      spi_controller_notify_keylisteners (saved_controller, event, TRUE);
    }
  return (released == 0);
}

static void
wait_for_release_event (XEvent *event,
                        SpiDEController *controller)
{
  pressed_event = spi_keystroke_from_x_key_event ((XKeyEvent *) event);
  check_release_handler = g_timeout_add (CHECK_RELEASE_DELAY, check_release, &pressed_event);
  g_source_set_name_by_id (check_release_handler, "[at-spi2-core] check_release");
}

static void
spi_dec_x11_emit_modifier_event (SpiDEController *controller, guint prev_mask, guint current_mask)
{
  dbus_uint32_t d1, d2;

#ifdef SPI_XKB_DEBUG
  fprintf (stderr, "MODIFIER CHANGE EVENT! %x to %x\n",
           prev_mask, current_mask);
#endif

  /* set bits for the virtual modifiers like NUMLOCK */
  if (prev_mask & _numlock_physical_mask)
    prev_mask |= SPI_KEYMASK_NUMLOCK;
  if (current_mask & _numlock_physical_mask)
    current_mask |= SPI_KEYMASK_NUMLOCK;

  d1 = prev_mask & key_modifier_mask;
  d2 = current_mask & key_modifier_mask;
  spi_dec_dbus_emit (controller, SPI_DBUS_INTERFACE_EVENT_KEYBOARD, "Modifiers", "", d1, d2);
}

static void
spi_dec_x11_generate_mouse_event (SpiDEController *controller,
                                  gint x,
                                  gint y,
                                  const char *eventName)
{
  int button = 0;
  gboolean err = FALSE;
  Display *display = spi_get_display ();

  switch (eventName[0])
    {
    case 'b':
      switch (eventName[1])
        {
        /* TODO: check number of buttons before parsing */
        case '1':
          button = 1;
          break;
        case '2':
          button = 2;
          break;
        case '3':
          button = 3;
          break;
        case '4':
          button = 4;
          break;
        case '5':
          button = 5;
          break;
        default:
          err = TRUE;
        }
      if (!err)
        {
          if (x != -1 && y != -1)
            {
              XTestFakeMotionEvent (display, DefaultScreen (display),
                                    x, y, 0);
            }
          XTestFakeButtonEvent (display, button, !(eventName[2] == 'r'), 0);
          if (eventName[2] == 'c')
            XTestFakeButtonEvent (display, button, FALSE, 1);
          else if (eventName[2] == 'd')
            {
              XTestFakeButtonEvent (display, button, FALSE, 1);
              XTestFakeButtonEvent (display, button, TRUE, 2);
              XTestFakeButtonEvent (display, button, FALSE, 3);
            }
        }
      break;
    case 'r': /* relative motion */
      XTestFakeRelativeMotionEvent (display, x, y, 0);
      break;
    case 'a': /* absolute motion */
      XTestFakeMotionEvent (display, DefaultScreen (display),
                            x, y, 0);
      break;
    }
}

void
spi_dec_setup_x11 (SpiDEControllerClass *klass)
{
  klass->plat.get_keycode = spi_dec_x11_get_keycode;
  klass->plat.mouse_check = spi_dec_x11_mouse_check;
  klass->plat.synth_keycode_press = spi_dec_x11_synth_keycode_press;
  klass->plat.synth_keycode_release = spi_dec_x11_synth_keycode_release;
  klass->plat.lock_modifiers = spi_dec_x11_lock_modifiers;
  klass->plat.unlock_modifiers = spi_dec_x11_unlock_modifiers;
  klass->plat.synth_keystring = spi_dec_x11_synth_keystring;
  klass->plat.grab_key = spi_dec_x11_grab_key;
  klass->plat.ungrab_key = spi_dec_x11_ungrab_key;
  klass->plat.emit_modifier_event = spi_dec_x11_emit_modifier_event;
  klass->plat.generate_mouse_event = spi_dec_x11_generate_mouse_event;

  klass->plat.init = spi_dec_x11_init;
  klass->plat.finalize = spi_dec_x11_finalize;
}
