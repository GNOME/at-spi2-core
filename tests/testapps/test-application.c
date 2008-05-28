/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Codethink Ltd.
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

/*
 * Testing AT-SPI requires both a test application and AT client. 
 * Test applications are built using the Dummy ATK implementation: MyAtk.
 * This file contains the entry point for all test applications.
 * Each test is built as a GModule, and this program loads the 
 * test module, as well as the AT-SPI module. The test module will
 * provide its own implementation of atk_get_root, and as such provide
 * all the application state for the test.
 */

#include <glib.h>
#include <gmodule.h>
#include <atk/atk.h>
#include <dbus/dbus.h>

/* The test module, GModule containing interface for an atk-test */
static GModule *test_module;
static gpointer test_module_get_root;

/* Test module interface */
/*************************/

typedef AtkObject *(*TestModuleGetRoot) (void);

/* Calls into the test module to get the root atk object */
static AtkObject *
get_root(void)
{
  return ((TestModuleGetRoot) test_module_get_root)();
}

/*************************/

/* The AtkUtil class is called to find the root accessible and to deal
 * with events. Its an incomplete class, its v-table needs to be filled in.
 */
static void
setup_atk_util(void)
{
  AtkUtilClass *klass;

  klass = g_type_class_ref(ATK_TYPE_UTIL);
  klass->get_root = get_root;
  g_type_class_unref(klass);
}

typedef void (*GtkModuleInit) (int argc, char *argv[]);

/* AT-SPI is a gtk module that must be loaded and initialized */
static void
load_atspi_module(const char *path, int argc, char *argv[])
{
  GModule *bridge;
  gpointer init;

  bridge = g_module_open(path, G_MODULE_BIND_LOCAL|G_MODULE_BIND_LAZY);
  if (!bridge)
    g_error("Couldn't load atk-bridge module : %s\n", path);

  if (!g_module_symbol(bridge, "gtk_module_init", &init))
    g_error("Couldn't load symbol \"gtk_module_init\"\n");

  ((GtkModuleInit) init)(argc, argv);
}

static void
load_test_module(const char *path, int argc, char *argv[])
{
  gpointer init;

  test_module = g_module_open(path, G_MODULE_BIND_LOCAL|G_MODULE_BIND_LAZY);
  if (!test_module)
    g_error("Couldn't load test module : %s\n", path);

  if (!g_module_symbol(test_module, "test_init", &init))
    g_error("Couldn't load symbol \"test_init\"\n");

  if (!g_module_symbol(test_module, "test_get_root", &test_module_get_root))
    g_error("Couldn't load symbol \"test_get_root\"\n");

  ((GtkModuleInit) init)(argc, argv);
}

/*Command line data*/
static gchar *tmodule_path;
static gchar *amodule_path;

static GOptionEntry optentries[] = 
{
  {"test-module", 't', 0, G_OPTION_ARG_STRING, &tmodule_path, "Module containing test scenario", NULL},
  {"atspi-module", 'a', 0, G_OPTION_ARG_STRING, &amodule_path, "Gtk module with atk-atspi adaptor", NULL},
  {NULL}
};

/* main
 * 
 * Entry point for all test applications.
 */
main(int argc, char *argv[])
{
  GMainLoop *mainloop;
  GOptionContext *opt;
  GError *err = NULL;

  /*Parse command options*/
  opt = g_option_context_new(NULL);
  g_option_context_add_main_entries(opt, optentries, NULL);
  if (!g_option_context_parse(opt, &argc, &argv, &err))
    {
      g_print("Option parsing failed: %s\n", err->message);
      exit(1);
    }

  g_type_init();

  setup_atk_util();
  load_test_module(tmodule_path, argc, argv);
  load_atspi_module(amodule_path, argc, argv);

  mainloop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (mainloop);

  return 0;
}
