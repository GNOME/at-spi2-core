#include "atspi-private.h"

static void atspi_state_set_class_init (AtspiStateSetClass *klass);

G_DEFINE_TYPE (AtspiStateSet, atspi_state_set, G_TYPE_OBJECT)

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
atspi_state_set_new (gint64 states)
{
  AtspiStateSet *set;
  
  set = g_object_new (ATSPI_TYPE_STATE_SET, NULL);
  g_return_val_if_fail (set != NULL, NULL);

  set->states = states;
  return set;
}
