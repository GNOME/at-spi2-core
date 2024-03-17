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

/*
 * Testing AT-SPI requires both a test application and AT client.
 * Test applications are built using the Dummy ATK implementation: MyAtk.
 * This file contains the entry point for all test applications.
 * The test will provide its own implementation of atk_get_root,
 * and as such provide all the application state for the test.
 */

#include "atk-object-xml-loader.h"
#include "my-atk.h"
#include <atk-bridge.h>
#include <atk/atk.h>
#include <glib-unix.h>
#include <glib.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static AtkObject *root_accessible;

static GMainLoop *mainloop;

static gchar *tdata_path = NULL;

void
test_init (gchar *path)
{
  gchar *td;

  if (path == NULL)
    {
      g_print ("No test data file provided\n");
      exit (EXIT_FAILURE);
    }
  tdata_path = path;

  td = g_build_path (G_DIR_SEPARATOR_S, tdata_path, NULL, NULL);
  root_accessible = ATK_OBJECT (atk_object_xml_parse (td));
  g_free (td);
}

AtkObject *
test_get_root (void)
{
  return root_accessible;
}

static AtkObject *
get_root (void)
{
  return test_get_root ();
}

const gchar *
get_toolkit_name (void)
{
  return strdup ("atspitesting-toolkit");
}

static void
setup_atk_util (void)
{
  AtkUtilClass *klass;

  klass = g_type_class_ref (ATK_TYPE_UTIL);
  klass->get_root = get_root;
  klass->get_toolkit_name = get_toolkit_name;
  g_type_class_unref (klass);
}

static GOptionEntry optentries[] = {
  { "test-data-file", 0, 0, G_OPTION_ARG_STRING, &tdata_path, "Path to file of test data", NULL },
  { NULL }
};

static gboolean
sigterm_received_cb (gpointer user_data)
{
  GMainLoop *mainloop = user_data;
  g_print ("test application received SIGTERM\n");
  g_main_loop_quit (mainloop);
  return G_SOURCE_REMOVE;
}

int
main (int argc, char *argv[])
{
  GOptionContext *opt;
  GError *err = NULL;
  opt = g_option_context_new (NULL);
  g_option_context_add_main_entries (opt, optentries, NULL);
  g_option_context_set_ignore_unknown_options (opt, TRUE);

  if (!g_option_context_parse (opt, &argc, &argv, &err))
    g_error ("Option parsing failed: %s\n", err->message);

  setlocale (LC_ALL, "");
  setup_atk_util ();
  test_init (tdata_path);

  atk_bridge_adaptor_init (&argc, &argv);

  mainloop = g_main_loop_new (NULL, FALSE);
  g_unix_signal_add (SIGTERM, sigterm_received_cb, mainloop);
  g_main_loop_run (mainloop);

  g_print ("test application exited main loop; terminating after cleanup\n");

  atk_bridge_adaptor_cleanup ();

  g_print ("test application %d exiting!\n", getpid ());
  return 0;
}
