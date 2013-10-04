/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
 * Copyright 2013 Igalia S.L.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <atk/atk.h>

#define EXPECTED_NUMBER 5

GMainLoop *global_loop = NULL;
gint global_number_emissions = 0;

#define TEST_TYPE_DOCUMENT                         (test_document_get_type ())
#define TEST_DOCUMENT(obj)                         (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_DOCUMENT, TestDocument))
#define TEST_DOCUMENT_CLASS(klass)                 (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_TYPE_DOCUMENT, TestDocumentClass))
#define TEST_IS_DOCUMENT(obj)                      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_DOCUMENT))
#define TEST_IS_DOCUMENT_CLASS(klass)              (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_TYPE_DOCUMENT))
#define TEST_DOCUMENT_GET_CLASS(obj)               (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_TYPE_DOCUMENT, TestDocumentClass))

typedef struct _TestDocument        TestDocument;
typedef struct _TestDocumentClass   TestDocumentClass;

struct _TestDocument
{
  AtkObject parent;
};

struct _TestDocumentClass
{
  AtkObjectClass parent_class;
};

GType       test_document_get_type (void) G_GNUC_CONST;
static void test_document_interface_init (AtkDocumentIface *iface);

G_DEFINE_TYPE_WITH_CODE (TestDocument,
                         test_document,
                         ATK_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_DOCUMENT,
                                                test_document_interface_init));

static void
test_document_class_init (TestDocumentClass *klass)
{
}

static void
test_document_init (TestDocument *document)
{
}

static void
test_document_interface_init (AtkDocumentIface *iface)
{
}

static void
document_page_changed_cb (AtkDocument *document,
                          gint page_number,
                          gpointer data)
{
  g_print ("Page-changed callback, page_number = %i\n", page_number);
  global_number_emissions++;
}

static gboolean
document_emit_page_changed (gpointer data)
{
  TestDocument* test_document = TEST_DOCUMENT (data);
  static gint next_page = 1;

  g_print ("Moving to next page. Emitting page-change, page_number = %i\n",
           next_page);
  g_signal_emit_by_name (test_document, "page-changed", next_page++, NULL);

  if (next_page > EXPECTED_NUMBER) {
    g_main_loop_quit (global_loop);
    return G_SOURCE_REMOVE;
  } else
    return G_SOURCE_CONTINUE;
}

static gboolean
init_test_document (void)
{
  GObject *my_document;

  my_document = g_object_new (TEST_TYPE_DOCUMENT, NULL);

  g_signal_connect (my_document, "page-changed",
                    G_CALLBACK (document_page_changed_cb),
                    NULL);

  g_idle_add (document_emit_page_changed, my_document);

  return TRUE;
}


int
main (gint  argc,
      char* argv[])
{
  global_loop = g_main_loop_new (NULL, FALSE);

  g_print("Starting Document test suite\n");

  init_test_document ();
  g_main_loop_run (global_loop);

  if (global_number_emissions == EXPECTED_NUMBER)
    g_print ("Document tests succeeded\n");
  else
    g_print ("Document tests failed\n");

  return 0;
}
