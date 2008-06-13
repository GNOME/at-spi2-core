#include <gmodule.h>
#include <atk/atk.h>

G_MODULE_EXPORT void
test_init (int argc, char *argv[])
{
  ;
}

G_MODULE_EXPORT void
test_next (int argc, char *argv[])
{
  ;
}

G_MODULE_EXPORT void
test_finished (int argc, char *argv[])
{
  ;
}

G_MODULE_EXPORT AtkObject *
test_get_root (void)
{
  return NULL;
}
