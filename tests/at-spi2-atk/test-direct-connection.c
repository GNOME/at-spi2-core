/*
 * AT-SPI - Assistive Technology Service Provider Interface
 *
 * Copyright 2026 Azamat Almazbek uulu
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <gio/gio.h>
#include <glib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#define TEST_APP_NAME "org.a11y.atspi.test.DirectConnection"
#define ATSPI_ROOT_PATH "/org/a11y/atspi/accessible/root"
#define ATSPI_APPLICATION_INTERFACE "org.a11y.atspi.Application"

static guint
count_pidfds (GPid pid)
{
  g_autofree gchar *directory_path = NULL;
  g_autoptr (GDir) directory = NULL;
  const gchar *entry;
  guint count = 0;

  directory_path = g_strdup_printf ("/proc/%d/fd", pid);
  directory = g_dir_open (directory_path, 0, NULL);
  if (!directory)
    return 0;

  while ((entry = g_dir_read_name (directory)))
    {
      g_autofree gchar *fd_path = g_build_filename (directory_path, entry, NULL);
      g_autofree gchar *target = g_file_read_link (fd_path, NULL);

      if (g_strcmp0 (target, "anon_inode:[pidfd]") == 0)
        count++;
    }

  return count;
}

static gboolean
wait_for_pidfd_count (GPid pid, guint expected)
{
  gint64 deadline = g_get_monotonic_time () + (2 * G_TIME_SPAN_SECOND);

  do
    {
      if (count_pidfds (pid) == expected)
        return TRUE;
      g_usleep (10 * 1000);
    }
  while (g_get_monotonic_time () < deadline);

  return FALSE;
}

static gboolean
wait_for_pidfd_increase (GPid pid, guint baseline)
{
  gint64 deadline = g_get_monotonic_time () + (2 * G_TIME_SPAN_SECOND);

  do
    {
      if (count_pidfds (pid) > baseline)
        return TRUE;
      g_usleep (10 * 1000);
    }
  while (g_get_monotonic_time () < deadline);

  return FALSE;
}

static gchar *
get_accessibility_bus_address (void)
{
  g_autoptr (GDBusConnection) session_bus = NULL;
  g_autoptr (GVariant) reply = NULL;
  g_autoptr (GError) error = NULL;
  const gchar *address;

  session_bus = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, &error);
  if (!session_bus)
    g_error ("Failed to connect to session bus: %s", error->message);

  reply = g_dbus_connection_call_sync (session_bus,
                                       "org.a11y.Bus",
                                       "/org/a11y/bus",
                                       "org.a11y.Bus",
                                       "GetAddress",
                                       NULL,
                                       G_VARIANT_TYPE ("(s)"),
                                       G_DBUS_CALL_FLAGS_NONE,
                                       -1,
                                       NULL,
                                       &error);
  if (!reply)
    g_error ("Failed to get accessibility bus address: %s", error->message);

  g_variant_get (reply, "(&s)", &address);
  return g_strdup (address);
}

static gchar *
wait_for_application_address (GDBusConnection *accessibility_bus)
{
  guint attempt;

  for (attempt = 0; attempt < 100; attempt++)
    {
      g_autoptr (GVariant) reply = NULL;
      g_autoptr (GError) error = NULL;
      const gchar *address;

      reply = g_dbus_connection_call_sync (accessibility_bus,
                                           TEST_APP_NAME,
                                           ATSPI_ROOT_PATH,
                                           ATSPI_APPLICATION_INTERFACE,
                                           "GetApplicationBusAddress",
                                           NULL,
                                           G_VARIANT_TYPE ("(s)"),
                                           G_DBUS_CALL_FLAGS_NONE,
                                           100,
                                           NULL,
                                           &error);
      if (reply)
        {
          g_variant_get (reply, "(&s)", &address);
          return g_strdup (address);
        }

      g_usleep (10 * 1000);
    }

  return NULL;
}

static GDBusConnection *
open_direct_connection (const gchar *address, GError **error)
{
  return g_dbus_connection_new_for_address_sync (
      address,
      G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
      NULL,
      NULL,
      error);
}

static gboolean
query_direct_connection (GDBusConnection *connection, GError **error)
{
  g_autoptr (GVariant) reply = NULL;

  reply = g_dbus_connection_call_sync (connection,
                                       NULL,
                                       ATSPI_ROOT_PATH,
                                       ATSPI_APPLICATION_INTERFACE,
                                       "GetApplicationBusAddress",
                                       NULL,
                                       G_VARIANT_TYPE ("(s)"),
                                       G_DBUS_CALL_FLAGS_NONE,
                                       -1,
                                       NULL,
                                       error);
  return reply != NULL;
}

static void
test_direct_connection_is_released (void)
{
  g_autofree gchar *accessibility_bus_address = NULL;
  g_autofree gchar *application_address = NULL;
  g_autoptr (GDBusConnection) accessibility_bus = NULL;
  g_autoptr (GDBusConnection) direct_connection = NULL;
  g_autoptr (GError) error = NULL;
  gchar *app_argv[] = {
    TESTS_BUILD_DIR "/app-test",
    "--test-data-file",
    TESTS_DATA_DIR "/test.xml",
    "--atspi-dbus-name",
    TEST_APP_NAME,
    NULL,
  };
  GPid app_pid = 0;
  guint baseline;
  guint iteration;

  accessibility_bus_address = get_accessibility_bus_address ();
  accessibility_bus = g_dbus_connection_new_for_address_sync (
      accessibility_bus_address,
      G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT |
          G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION,
      NULL,
      NULL,
      &error);
  if (!accessibility_bus)
    g_error ("Failed to connect to accessibility bus: %s", error->message);

  if (!g_spawn_async (NULL,
                      app_argv,
                      NULL,
                      G_SPAWN_DO_NOT_REAP_CHILD,
                      NULL,
                      NULL,
                      &app_pid,
                      &error))
    g_error ("Failed to start test application: %s", error->message);

  application_address = wait_for_application_address (accessibility_bus);
  if (!application_address)
    {
      g_test_fail_printf ("Test application did not publish its direct bus address");
      goto out;
    }

  baseline = count_pidfds (app_pid);
  direct_connection = open_direct_connection (application_address, &error);
  if (!direct_connection)
    {
      g_test_fail_printf ("Failed to open direct connection: %s", error->message);
      goto out;
    }

  if (!query_direct_connection (direct_connection, &error))
    {
      g_test_fail_printf ("Failed to query direct connection: %s", error->message);
      goto out;
    }

  if (!wait_for_pidfd_increase (app_pid, baseline))
    {
      g_test_skip ("libdbus does not retain peer credentials in a pidfd");
      goto out;
    }

  if (!g_dbus_connection_close_sync (direct_connection, NULL, &error))
    {
      g_test_fail_printf ("Failed to close direct connection: %s", error->message);
      goto out;
    }
  g_clear_object (&direct_connection);

  if (!wait_for_pidfd_count (app_pid, baseline))
    {
      g_test_fail_printf ("Direct connection pidfd was not released");
      goto out;
    }

  for (iteration = 0; iteration < 10; iteration++)
    {
      direct_connection = open_direct_connection (application_address, &error);
      if (!direct_connection)
        {
          g_test_fail_printf ("Failed to open direct connection: %s", error->message);
          goto out;
        }

      if (!query_direct_connection (direct_connection, &error))
        {
          g_test_fail_printf ("Failed to query direct connection: %s",
                              error->message);
          goto out;
        }

      if (!g_dbus_connection_close_sync (direct_connection, NULL, &error))
        {
          g_test_fail_printf ("Failed to close direct connection: %s", error->message);
          goto out;
        }
      g_clear_object (&direct_connection);
    }

  if (!wait_for_pidfd_count (app_pid, baseline))
    g_test_fail_printf ("Direct connection pidfds accumulated after reconnecting");

out:
  if (direct_connection)
    g_dbus_connection_close_sync (direct_connection, NULL, NULL);
  kill (app_pid, SIGTERM);
  waitpid (app_pid, NULL, 0);
  g_spawn_close_pid (app_pid);
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/atk-bridge/direct-connection-is-released",
                   test_direct_connection_is_released);
  return g_test_run ();
}
