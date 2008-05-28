#include <gmodule.h>
#include <atk/atk.h>

G_MODULE_EXPORT void
test_init (void)
{
  ;
}

G_MODULE_EXPORT AtkObject *
test_get_root (void)
{
  return NULL;
}
