/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2024 Red Hat
 *
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

#pragma once
#include "glib-object.h"

#include "atspi-device.h"
#include "atspi-types.h"

G_BEGIN_DECLS

#define ATSPI_DEVICE_A11Y_MANAGER_VIRTUAL_MOD_START 15 /* To avoid the clash with ATSPI_MODIFIER_NUMLOCK */
#define ATSPI_DEVICE_A11Y_MANAGER_VIRTUAL_MOD_END 31

#define ATSPI_TYPE_DEVICE_A11Y_MANAGER (atspi_device_a11y_manager_get_type ())
G_DECLARE_FINAL_TYPE (AtspiDeviceA11yManager, atspi_device_a11y_manager, ATSPI, DEVICE_A11Y_MANAGER, AtspiDevice)

AtspiDeviceA11yManager *atspi_device_a11y_manager_try_new ();

AtspiDeviceA11yManager *atspi_device_a11y_manager_try_new_full (const gchar *app_id);

G_END_DECLS
