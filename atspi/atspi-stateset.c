#include "atspi-private.h"

static void atspi_state_set_class_init (AtspiStateSetClass *klass);

G_DEFINE_TYPE (AtspiStateSet, atspi_state_set, G_TYPE_OBJECT)

static const char *state_names [] =
{
  "invalid",
  "active",
  "armed",
  "busy",
  "checked",
  "collapsed",
  "defunct",
  "editable",
  "enabled",
  "expandable",
  "expanded",
  "focusable",
  "focused",
  "has-tool-tip",
  "horizontal",
  "iconified",
  "modal",
  "multi-line",
  "multiselectable",
  "opaque",
  "pressed",
  "resizable",
  "selectable",
  "selected",
  "sensitive",
  "showing",
  "singleLine",
  "stale",
  "transient",
  "vertical",
  "visible",
  "manages-descendants",
  "indeterminate",
  "required",
  "truncated",
  "animated",
  "invalid-entry",
  "supports-autocompletion",
  "selectable-text",
  "is-default",
  "visited",
  NULL
};

static void
atspi_state_set_init (AtspiStateSet *set)
{
  set->states = 0;
}

static void
atspi_state_set_class_init (AtspiStateSetClass* klass)
{
}

AtspiStateSet *
atspi_state_set_new (AtspiAccessible *accessible, gint64 states)
{
  AtspiStateSet *set;
  
  set = g_object_new (ATSPI_TYPE_STATE_SET, NULL);
  g_return_val_if_fail (set != NULL, NULL);

  set->accessible = accessible;
  set->states = states;
  return set;
}

void atspi_state_set_set_by_name (AtspiStateSet *set, const gchar *name, gboolean enabled)
{
  gint i = 0;

  if (!(set->accessible->cached_properties & ATSPI_CACHE_STATES))
    return;

  /* TODO: This could perhaps be optimized */
  for (i = 0; state_names [i]; i++)
  {
    if (!strcmp (state_names [i], name))
    {
      if (enabled)
        set->states |= (1 << i);
      else
        set->states &= ~(1 << i);
      return;
    }
  }
  g_warning ("at-spi: Attempt to set unknown state '%s'", name);
}

static void
refresh_states (AtspiStateSet *set)
{
  GArray *state_array;
  dbus_uint32_t *states;

  if ((set->accessible->cached_properties & ATSPI_CACHE_STATES))
    return;

  if (!_atspi_dbus_call (set->accessible, atspi_interface_accessible, "GetState", NULL, "=>au", &state_array))
    return;

  states = (dbus_uint32_t *) state_array->data;

  set->states = ((gint64)states [1]) << 32;
  set->states += states [0];
  g_array_free (state_array, TRUE);
}

/**
 * atspi_state_set_contains:
 * @set: a pointer to the #AtspiStateSet object on which to operate.
 * @state: an #AtspiStateType for which the specified #AtspiStateSet
 *       will be queried.
 *
 * Determine whether a given #AtspiStateSet includes a given state; that is,
 *       whether @state is true for the stateset in question.
 *
 * Returns: #TRUE if @state is true/included in the given #AtspiStateSet,
 *          otherwise #FALSE.
 *
 **/
gboolean
atspi_state_set_contains (AtspiStateSet *set,
			     AtspiStateType state)
{
  refresh_states (set);
  return (set->states & (1 << state)) ? TRUE : FALSE;
}
