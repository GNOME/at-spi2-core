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

#include <signal.h>
#include "atk_test_util.h"

pid_t child_pid;

static void assert_clean_exit (int sig)
{
  kill (child_pid, SIGTERM);
}

void clean_exit_on_fail ()
{
  signal (SIGABRT, assert_clean_exit);
}

void
run_app (const char *file_name)
{
  child_pid = fork ();
  if (child_pid == 0) {
    execlp (TESTS_BUILD_DIR "/app-test",
            TESTS_BUILD_DIR "/app-test",
            "--test-data-file",
            file_name,
            NULL);
    _exit (EXIT_SUCCESS);
  }
  if (child_pid) fprintf(stderr, "child_pid %d\n", child_pid);
}

static AtspiAccessible *try_get_root_obj (AtspiAccessible *obj)
{
  int i;

  gint child_count = atspi_accessible_get_child_count (obj, NULL);
  if (child_count < 1) {
    return NULL;
  }

  for (i=0; i<child_count; i++) {
    AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,i, NULL);
    if (child && !strcmp (atspi_accessible_get_name (child, NULL), "root_object"))
      return child;
  }

  return NULL;
}

AtspiAccessible * get_root_obj (const char *file_name)
{
  int tries = 0;
  AtspiAccessible *child;
  struct timespec timeout = { .tv_sec = 0, .tv_nsec = 10 * 1000000 };
  AtspiAccessible *obj = NULL;

  fprintf(stderr, "run_app: %s\n", file_name);
  run_app (file_name);

  obj = atspi_get_desktop (0);

  /* Wait for application to start, up to 100 times 10ms.  */
  while (++tries <= 100)
  {
    child = try_get_root_obj (obj);
    if (child)
      return child;

    nanosleep(&timeout, NULL);
  }

  if (atspi_accessible_get_child_count (obj, NULL) < 1) {
    g_test_message ("Fail, test application not found\n");
  } else {
    g_test_message ("test object not found\n");
  }
  g_test_fail ();
  kill (child_pid, SIGTERM);
  return NULL;
}

void terminate_app (void)
{
  int tries = 0;

  AtspiAccessible *child;
  struct timespec timeout = { .tv_sec = 0, .tv_nsec = 10 * 1000000 };
  AtspiAccessible *obj = NULL;

  kill (child_pid, SIGTERM);

  obj = atspi_get_desktop (0);

  /* Wait for application to stop, up to 100 times 10ms.  */
  while (++tries <= 100)
  {
    child = try_get_root_obj (obj);
    if (child == NULL)
      return;

    nanosleep(&timeout, NULL);
  }

  g_test_message ("Fail, test application still running\n");
  g_test_fail ();
}
