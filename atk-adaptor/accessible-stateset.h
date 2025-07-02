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

#pragma once

#include "atk/atk.h"
#include "atspi/atspi.h"

G_BEGIN_DECLS

/* private - internal API to abstract away atk API */
AtkStateSet *spi_state_set_cache_from_sequence (GArray *seq);
AtkState spi_atk_state_from_spi_state (AtspiStateType state);
void spi_atk_state_to_dbus_array (AtkObject *object, dbus_uint32_t *array);
void spi_atk_state_set_to_dbus_array (AtkStateSet *set, dbus_uint32_t *array);
#define spi_state_set_cache_ref(s) g_object_ref (s)
#define spi_state_set_cache_unref(s) g_object_unref (s)
#define spi_state_set_cache_new(seq) spi_state_set_cache_from_sequence (seq)
#define spi_state_set_cache_contains(s, a) atk_state_set_contains_state (ATK_STATE_SET (s), \
                                                                         spi_atk_state_from_spi_state (a))
#define spi_state_set_cache_add(s, a) atk_state_set_add_state (ATK_STATE_SET (s), \
                                                               spi_atk_state_from_spi_state (a))
#define spi_state_set_cache_remove(s, a) atk_state_set_remove_state (ATK_STATE_SET (s), \
                                                                     spi_atk_state_from_spi_state (a))
#define spi_state_set_cache_xor(a, b) atk_state_set_xor_sets (ATK_STATE_SET (a), ATK_STATE_SET (b))
#define spi_state_set_cache_is_empty(a) atk_state_set_is_empty (ATK_STATE_SET (a))

G_END_DECLS
