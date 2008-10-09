/*
 * Copyright (C) 2008 Codethink Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <glib-object.h>
#include <string.h>
#include <stdio.h>

#include "classy-test.h"
#include "classy-test-suite.h"

/*---------------------------------------------------------------------------*/

static gboolean
run_test_app(gchar *module_name, gchar *dbus_name)
{
  gboolean result;
  GPid pid;
  GError *err = NULL;

  const gchar *test_data_directory;
  const gchar *test_modules_directory;
  const gchar *test_atspi_library;
  const gchar *test_application;
  gchar *test_module;
  gchar *command_line;

  test_data_directory = g_getenv("TEST_DATA_DIRECTORY");
  test_modules_directory = g_getenv("TEST_MODULES_DIRECTORY");
  test_atspi_library = g_getenv("TEST_ATSPI_LIBRARY");
  test_application = g_getenv("TEST_APPLICATION");

  test_module = g_build_path("/", test_modules_directory, module_name, NULL); 

  command_line = g_build_path(" ", test_application,
                                   "--atspi-dbus-name", dbus_name,
				   "--test-atspi-library", test_atspi_library,
				   "--test-module", test_module,
				   "--test-data-directory", test_data_directory,
				   NULL); 

  if (!(result = g_spawn_command_line_async(NULL, &err)))
    {
      fprintf(stderr, "\nCould not spawn test application - %s", err->message);
    }

  g_free(test_module);
  g_free(command_line);

  return result;
}

/*---------------------------------------------------------------------------*/

static gboolean
suite_idle_handler(gpointer data);

static void
suite_finished_handler(gpointer data)
{
	ClassyTestSuite *suite = CLASSY_TEST_SUITE(data);
	ClassyTest *test = CLASSY_TEST(g_array_index(suite->cases, gpointer, suite->current));

	if ((classy_test_state(test) == CLASSY_TEST_WIN) || suite->cont) {
		g_idle_add(suite_idle_handler, suite);
	}
	/* TODO If test has failed remember to send signal saying so */
}

static gboolean
suite_idle_handler(gpointer data)
{
	ClassyTestSuite *suite = CLASSY_TEST_SUITE(data);

	suite->current++;
	if (suite->current >= suite->cases->len) {
		/* No more tests, check for success or fail */
		gboolean succeeded = TRUE;
		gint i;
		for (i=0; i < suite->cases->len; i++) {
			ClassyTest *test;
			test = CLASSY_TEST(g_array_index(suite->cases, gpointer, i));
			succeeded = succeeded && (classy_test_state(test) == CLASSY_TEST_WIN);
		}
		if (succeeded == TRUE)
			classy_test_pass(CLASSY_TEST(suite));
		else
			classy_test_fail(CLASSY_TEST(suite), "Sub-test has failed");
	} else {
		/* More tests, run this one*/
		ClassyTest *test;
		test = CLASSY_TEST(g_array_index(suite->cases, gpointer, suite->current));
		g_signal_connect(test, "finished", (GCallback) suite_finished_handler, data);
		classy_test_run(test);
	}
	return FALSE;
}

static void
suite_run(ClassyTest *test)
{
	g_idle_add(suite_idle_handler, test);
}

/*---------------------------------------------------------------------------*/

static gchar *
suite_report(ClassyTest *test)
{
	ClassyTestSuite *suite = CLASSY_TEST_SUITE(test);
	GString *report;
	gint i;

	report = g_string_new("");

	switch (classy_test_state(test)) {
		case CLASSY_TEST_FAIL:
			g_string_printf(report, "FAIL       : %s\n           : %s\n ", test->name);
		case CLASSY_TEST_WIN:
			g_string_printf(report, "PASS       : %s\n", test->name);
		default:
			g_string_printf(report, "INCOMPLETE : %s\n", test->name);
	}

	for (i=0; i < suite->cases->len; i++) {
		ClassyTest *subtest = CLASSY_TEST(g_array_index(suite->cases, gpointer, i));
		g_string_append_printf(report, "             ");
		g_string_append_printf(report, "%s", classy_test_report(subtest));
	}
	return g_string_free(report, FALSE);
}

/*---------------------------------------------------------------------------*/

G_DEFINE_TYPE (ClassyTestSuite, classy_test_suite, TYPE_CLASSY_TEST)

static void
classy_test_suite_finalize (GObject *obj)
{
	ClassyTestSuite *suite = CLASSY_TEST_SUITE(obj);
	gint i;

	for (i=0; i < suite->cases->len; i++) {
		ClassyTest *test;
		test = CLASSY_TEST(g_array_index(suite->cases, gpointer, i));
		g_object_unref(test);
	}
	g_free(suite->data);

	G_OBJECT_CLASS (classy_test_suite_parent_class)->finalize (obj);
}

static void 
classy_test_suite_class_init (ClassyTestSuiteClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	ClassyTestClass *test_class = CLASSY_TEST_CLASS(klass);

	gobject_class->finalize = classy_test_suite_finalize;

	test_class->report = suite_report;
	test_class->run = suite_run;
}

static void
classy_test_suite_init (ClassyTestSuite *suite)
{
	suite->cases = g_array_new(FALSE, FALSE, sizeof(gpointer));	
	suite->current = -1;
}

/*---------------------------------------------------------------------------*/

ClassyTestSuite *
classy_test_suite_new(gchar *name, gint dsize, gboolean cont)
{
	ClassyTestSuite *suite;
	ClassyTest *test;

	suite = g_object_new(TYPE_CLASSY_TEST_SUITE, NULL);
	suite->cont = cont;
	suite->data = g_malloc0(dsize);

	test = CLASSY_TEST(suite);
	test->name = g_strdup(name);
	
	return suite;
}

void
classy_test_suite_add(ClassyTestSuite *suite, ClassyTest *test)
{
	g_array_append(suite->cases, test);
}

/*---------------------------------------------------------------------------*/
