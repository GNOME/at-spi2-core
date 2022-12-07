/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
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

#ifndef SPI_DEVICE_EVENT_CONTROLLER_H_
#define SPI_DEVICE_EVENT_CONTROLLER_H_

#include <dbus/dbus.h>

typedef struct _SpiDEController SpiDEController;

#include "de-types.h"
#include "registry.h"

G_BEGIN_DECLS

#define SPI_DEVICE_EVENT_CONTROLLER_TYPE (spi_device_event_controller_get_type ())
#define SPI_DEVICE_EVENT_CONTROLLER(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDEController))
#define SPI_DEVICE_EVENT_CONTROLLER_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDEControllerClass))
#define SPI_IS_DEVICE_EVENT_CONTROLLER(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_DEVICE_EVENT_CONTROLLER_TYPE))
#define SPI_IS_DEVICE_EVENT_CONTROLLER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_DEVICE_EVENT_CONTROLLER_TYPE))
#define SPI_DEVICE_EVENT_CONTROLLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SPI_DEVICE_EVENT_CONTROLLER_TYPE, SpiDEControllerClass))

struct _SpiDEController
{
  GObject parent;
  DBusConnection *bus;
  GList *key_listeners;
  GList *mouse_listeners;
  GList *keygrabs_list;
  GQueue *message_queue;
  guint message_queue_idle;

  guint mouse_mask_state;
  gboolean have_mouse_event_listener;
};

typedef enum
{
  SPI_DEVICE_TYPE_KBD,
  SPI_DEVICE_TYPE_LAST_DEFINED
} SpiDeviceTypeCategory;

typedef struct
{
  char *bus_name;
  char *path;
  SpiDeviceTypeCategory type;
  gulong types;
} DEControllerListener;

typedef struct
{
  DEControllerListener listener;

  GSList *keys;
  Accessibility_ControllerEventMask mask;
  Accessibility_EventListenerMode *mode;
} DEControllerKeyListener;

typedef struct
{
  gint (*get_keycode) (SpiDEController *controller,
                       gint keysym,
                       gchar *key_str,
                       gboolean fix,
                       guint *modmask);

  guint (*mouse_check) (SpiDEController *controller,
                        gint *x,
                        gint *y,
                        gboolean *moved);

  gboolean (*register_global_keygrabs) (SpiDEController *controller,
                                        DEControllerKeyListener *key_listener);

  void (*deregister_global_keygrabs) (SpiDEController *controller,
                                      DEControllerKeyListener *key_listener);

  gboolean (*synth_keycode_press) (SpiDEController *controller,
                                   guint keycode);

  gboolean (*synth_keycode_release) (SpiDEController *controller,
                                     guint keycode);

  gboolean (*lock_modifiers) (SpiDEController *controller,
                              unsigned modifiers);

  gboolean (*unlock_modifiers) (SpiDEController *controller,
                                unsigned modifiers);

  gboolean (*synth_keystring) (SpiDEController *controller,
                               guint synth_type,
                               gint keycode,
                               const char *keystring);

  gboolean (*grab_key) (SpiDEController *controller,
                        guint key_val,
                        Accessibility_ControllerEventMask mod_mask);

  void (*ungrab_key) (SpiDEController *controller,
                      guint key_val,
                      Accessibility_ControllerEventMask mod_mask);

  void (*emit_modifier_event) (SpiDEController *controller,
                               guint prev_mask,
                               guint current_mask);

  void (*generate_mouse_event) (SpiDEController *controller,
                                gint x,
                                gint y,
                                const char *eventName);

  void (*init) (SpiDEController *controller);
  void (*finalize) (SpiDEController *controller);
} SpiDEControllerPlat;

typedef struct
{
  GObjectClass parent_class;
  SpiDEControllerPlat plat;
} SpiDEControllerClass;

GType spi_device_event_controller_get_type (void);

void spi_device_event_controller_start_poll_mouse (SpiDEController *dec);
void spi_device_event_controller_stop_poll_mouse (SpiDEController *dec);

void spi_remove_device_listeners (SpiDEController *controller, const char *bus_name);

SpiDEController *spi_registry_dec_new (DBusConnection *bus);

gboolean
spi_controller_notify_keylisteners (SpiDEController *controller,
                                    Accessibility_DeviceEvent *key_event,
                                    dbus_bool_t is_system_global);

gboolean spi_controller_update_key_grabs (SpiDEController *controller,
                                          Accessibility_DeviceEvent *recv);

gboolean spi_dec_synth_keysym (SpiDEController *controller, long keysym);

void spi_dec_dbus_emit (SpiDEController *controller, const char *interface, const char *name, const char *minor, int a1, int a2);

G_END_DECLS

#endif /* DEVICEEVENTCONTROLLER_H_ */
