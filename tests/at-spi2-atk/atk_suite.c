/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "atk_suite.h"
#include "atk_test_util.h"

static gchar *tdata_list = NULL;
static gchar *one_test = NULL;

typedef struct _Atk_Test_Case Atk_Test_Case;

struct _Atk_Test_Case {
  const char *test_case;
  void (*build)( void);
};

static const Atk_Test_Case atc[] = {
  { ATK_TEST_PATH_ACCESSIBLE, atk_test_accessible },
  { ATK_TEST_PATH_ACTION, atk_test_action },
  { ATK_TEST_PATH_COMP, atk_test_component },
  { ATK_TEST_PATH_COLLECTION, atk_test_collection },
  { ATK_TEST_PATH_DOC, atk_test_document },
  { ATK_TEST_PATH_EDIT_TEXT, atk_test_editable_text },
  { ATK_TEST_PATH_HYPERLINK, atk_test_hyperlink },
  { ATK_TEST_PATH_HYPERTEXT, atk_test_hypertext },
  { ATK_TEST_PATH_IMAGE, atk_test_image },
  { ATK_TEST_PATH_SELECTION, atk_test_selection },
  { ATK_TEST_PATH_STATE_SET, atk_test_state_set },
  { ATK_TEST_PATH_TABLE, atk_test_table },
  { ATK_TEST_PATH_TABLE_CELL, atk_test_table_cell },
  { ATK_TEST_PATH_TEXT, atk_test_text },
  { ATK_TEST_PATH_VALUE, atk_test_value },
  { NULL, NULL}
};

static void
_list_tests (void)
{
  const Atk_Test_Case *itr;

  itr = atc;
  g_print ("Available Test Cases:\n");
  for (; itr->test_case; itr++)
    g_print ("\t%s\n", itr->test_case);
}

static void
atk_suite_build (int argc, char **argv )
{
  g_test_init (&argc, &argv, NULL);
  atk_test_accessible ();
  atk_test_action ();
  atk_test_component ();
  atk_test_collection ();
  atk_test_document ();

  atk_test_editable_text ();
  atk_test_hyperlink ();
  atk_test_hypertext ();
  atk_test_image ();
  atk_test_selection ();
  atk_test_state_set ();
  atk_test_table ();
  atk_test_table_cell ();
  atk_test_text ();
  atk_test_value ();
}

static GOptionEntry optentries[] = {
  {"list", 'l', 0, G_OPTION_ARG_NONE, &tdata_list, "Display all available test cases", NULL},
  {"separate", 0, 0, G_OPTION_ARG_STRING, &one_test, "Run only NAME test", "NAME"},
  {NULL}
};

int
main(int argc, char **argv)
{
  int test_result;
  GOptionContext *opt;
  opt = g_option_context_new (NULL);
  g_option_context_add_main_entries (opt, optentries, NULL);
  g_option_context_set_help_enabled (opt, TRUE);
  g_option_context_set_ignore_unknown_options (opt, TRUE);

  if (!g_option_context_parse (opt, &argc, &argv, NULL))
    return EXIT_FAILURE;

  if (tdata_list) {
    _list_tests();
    return EXIT_SUCCESS;
  }

  clean_exit_on_fail ();

  if (one_test) {
    if (!g_strcmp0 (one_test, "Accessible")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_accessible ();
      test_result = g_test_run ();
      return (test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Action")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_action ();
      test_result = g_test_run ();
      return (test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Component")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_component ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Collection")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_collection ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Document")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_document ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Editable_Text")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_editable_text ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Hyperlink")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_hyperlink ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Hypertext")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_hypertext ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Image")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_image ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Selection")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_selection ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "State_Set")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_state_set ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Table")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_table ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Table_Cell")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_table_cell ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Text")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_text ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    if (!g_strcmp0 (one_test, "Value")) {
      g_test_init (&argc, &argv, NULL);
      atk_test_value ();
      test_result = g_test_run ();
      return ( test_result == 0 ) ? 0 : 255;
    }
    g_print ("Unknown test name\n");
    _list_tests ();
    return EXIT_SUCCESS;
  }
  atk_suite_build (argc, argv);
  test_result = g_test_run ();

  return (test_result == 0 ) ? 0 : 255;

}
