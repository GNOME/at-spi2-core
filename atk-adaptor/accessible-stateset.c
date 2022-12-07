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

/* stateset.c : implements the StateSet interface */

#include "config.h"

#include "accessible-stateset.h"
#include "bitarray.h"

#include <stdio.h>

static AtspiStateType *accessible_state_types = NULL;
static AtkStateType *atk_state_types = NULL;

static gboolean
spi_init_state_type_tables (void)
{
  gint i;

  if (accessible_state_types || atk_state_types)
    return FALSE;
  if (!accessible_state_types)
    accessible_state_types = g_new (AtspiStateType, ATK_STATE_LAST_DEFINED);
  if (!atk_state_types)
    atk_state_types = g_new (AtkStateType, ATSPI_STATE_LAST_DEFINED);
  g_return_val_if_fail (accessible_state_types, FALSE);
  g_return_val_if_fail (atk_state_types, FALSE);

  for (i = 0; i < ATSPI_STATE_LAST_DEFINED; i++)
    {
      atk_state_types[i] = ATK_STATE_INVALID;
    }

  for (i = 0; i < ATK_STATE_LAST_DEFINED; i++)
    {
      accessible_state_types[i] = ATSPI_STATE_INVALID;
    }

  accessible_state_types[ATK_STATE_ACTIVE] = ATSPI_STATE_ACTIVE;
  atk_state_types[ATSPI_STATE_ACTIVE] = ATK_STATE_ACTIVE;
  accessible_state_types[ATK_STATE_ANIMATED] = ATSPI_STATE_ANIMATED;
  atk_state_types[ATSPI_STATE_ANIMATED] = ATK_STATE_ANIMATED;
  accessible_state_types[ATK_STATE_ARMED] = ATSPI_STATE_ARMED;
  atk_state_types[ATSPI_STATE_ARMED] = ATK_STATE_ARMED;
  accessible_state_types[ATK_STATE_BUSY] = ATSPI_STATE_BUSY;
  atk_state_types[ATSPI_STATE_BUSY] = ATK_STATE_BUSY;
  accessible_state_types[ATK_STATE_CHECKED] = ATSPI_STATE_CHECKED;
  atk_state_types[ATSPI_STATE_CHECKED] = ATK_STATE_CHECKED;
  accessible_state_types[ATK_STATE_DEFUNCT] = ATSPI_STATE_DEFUNCT;
  atk_state_types[ATSPI_STATE_DEFUNCT] = ATK_STATE_DEFUNCT;
  accessible_state_types[ATK_STATE_EDITABLE] = ATSPI_STATE_EDITABLE;
  atk_state_types[ATSPI_STATE_EDITABLE] = ATK_STATE_EDITABLE;
  accessible_state_types[ATK_STATE_ENABLED] = ATSPI_STATE_ENABLED;
  atk_state_types[ATSPI_STATE_ENABLED] = ATK_STATE_ENABLED;
  accessible_state_types[ATK_STATE_EXPANDABLE] = ATSPI_STATE_EXPANDABLE;
  atk_state_types[ATSPI_STATE_EXPANDABLE] = ATK_STATE_EXPANDABLE;
  accessible_state_types[ATK_STATE_EXPANDED] = ATSPI_STATE_EXPANDED;
  atk_state_types[ATSPI_STATE_EXPANDED] = ATK_STATE_EXPANDED;
  accessible_state_types[ATK_STATE_FOCUSABLE] = ATSPI_STATE_FOCUSABLE;
  atk_state_types[ATSPI_STATE_FOCUSABLE] = ATK_STATE_FOCUSABLE;
  accessible_state_types[ATK_STATE_FOCUSED] = ATSPI_STATE_FOCUSED;
  atk_state_types[ATSPI_STATE_FOCUSED] = ATK_STATE_FOCUSED;
  accessible_state_types[ATK_STATE_HORIZONTAL] = ATSPI_STATE_HORIZONTAL;
  atk_state_types[ATSPI_STATE_HORIZONTAL] = ATK_STATE_HORIZONTAL;
  accessible_state_types[ATK_STATE_ICONIFIED] = ATSPI_STATE_ICONIFIED;
  atk_state_types[ATSPI_STATE_ICONIFIED] = ATK_STATE_ICONIFIED;
  accessible_state_types[ATK_STATE_MODAL] = ATSPI_STATE_MODAL;
  atk_state_types[ATSPI_STATE_MODAL] = ATK_STATE_MODAL;
  accessible_state_types[ATK_STATE_MULTI_LINE] = ATSPI_STATE_MULTI_LINE;
  atk_state_types[ATSPI_STATE_MULTI_LINE] = ATK_STATE_MULTI_LINE;
  accessible_state_types[ATK_STATE_MULTISELECTABLE] = ATSPI_STATE_MULTISELECTABLE;
  atk_state_types[ATSPI_STATE_MULTISELECTABLE] = ATK_STATE_MULTISELECTABLE;
  accessible_state_types[ATK_STATE_OPAQUE] = ATSPI_STATE_OPAQUE;
  atk_state_types[ATSPI_STATE_OPAQUE] = ATK_STATE_OPAQUE;
  accessible_state_types[ATK_STATE_PRESSED] = ATSPI_STATE_PRESSED;
  atk_state_types[ATSPI_STATE_PRESSED] = ATK_STATE_PRESSED;
  accessible_state_types[ATK_STATE_RESIZABLE] = ATSPI_STATE_RESIZABLE;
  atk_state_types[ATSPI_STATE_RESIZABLE] = ATK_STATE_RESIZABLE;
  accessible_state_types[ATK_STATE_SELECTABLE] = ATSPI_STATE_SELECTABLE;
  atk_state_types[ATSPI_STATE_SELECTABLE] = ATK_STATE_SELECTABLE;
  accessible_state_types[ATK_STATE_SELECTED] = ATSPI_STATE_SELECTED;
  atk_state_types[ATSPI_STATE_SELECTED] = ATK_STATE_SELECTED;
  accessible_state_types[ATK_STATE_SENSITIVE] = ATSPI_STATE_SENSITIVE;
  atk_state_types[ATSPI_STATE_SENSITIVE] = ATK_STATE_SENSITIVE;
  accessible_state_types[ATK_STATE_SHOWING] = ATSPI_STATE_SHOWING;
  atk_state_types[ATSPI_STATE_SHOWING] = ATK_STATE_SHOWING;
  accessible_state_types[ATK_STATE_SINGLE_LINE] = ATSPI_STATE_SINGLE_LINE;
  atk_state_types[ATSPI_STATE_SINGLE_LINE] = ATK_STATE_SINGLE_LINE;
  accessible_state_types[ATK_STATE_STALE] = ATSPI_STATE_STALE;
  atk_state_types[ATSPI_STATE_STALE] = ATK_STATE_STALE;
  accessible_state_types[ATK_STATE_TRANSIENT] = ATSPI_STATE_TRANSIENT;
  atk_state_types[ATSPI_STATE_TRANSIENT] = ATK_STATE_TRANSIENT;
  accessible_state_types[ATK_STATE_VERTICAL] = ATSPI_STATE_VERTICAL;
  atk_state_types[ATSPI_STATE_VERTICAL] = ATK_STATE_VERTICAL;
  accessible_state_types[ATK_STATE_VISIBLE] = ATSPI_STATE_VISIBLE;
  atk_state_types[ATSPI_STATE_VISIBLE] = ATK_STATE_VISIBLE;
  accessible_state_types[ATK_STATE_MANAGES_DESCENDANTS] = ATSPI_STATE_MANAGES_DESCENDANTS;
  atk_state_types[ATSPI_STATE_MANAGES_DESCENDANTS] = ATK_STATE_MANAGES_DESCENDANTS;
  accessible_state_types[ATK_STATE_INDETERMINATE] = ATSPI_STATE_INDETERMINATE;
  atk_state_types[ATSPI_STATE_INDETERMINATE] = ATK_STATE_INDETERMINATE;
  accessible_state_types[ATK_STATE_TRUNCATED] = ATSPI_STATE_TRUNCATED;
  atk_state_types[ATSPI_STATE_TRUNCATED] = ATK_STATE_TRUNCATED;
  accessible_state_types[ATK_STATE_REQUIRED] = ATSPI_STATE_REQUIRED;
  atk_state_types[ATSPI_STATE_REQUIRED] = ATK_STATE_REQUIRED;
  accessible_state_types[ATK_STATE_INVALID_ENTRY] = ATSPI_STATE_INVALID_ENTRY;
  atk_state_types[ATSPI_STATE_INVALID_ENTRY] = ATK_STATE_INVALID_ENTRY;
  accessible_state_types[ATK_STATE_SUPPORTS_AUTOCOMPLETION] = ATSPI_STATE_SUPPORTS_AUTOCOMPLETION;
  atk_state_types[ATSPI_STATE_SUPPORTS_AUTOCOMPLETION] = ATK_STATE_SUPPORTS_AUTOCOMPLETION;
  accessible_state_types[ATK_STATE_SELECTABLE_TEXT] = ATSPI_STATE_SELECTABLE_TEXT;
  atk_state_types[ATSPI_STATE_SELECTABLE_TEXT] = ATK_STATE_SELECTABLE_TEXT;
  accessible_state_types[ATK_STATE_DEFAULT] = ATSPI_STATE_IS_DEFAULT;
  atk_state_types[ATSPI_STATE_IS_DEFAULT] = ATK_STATE_DEFAULT;
  accessible_state_types[ATK_STATE_VISITED] = ATSPI_STATE_VISITED;
  atk_state_types[ATSPI_STATE_VISITED] = ATK_STATE_VISITED;
  accessible_state_types[ATK_STATE_HAS_POPUP] = ATSPI_STATE_HAS_POPUP;
  atk_state_types[ATSPI_STATE_HAS_POPUP] = ATK_STATE_HAS_POPUP;
  accessible_state_types[ATK_STATE_CHECKABLE] = ATSPI_STATE_CHECKABLE;
  atk_state_types[ATSPI_STATE_CHECKABLE] = ATK_STATE_CHECKABLE;
  accessible_state_types[ATK_STATE_HAS_TOOLTIP] = ATSPI_STATE_HAS_TOOLTIP;
  atk_state_types[ATSPI_STATE_HAS_TOOLTIP] = ATK_STATE_HAS_TOOLTIP;
  accessible_state_types[ATK_STATE_READ_ONLY] = ATSPI_STATE_READ_ONLY;
  atk_state_types[ATSPI_STATE_READ_ONLY] = ATK_STATE_READ_ONLY;
  accessible_state_types[ATK_STATE_COLLAPSED] = ATSPI_STATE_COLLAPSED;
  atk_state_types[ATSPI_STATE_COLLAPSED] = ATK_STATE_COLLAPSED;

  return TRUE;
}

static inline AtkState
state_spi_to_atk (AtspiStateType state)
{
  guint idx = state;
  if (idx < ATSPI_STATE_LAST_DEFINED)
    return atk_state_types[idx];
  else
    return ATK_STATE_INVALID;
}

AtkState
spi_atk_state_from_spi_state (AtspiStateType state)
{
  spi_init_state_type_tables ();
  return state_spi_to_atk (state);
}

AtkStateSet *
spi_state_set_cache_from_sequence (GArray *seq)
{
  int i;
  AtkStateSet *set;
  AtkStateType *states;

  spi_init_state_type_tables ();

  states = g_newa (AtkStateType, seq->len);
  for (i = 0; i < seq->len; i++)
    states[i] = state_spi_to_atk (g_array_index (seq, dbus_int32_t, i));

  set = atk_state_set_new ();
  atk_state_set_add_states (set, states, seq->len);

  g_array_free (seq, TRUE);
  return set;
}

void
spi_atk_state_to_dbus_array (AtkObject *object, dbus_uint32_t *array)
{
  AtkStateSet *set = atk_object_ref_state_set (object);
  spi_atk_state_set_to_dbus_array (set, array);
  g_clear_object (&set);
}

void
spi_atk_state_set_to_dbus_array (AtkStateSet *set, dbus_uint32_t *array)
{
  int i;

  array[0] = 0;
  array[1] = 0;
  if (!set)
    return;
  spi_init_state_type_tables ();

  g_assert (ATK_STATE_LAST_DEFINED <= 64);
  for (i = 0; i < ATK_STATE_LAST_DEFINED; i++)
    {
      if (atk_state_set_contains_state (set, i))
        {
          int a = accessible_state_types[i];
          g_assert (a < 64);
          BITARRAY_SET (array, a);
        }
    }
}
