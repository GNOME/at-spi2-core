/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include "atk_test_util.h"
#include <signal.h>

static AtspiEventListener *fixture_listener = NULL;
static TestAppFixture *current_fixture = NULL;

static pid_t
run_app (const char *file_name, const char *name_to_claim)
{
  pid_t child_pid = fork ();
  if (child_pid == 0)
    {
      execlp (TESTS_BUILD_DIR "/app-test",
              TESTS_BUILD_DIR "/app-test",
              "--test-data-file",
              file_name,
              "--atspi-dbus-name",
              name_to_claim,
              NULL);
      _exit (EXIT_SUCCESS);
    }

  return child_pid;
}

static AtspiAccessible *
try_get_root_obj (AtspiAccessible *obj)
{
  GError *error = NULL;
  gchar *name;
  int i;

  gint child_count = atspi_accessible_get_child_count (obj, &error);
  if (child_count < 0)
    {
      if (error)
        {
          g_print ("  get_child_count: %s\n", error->message);
          g_error_free (error);
        }
      else
        {
          g_print ("  get_child_count=%d with no error\n", child_count);
        }
      return NULL;
    }
  else if (child_count < 1)
    {
      g_print ("  child_count == %d, bailing out\n", child_count);
      return NULL;
    }

  for (i = child_count - 1; i >= 0; i--)
    {
      AtspiAccessible *child = atspi_accessible_get_child_at_index (obj, i, &error);
      if (!child)
        {
          if (error)
            {
              g_print ("  getting child_at_index: %s\n", error->message);
              g_error_free (error);
              error = NULL;
            }
          else
            {
              g_print ("  getting child_at_index returned NULL child with no error\n");
            }
          continue;
        }
      if ((name = atspi_accessible_get_name (child, &error)) != NULL)
        {
          if (!strcmp (name, "root_object"))
            {
              g_free (name);
              return child;
            }
          g_print ("  name=%s\n", name);
          g_free (name);
          if (error)
            {
              g_error_free (error);
              error = NULL;
            }
        }
      else
        {
          if (error)
            {
              g_print ("try_get_root_obj getting child name: %s\n", error->message);
              g_error_free (error);
              error = NULL;
            }
          else
            {
              g_print ("  get_name returned NULL name with no error\n");
            }
        }
      g_object_unref (child);
    }

  return NULL;
}

/* Callback from AtspiEventListener.  We monitor children-changed on the root, so we can know
 * when the helper test-application has launched and registered.
 */
static void
listener_event_cb (AtspiEvent *event, void *user_data)
{
  TestAppFixture *fixture = current_fixture;

  if (atspi_accessible_get_role (event->source, NULL) == ATSPI_ROLE_DESKTOP_FRAME && strstr (event->type, "add"))
    {
      AtspiAccessible *obj = atspi_get_desktop (0);

      fixture->root_obj = try_get_root_obj (obj);
      g_object_unref (obj);

      if (fixture->root_obj)
        {
          fixture->state = FIXTURE_STATE_CHILD_ACQUIRED;
          atspi_event_quit ();
        }
    }
  g_boxed_free (ATSPI_TYPE_EVENT, event);
}

/* Sets up the atspi event listener for the test-application helpers.
 *
 * We get notified when the test-application registers its root object by listening
 * to the children-changed signal.
 */
void
fixture_listener_init (void)
{
  GError *error = NULL;

  fixture_listener = atspi_event_listener_new (listener_event_cb, NULL, NULL);
  if (!atspi_event_listener_register (fixture_listener, "object:children-changed", &error))
    {
      g_error ("Could not register event listener for children-changed: %s\n", error->message);
    }
}

void
fixture_listener_destroy (void)
{
  GError *error = NULL;

  if (!atspi_event_listener_deregister (fixture_listener, "object:children-changed", &error))
    {
      g_error ("Could not deregister event listener: %s", error->message);
    }

  g_object_unref (fixture_listener);
  fixture_listener = NULL;
}

static gboolean
wait_for_test_app_timeout_cb (gpointer user_data)
{
  TestAppFixture *fixture = user_data;

  fixture->test_app_timed_out = TRUE;
  fixture->wait_for_test_app_timeout = 0;
  atspi_event_quit ();

  return FALSE;
}

/* Each of the helper programs with the test fixtures claims a different DBus name,
 * to make them non-ambiguous when they get restarted all the time.  This is the serial
 * number that gets appended to each name.
 */
static guint fixture_serial = 0;

void
do_abort (int sig)
{
  signal (sig, SIG_DFL);
  if (current_fixture)
    fixture_teardown (current_fixture, NULL);
}

void
fixture_setup (TestAppFixture *fixture, gconstpointer user_data)
{
  const char *file_name = user_data;

  fixture->state = FIXTURE_STATE_WAITING_FOR_CHILD;
  fixture->name_to_claim = g_strdup_printf ("org.a11y.Atspi2Atk.TestApplication_%u", fixture_serial);
  fixture_serial += 1;

  fixture->child_pid = run_app (file_name, fixture->name_to_claim);

  fixture->test_app_timed_out = FALSE;
  fixture->wait_for_test_app_timeout = g_timeout_add (2000, wait_for_test_app_timeout_cb, fixture); /* 2 sec */

  current_fixture = fixture;
  putenv ("ATSPI_IN_TESTS=1");
  signal (SIGABRT, do_abort);
  signal (SIGSEGV, do_abort);
  atspi_event_main ();

  if (fixture->wait_for_test_app_timeout)
    g_source_remove (fixture->wait_for_test_app_timeout);
  fixture->wait_for_test_app_timeout = 0;

  if (fixture->test_app_timed_out)
    {
      g_print ("test app timed out before registering its root object");
      g_test_fail ();
    }
}

void
fixture_teardown (TestAppFixture *fixture, gconstpointer user_data)
{
  current_fixture = NULL;

  kill (fixture->child_pid, SIGTERM);
  fixture->child_pid = -1;

  if (fixture->root_obj)
    {
      g_object_unref (fixture->root_obj);
      fixture->root_obj = NULL;
    }

  g_free (fixture->name_to_claim);
  fixture->name_to_claim = NULL;
}

void
check_name (AtspiAccessible *accessible, const char *expected_name)
{
  gchar *obj_name;

  g_assert_nonnull (accessible);
  obj_name = atspi_accessible_get_name (accessible, NULL);
  g_assert_cmpstr (expected_name, ==, obj_name);
  g_free (obj_name);
}
