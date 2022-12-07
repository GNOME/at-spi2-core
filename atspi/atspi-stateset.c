/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include "atspi-private.h"

/**
 * AtspiStateSet:
 *
 * The atspi-stateset objects implement wrappers around a
 * bitmap of accessible states.
 */

static void atspi_state_set_class_init (AtspiStateSetClass *klass);

G_DEFINE_TYPE (AtspiStateSet, atspi_state_set, G_TYPE_OBJECT)

static void
atspi_state_set_init (AtspiStateSet *set)
{
  set->states = 0;
}

static void
atspi_state_set_class_init (AtspiStateSetClass *klass)
{
}

/**
 * atspi_state_set_new:
 * @states: (element-type AtspiStateType): An array of states with which the
 *          method initializes the state set.
 *
 * Generates an #AtspiStateSet with the given @states.
 *
 * Returns: A new #AtspiStateSet with the given states.
 **/
AtspiStateSet *
atspi_state_set_new (GArray *states)
{
  AtspiStateSet *set = g_object_new (ATSPI_TYPE_STATE_SET, NULL);
  gint i;

  if (!set || !states)
    return set;

  for (i = 0; i < states->len; i++)
    atspi_state_set_add (set, g_array_index (states, AtspiStateType, i));
  return set;
}

AtspiStateSet *
_atspi_state_set_new_internal (AtspiAccessible *accessible, gint64 states)
{
  AtspiStateSet *set;

  set = g_object_new (ATSPI_TYPE_STATE_SET, NULL);
  g_return_val_if_fail (set != NULL, NULL);

  set->accessible = accessible;
  set->states = states;
  return set;
}

/**
 * atspi_state_set_set_by_name:
 * @set: a pointer to the #AtspiStateSet object on which to operate.
 * @name: a string corresponding to a state name.
 * @enabled: if #TRUE, @name should be enabled in the @set in question;
 *          otherwise, it should be disabled.
 *
 * Enables/disables a state in an #AtspiStateSet according to its @name.
 **/
void
atspi_state_set_set_by_name (AtspiStateSet *set, const gchar *name, gboolean enabled)
{
  GTypeClass *type_class;
  GEnumValue *value;

  if (set->accessible &&
      !(set->accessible->cached_properties & ATSPI_CACHE_STATES))
    return;

  type_class = g_type_class_ref (ATSPI_TYPE_STATE_TYPE);

  value = g_enum_get_value_by_nick (G_ENUM_CLASS (type_class), name);

  if (!value)
    {
      g_warning ("AT-SPI: Attempt to set unknown state '%s'", name);
    }
  else if (enabled)
    set->states |= ((gint64) 1 << value->value);
  else
    set->states &= ~((gint64) 1 << value->value);

  g_type_class_unref (type_class);
}

static void
refresh_states (AtspiStateSet *set)
{
  GArray *state_array;
  dbus_uint32_t *states;

  if (!set->accessible ||
      (set->accessible->cached_properties & ATSPI_CACHE_STATES))
    return;

  if (!_atspi_dbus_call (set->accessible, atspi_interface_accessible, "GetState", NULL, "=>au", &state_array))
    return;

  states = (dbus_uint32_t *) state_array->data;

  set->states = ((gint64) states[1]) << 32;
  set->states |= (gint64) states[0];
  g_array_free (state_array, TRUE);
}

/**
 * atspi_state_set_add:
 * @set: a pointer to the #AtspiStateSet object on which to operate.
 * @state: an #AtspiStateType to be added to the specified #AtspiStateSet.
 *
 * Adds a particular #AtspiState to an #AtspiStateSet (i.e. sets the
 *       given state to #TRUE in the stateset).
 *
 **/
void
atspi_state_set_add (AtspiStateSet *set, AtspiStateType state)
{
  g_return_if_fail (set != NULL);
  set->states |= (((gint64) 1) << state);
}

/**
 * atspi_state_set_compare:
 * @set: a pointer to the first #AtspiStateSet object on which to operate.
 * @set2: a pointer to the second #AtspiStateSet object on which to operate.
 *
 * Determines the differences between two instances of #AtspiStateSet.
 *
 * @see #atspi_state_set_equals.
 *
 * Returns: (transfer full): an #AtspiStateSet object containing all states
 * contained on one of the two sets but not the other.
 *
 **/
AtspiStateSet *
atspi_state_set_compare (AtspiStateSet *set,
                         AtspiStateSet *set2)
{
  g_return_val_if_fail (set != NULL, NULL);
  g_return_val_if_fail (set2 != NULL, NULL);

  return _atspi_state_set_new_internal (NULL, set->states ^ set2->states);
}

/**
 * atspi_state_set_contains:
 * @set: a pointer to the #AtspiStateSet object on which to operate.
 * @state: an #AtspiStateType for which the specified #AtspiStateSet
 *          will be queried.
 *
 * Determines whether a given #AtspiStateSet includes a given state; that is,
 *          whether @state is true for the @set in question.
 *
 * Returns: #TRUE if @state is true/included in the given #AtspiStateSet,
 *          otherwise #FALSE.
 *
 **/
gboolean
atspi_state_set_contains (AtspiStateSet *set,
                          AtspiStateType state)
{
  if (!set)
    return FALSE;
  refresh_states (set);
  return (set->states & ((gint64) 1 << state)) ? TRUE : FALSE;
}

/**
 * atspi_state_set_equals:
 * @set: a pointer to the first #AtspiStateSet object on which to operate.
 * @set2: a pointer to the second #AtspiStateSet object on which to operate.
 *
 * Determines whether two instances of #AtspiStateSet are equivalent (i.e.
 *          consist of the same #AtspiStates).  Useful for checking multiple
 *          state variables at once.
 *
 * @see #atspi_state_set_compare.
 *
 * Returns: #TRUE if the two #AtspiStateSets are equivalent,
 * otherwise #FALSE.
 *
 **/
gboolean
atspi_state_set_equals (AtspiStateSet *set,
                        AtspiStateSet *set2)
{
  if (set == set2)
    return TRUE;
  if (set == NULL || set2 == NULL)
    return FALSE;
  return (set->states == set2->states);
}

/**
 * atspi_state_set_get_states:
 * @set: The #AtspiStateSet to be queried.
 *
 * Returns the states in an #AtspiStateSet as an array.
 *
 * Returns: (element-type AtspiStateType) (transfer full): A #GArray of state
 *          types representing the current state.
 **/
GArray *
atspi_state_set_get_states (AtspiStateSet *set)
{
  gint i = 0;
  guint64 val = 1;
  GArray *ret;

  g_return_val_if_fail (set != NULL, NULL);
  refresh_states (set);
  ret = g_array_new (TRUE, TRUE, sizeof (AtspiStateType));
  if (!ret)
    return NULL;
  for (i = 0; i < 64; i++)
    {
      if (set->states & val)
        ret = g_array_append_val (ret, i);
      val <<= 1;
    }
  return ret;
}

/**
 * atspi_state_set_is_empty:
 * @set: The #AtspiStateSet to query.
 *
 * Returns: #TRUE if the state set contains no states; #FALSE otherwise.
 **/
gboolean
atspi_state_set_is_empty (AtspiStateSet *set)
{
  return (set->states == 0);
}

/**
 * atspi_state_set_remove:
 * @set: a pointer to the #AtspiStateSet object on which to operate.
 * @state: an #AtspiStateType to remove from the specified @set.
 *
 * Removes a particular #AtspiState to an #AtspiStateSet (i.e. sets the
 *       given state to #FALSE in the stateset.)
 *
 **/
void
atspi_state_set_remove (AtspiStateSet *set, AtspiStateType state)
{
  g_return_if_fail (set != NULL);
  set->states &= ~((gint64) 1 << state);
}
