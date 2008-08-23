#include <gmodule.h>
#include <atk/atk.h>
#include <my-atk.h>

static AtkObject *root_accessible;

G_MODULE_EXPORT void
test_init (gchar *path)
{
  root_accessible  = g_object_new(MY_TYPE_ATK_ACTION, NULL);
}

G_MODULE_EXPORT void
test_next (int argc, char *argv[])
{
  g_print("Moving to next stage\n");
}

G_MODULE_EXPORT void
test_finished (int argc, char *argv[])
{
  g_print("Test has completed\n");
}

G_MODULE_EXPORT AtkObject *
test_get_root (void)
{
  return root_accessible;
}
