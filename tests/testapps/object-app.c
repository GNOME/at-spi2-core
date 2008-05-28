#include <gmodule.h>
#include <atk/atk.h>
#include <my-atk.h>

static AtkObject *root_accessible;

G_MODULE_EXPORT void
test_init (void)
{
  root_accessible = (AtkObject *) g_object_new(MY_TYPE_ATK_OBJECT, NULL);

  atk_object_set_name(root_accessible, "Root Accessible");
  atk_object_set_role(root_accessible, ATK_ROLE_ACCEL_LABEL);
  atk_object_set_description(root_accessible, "A root object for the test tree"); 
}

G_MODULE_EXPORT AtkObject *
test_get_root (void)
{
  return root_accessible;
}
