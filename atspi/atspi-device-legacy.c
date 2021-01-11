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

#include "atspi-private.h"
#include "atspi-device-legacy.h"

typedef struct _AtspiDeviceLegacyPrivate AtspiDeviceLegacyPrivate;
struct _AtspiDeviceLegacyPrivate
{
  AtspiDeviceListener *listener;
};

GObjectClass *device_legacy_parent_class;

G_DEFINE_TYPE_WITH_CODE (AtspiDeviceLegacy, atspi_device_legacy,
			  ATSPI_TYPE_DEVICE,
			  G_ADD_PRIVATE (AtspiDeviceLegacy))

gboolean
key_cb (const AtspiDeviceEvent *event, void *user_data)
{
  AtspiDeviceLegacy *device = ATSPI_DEVICE_LEGACY (user_data);

   return atspi_device_notify_key (ATSPI_DEVICE (device), event->type == (AtspiEventType)ATSPI_KEY_PRESS, event->hw_code, event->id, event->modifiers, event->event_string);
}

static void
atspi_device_legacy_init (AtspiDeviceLegacy *device)
{
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (device);
  gint i;

  priv->listener = atspi_device_listener_new (key_cb, device, NULL);
  for (i = 0; i < 256; i++)
    atspi_register_keystroke_listener (priv->listener, NULL, i, 3, ATSPI_KEYLISTENER_SYNCHRONOUS | ATSPI_KEYLISTENER_CANCONSUME, NULL);
}

static void
atspi_device_legacy_finalize (GObject *object)
{
  AtspiDeviceLegacy *device = ATSPI_DEVICE_LEGACY (object);
  AtspiDeviceLegacyPrivate *priv = atspi_device_legacy_get_instance_private (device);

  g_clear_object (&priv->listener);
  device_legacy_parent_class->finalize (object);
}


static void
atspi_device_legacy_class_init (AtspiDeviceLegacyClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  device_legacy_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = atspi_device_legacy_finalize;
}

/**
 * atspi_device_legacy_new:
 *
 * Creates a new #AtspiDeviceLegacy.
 *
 * Returns: (transfer full): a pointer to a newly-created #AtspiDeviceLegacy.
 *
 **/
AtspiDeviceLegacy *
atspi_device_legacy_new ()
{
  AtspiDeviceLegacy *device = g_object_new (atspi_device_legacy_get_type (), NULL);

  return device;
}
