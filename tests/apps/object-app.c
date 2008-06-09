#include <gmodule.h>
#include <atk/atk.h>
#include <my-atk.h>

#include "atk-object-xml-loader.h"

static gchar *tdata_path = NULL;

static AtkObject *root_accessible;

#define OBJECT_TEST_1 "object-test-stage1.xml"

G_MODULE_EXPORT void
test_init (gchar *path)
{
  gchar *td;

  if (path == NULL)
     g_error("No test data path provided");
  tdata_path = path;

  td = g_build_path(G_DIR_SEPARATOR_S, tdata_path, OBJECT_TEST_1, NULL);
  root_accessible = ATK_OBJECT(atk_object_xml_parse(td));
  g_free(td);
}

G_MODULE_EXPORT AtkObject *
test_get_root (void)
{
  return root_accessible;
}
