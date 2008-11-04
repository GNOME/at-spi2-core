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

#include <stdlib.h>
#include <glib.h>
#include <gmodule.h>
#include <atk/atk.h>
#include <dbus/dbus.h>

/* The test module, GModule containing interface for an atk-test */
static GModule *test_module;
static gpointer test_module_get_root;
static gpointer test_module_next;
static gpointer test_module_finished;

static DBusConnection *dbus_bus;
static GMainLoop *mainloop;

/* Test module interface */
/*************************/

typedef AtkObject *(*TestModuleGetRoot) (void);

/* Calls into the test module to get the root atk object */
static AtkObject *
get_root(void)
{
  return ((TestModuleGetRoot) test_module_get_root)();
}

typedef void (*VoidVoid) (void);

/* Called to move to next stage of test.*/
static void
next(void)
{
  ((VoidVoid) test_module_next)();
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

typedef void (*GtkModuleInit) (int *argc, char **argv[]);

/* AT-SPI is a gtk module that must be loaded and initialized */
static void
load_atspi_module(const char *path, int *argc, char **argv[])
{
  GModule *bridge;
  gpointer init;

  bridge = g_module_open(path, G_MODULE_BIND_LOCAL|G_MODULE_BIND_LAZY);
  if (!bridge)
    g_error("Couldn't load atk-bridge module : %s\n", g_module_error());

  if (!g_module_symbol(bridge, "gtk_module_init", &init))
    g_error("Couldn't load symbol \"gtk_module_init\"\n");

  ((GtkModuleInit) init)(argc, argv);
}

typedef void (*TestModuleInit) (gchar *path);

static void
load_test_module(const char *path, const char *tdpath)
{
  gpointer init;

  test_module = g_module_open(path, G_MODULE_BIND_LOCAL|G_MODULE_BIND_LAZY);
  if (!test_module)
    g_error("Couldn't load test module : %s\n", path);

  if (!g_module_symbol(test_module, "test_init", &init))
    g_error("Couldn't load symbol \"test_init\"\n");

  if (!g_module_symbol(test_module, "test_get_root", &test_module_get_root))
    g_error("Couldn't load symbol \"test_get_root\"\n");

  if (!g_module_symbol(test_module, "test_next", &test_module_next))
    g_error("Couldn't load symbol \"test_next\"\n");

  if (!g_module_symbol(test_module, "test_finished", &test_module_finished))
    g_error("Couldn't load symbol \"test_finished\"\n");

  ((TestModuleInit) init)((gchar *)tdpath);
}

static const char* introspection_string = 
"<node name=\"/org/codethink/atspi/test\">"
"	<interface name=\"org.codethink.atspi.test\">"
"		<method name=\"next\"/>"
"		<method name=\"finish\"/>"
"               <signal name=\"started\"/>"
"	</interface>"
"</node>";

static DBusHandlerResult
message_handler (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  const char *iface = dbus_message_get_interface (message);
  const char *member = dbus_message_get_member (message);
  DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
  gboolean exit = FALSE;

  DBusMessage *reply = NULL;

  g_return_val_if_fail(iface != NULL, DBUS_HANDLER_RESULT_NOT_YET_HANDLED);
  
  if (!strcmp(iface, "org.codethink.atspi.test"))
    {
      if (!strcmp(member, "next"))
	{
	  next();
          reply = dbus_message_new_method_return (message);
	  g_assert(reply != NULL);
	  result = DBUS_HANDLER_RESULT_HANDLED;
	}

      if (!strcmp(member, "finish"))
	{
	  ((VoidVoid) test_module_finished)();
          reply = dbus_message_new_method_return (message);
	  g_assert(reply != NULL);
	  result = DBUS_HANDLER_RESULT_HANDLED;
	  exit = TRUE;
	}
    }

  if (!strcmp(iface, "org.freedesktop.DBus.Introspectable"))
    {
      if (!strcmp(member, "Introspect"))
	{
	  reply = dbus_message_new_method_return (message);
	  g_assert(reply != NULL);
	  dbus_message_append_args(reply, DBUS_TYPE_STRING, &introspection_string,
				   DBUS_TYPE_INVALID);
	  result = DBUS_HANDLER_RESULT_HANDLED;
	}
    }

  if (reply)
    {
      dbus_connection_send (bus, reply, NULL);
      dbus_message_unref (reply);
    }

  if (exit == TRUE)
    {
      dbus_connection_flush(bus);
      dbus_connection_unref(bus);
      g_main_loop_quit(mainloop);
      abort();
    }
  return result;
}

static DBusObjectPathVTable test_vtable =
{
  NULL,
  &message_handler,
  NULL, NULL, NULL, NULL
};

static void
init_dbus_interface(void)
{
  DBusError error;

  dbus_error_init(&error);
  dbus_bus = dbus_bus_get(DBUS_BUS_SESSION, &error);
  g_print("\nUnique D-Bus name is: %s\n", dbus_bus_get_unique_name(dbus_bus));

  if (!dbus_bus)
    g_error("Couldn't get the session bus - %s\n", error.message);

  g_assert(dbus_connection_register_object_path(dbus_bus,
						"/org/codethink/atspi/test",
					       	&test_vtable,
						NULL));

  dbus_connection_setup_with_g_main(dbus_bus, g_main_context_default());
}

static void
send_started_signal(void)
{
  DBusMessage* sig;
  DBusMessageIter args;

  sig = dbus_message_new_signal("/org/codethink/atspi/test", "org.codethink.atspi.test", "started");
  g_assert(sig != NULL);
  if (!dbus_connection_send(dbus_bus, sig, NULL))
    g_error("Out of memory");
  dbus_connection_flush(dbus_bus);
  dbus_message_unref(sig);
}

/*Command line data*/
static gchar *tmodule_path = NULL;
static gchar *amodule_path = NULL;
static gchar *tdata_path = NULL;

static GOptionEntry optentries[] = 
{
  {"test-module", 0, 0, G_OPTION_ARG_STRING, &tmodule_path, "Module containing test scenario", NULL},
  {"test-atspi-library", 0, 0, G_OPTION_ARG_STRING, &amodule_path, "Gtk module with atk-atspi adaptor", NULL},
  {"test-data-directory", 0, 0, G_OPTION_ARG_STRING, &tdata_path, "Path to directory of test data", NULL},
  {NULL}
};

/* main
 * 
 * Entry point for all test applications.
 */
main(int argc, char *argv[])
{
  GOptionContext *opt;
  GError *err = NULL;

  /*Parse command options*/
  opt = g_option_context_new(NULL);
  g_option_context_add_main_entries(opt, optentries, NULL);
  g_option_context_set_ignore_unknown_options(opt, TRUE);
  
  if (!g_option_context_parse(opt, &argc, &argv, &err))
      g_error("Option parsing failed: %s\n", err->message);

  if (tmodule_path == NULL)
      g_error("No test module provided");
  if (amodule_path == NULL)
      g_error("No atspi module provided");

  g_type_init();

  setup_atk_util();
  load_test_module(tmodule_path, tdata_path);
  load_atspi_module(amodule_path, &argc, &argv);
  init_dbus_interface();
  send_started_signal();

  mainloop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (mainloop);

  return 0;
}
