/* -*- mode: c; c-basic-offset: 2; indent-tabs-mode: nil; -*-
 * 
 * at-spi-bus-launcher: Manage the a11y bus as a child process 
 *
 * Copyright 2011 Red Hat, Inc.
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

#include "config.h"

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#include <gio/gio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

typedef enum {
  A11Y_BUS_STATE_IDLE = 0,
  A11Y_BUS_STATE_READING_ADDRESS,
  A11Y_BUS_STATE_RUNNING,
  A11Y_BUS_STATE_ERROR
} A11yBusState;

typedef struct {
  GMainLoop *loop;
  gboolean launch_immediately;
  GDBusConnection *session_bus;

  A11yBusState state;
  /* -1 == error, 0 == pending, > 0 == running */
  int a11y_bus_pid;
  char *a11y_bus_address;
  int pipefd[2];
  char *a11y_launch_error_message;
} A11yBusLauncher;

static A11yBusLauncher *_global_app = NULL;

static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.a11y.Bus'>"
  "    <method name='GetAddress'>"
  "      <arg type='s' name='address' direction='out'/>"
  "    </method>"
  "  </interface>"
  "</node>";
static GDBusNodeInfo *introspection_data = NULL;

static void
setup_bus_child (gpointer data)
{
  A11yBusLauncher *app = data;
  (void) app;

  close (app->pipefd[0]);
  dup2 (app->pipefd[1], 3);
  close (app->pipefd[1]);

  /* On Linux, tell the bus process to exit if this process goes away */
#ifdef __linux
#include <sys/prctl.h>
  prctl (PR_SET_PDEATHSIG, 15);
#endif  
}

/**
 * unix_read_all_fd_to_string:
 *
 * Read all data from a file descriptor to a C string buffer.
 */
static gboolean
unix_read_all_fd_to_string (int      fd,
                            char    *buf,
                            ssize_t  max_bytes)
{
  ssize_t bytes_read;

  while (max_bytes > 1 && (bytes_read = read (fd, buf, MAX (4096, max_bytes - 1))))
    {
      if (bytes_read < 0)
        return FALSE;
      buf += bytes_read;
      max_bytes -= bytes_read;
    }
  *buf = '\0';
  return TRUE;
}

static void
on_bus_exited (GPid     pid,
               gint     status,
               gpointer data)
{
  A11yBusLauncher *app = data;
  
  app->a11y_bus_pid = -1;
  app->state = A11Y_BUS_STATE_ERROR;
  if (app->a11y_launch_error_message == NULL)
    {
      if (WIFEXITED (status))
        app->a11y_launch_error_message = g_strdup_printf ("Bus exited with code %d", WEXITSTATUS (status));
      else if (WIFSIGNALED (status))
        app->a11y_launch_error_message = g_strdup_printf ("Bus killed by signal %d", WTERMSIG (status));
      else if (WIFSTOPPED (status))
        app->a11y_launch_error_message = g_strdup_printf ("Bus stopped by signal %d", WSTOPSIG (status));
    }
  g_main_loop_quit (app->loop);
} 

static void
ensure_a11y_bus (A11yBusLauncher *app)
{
  GPid pid;
  char *argv[] = { DBUS_DAEMON, NULL, "--nofork", "--print-address", "3", NULL };
  char addr_buf[2048];
  GError *error = NULL;

  if (app->a11y_bus_pid != 0)
    return;
  
  argv[1] = g_strdup_printf ("--config-file=%s/at-spi2/accessibility.conf", SYSCONFDIR);

  if (pipe (app->pipefd) < 0)
    g_error ("Failed to create pipe: %s", strerror (errno));
  
  if (!g_spawn_async (NULL,
                      argv,
                      NULL,
                      G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD,
                      setup_bus_child,
                      app,
                      &pid,
                      &error))
    {
      app->a11y_bus_pid = -1;
      app->a11y_launch_error_message = g_strdup (error->message);
      g_clear_error (&error);
      goto error;
    }

  close (app->pipefd[1]);
  app->pipefd[1] = -1;

  g_child_watch_add (pid, on_bus_exited, app);

  app->state = A11Y_BUS_STATE_READING_ADDRESS;
  app->a11y_bus_pid = pid;
  g_debug ("Launched a11y bus, child is %ld", (long) pid);
  if (!unix_read_all_fd_to_string (app->pipefd[0], addr_buf, sizeof (addr_buf)))
    {
      app->a11y_launch_error_message = g_strdup_printf ("Failed to read address: %s", strerror (errno));
      kill (app->a11y_bus_pid, SIGTERM);
      goto error;
    }
  close (app->pipefd[0]);
  app->pipefd[0] = -1;
  app->state = A11Y_BUS_STATE_RUNNING;

  /* Trim the trailing newline */
  app->a11y_bus_address = g_strchomp (g_strdup (addr_buf));
  g_debug ("a11y bus address: %s", app->a11y_bus_address);

  {
    Display *display = XOpenDisplay (NULL);
    if (display)
      {
        Atom bus_address_atom = XInternAtom (display, "AT_SPI_BUS", False);
        XChangeProperty (display,
                         XDefaultRootWindow (display),
                         bus_address_atom,
                         XA_STRING, 8, PropModeReplace,
                         (guchar *) app->a11y_bus_address, strlen (app->a11y_bus_address));
      }
    XFlush (display);
    XCloseDisplay (display);
  }

  return;
  
 error:
  close (app->pipefd[0]);
  close (app->pipefd[1]);
  app->state = A11Y_BUS_STATE_ERROR;
}

static void
handle_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{
  A11yBusLauncher *app = user_data;

  if (g_strcmp0 (method_name, "GetAddress") == 0)
    {
      ensure_a11y_bus (app);
      if (app->a11y_bus_pid > 0)
        g_dbus_method_invocation_return_value (invocation,
                                               g_variant_new ("(s)", app->a11y_bus_address));
      else
        g_dbus_method_invocation_return_dbus_error (invocation,
                                                    "org.a11y.Bus.Error",
                                                    app->a11y_launch_error_message);
    }
}

static const GDBusInterfaceVTable interface_vtable =
{
  handle_method_call,
  NULL,
  NULL  /* handle_set_property */
};

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
  A11yBusLauncher *app = user_data;
  GError *error;
  guint registration_id;
  
  if (connection == NULL)
    {
      g_main_loop_quit (app->loop);
      return;
    }
  app->session_bus = connection;

  if (app->launch_immediately)
    {
      ensure_a11y_bus (app);
      if (app->state == A11Y_BUS_STATE_ERROR)
        {
          g_main_loop_quit (app->loop);
          return;
        }
    }

  error = NULL;
  registration_id = g_dbus_connection_register_object (connection,
                                                       "/org/a11y/bus",
                                                       introspection_data->interfaces[0],
                                                       &interface_vtable,
                                                       _global_app,
                                                       NULL,
                                                       &error);
  if (registration_id == 0)
    g_error ("%s", error->message);
}

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
  A11yBusLauncher *app = user_data;
  if (app->session_bus == NULL
      && connection == NULL
      && app->a11y_launch_error_message == NULL)
    app->a11y_launch_error_message = g_strdup ("Failed to connect to session bus");
  g_main_loop_quit (app->loop);
}

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
  A11yBusLauncher *app = user_data;
  (void) app;
}

static int sigterm_pipefd[2];

static void
sigterm_handler (int signum)
{
  write (sigterm_pipefd[1], "X", 1);
}

static gboolean
on_sigterm_pipe (GIOChannel  *channel,
                 GIOCondition condition,
                 gpointer     data)
{
  A11yBusLauncher *app = data;
  
  g_main_loop_quit (app->loop);

  return FALSE;
}

static void
init_sigterm_handling (A11yBusLauncher *app)
{
  GIOChannel *sigterm_channel;

  if (pipe (sigterm_pipefd) < 0)
    g_error ("Failed to create pipe: %s", strerror (errno));
  signal (SIGTERM, sigterm_handler);

  sigterm_channel = g_io_channel_unix_new (sigterm_pipefd[0]);
  g_io_add_watch (sigterm_channel,
                  G_IO_IN | G_IO_ERR | G_IO_HUP,
                  on_sigterm_pipe,
                  app);
}

static gboolean
is_a11y_using_corba (void)
{
  char *gconf_argv[] = { "gconftool-2", "--get", "/desktop/gnome/interface/at-spi-corba", NULL };
  char *stdout = NULL;
  int estatus;
  gboolean result = FALSE;

  if (!g_spawn_sync (NULL, gconf_argv, NULL,
                     G_SPAWN_SEARCH_PATH, NULL, NULL, &stdout, NULL, &estatus, NULL))
    goto out;
  if (estatus != 0)
    goto out;
  if (g_str_has_prefix (stdout, "true"))
    result = TRUE;
 out:
  g_free (stdout);
  return result;
}

int
main (int    argc,
      char **argv)
{
  GError *error = NULL;
  GMainLoop *loop;
  GDBusConnection *session_bus;
  int name_owner_id;

  g_type_init ();

  if (is_a11y_using_corba ())
    return 0;

  _global_app = g_slice_new0 (A11yBusLauncher);
  _global_app->loop = g_main_loop_new (NULL, FALSE);
  _global_app->launch_immediately = (argc == 2 && strcmp (argv[1], "--launch-immediately") == 0);

  init_sigterm_handling (_global_app);

  introspection_data = g_dbus_node_info_new_for_xml (introspection_xml, NULL);
  g_assert (introspection_data != NULL);

  name_owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                                  "org.a11y.Bus",
                                  G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT,
                                  on_bus_acquired,
                                  on_name_acquired,
                                  on_name_lost,
                                  _global_app,
                                  NULL);

  g_main_loop_run (_global_app->loop);

  if (_global_app->a11y_bus_pid > 0)
    kill (_global_app->a11y_bus_pid, SIGTERM);

  /* Clear the X property if our bus is gone; in the case where e.g. 
   * GDM is launching a login on an X server it was using before,
   * we don't want early login processes to pick up the stale address.
   */
  {
    Display *display = XOpenDisplay (NULL);
    if (display)
      {
        Atom bus_address_atom = XInternAtom (display, "AT_SPI_BUS", False);
        XDeleteProperty (display,
                         XDefaultRootWindow (display),
                         bus_address_atom);

        XFlush (display);
        XCloseDisplay (display);
      }
  }

  if (_global_app->a11y_launch_error_message)
    {
      g_printerr ("Failed to launch bus: %s", _global_app->a11y_launch_error_message);
      return 1;
    }
  return 0;
}
