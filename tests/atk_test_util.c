/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <signal.h>
#include "atk_test_util.h"

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
    execlp ("./app-test",
            "./app-test",
            "--test-data-file",
            file_name,
            NULL);
    _exit (EXIT_SUCCESS);
  }
}

AtspiAccessible * get_root_obj (const char *file_name)
{
  int i;
  AtspiAccessible *obj = NULL;

  run_app (file_name);

  /* sleep is needed to wait for fored test application*/
  sleep (1);

  obj = atspi_get_desktop (0);
  gint child_count = atspi_accessible_get_child_count (obj, NULL);
  if (child_count < 1) {
    g_test_message ("Fail, test application not found\n");
    g_test_fail ();
    kill (child_pid, SIGTERM);
    return NULL;
  }

  for (i=0; i<child_count; i++) {
    AtspiAccessible *child = atspi_accessible_get_child_at_index (obj,i, NULL);
    if (!strcmp (atspi_accessible_get_name (child, NULL), "root_object"))
      return child;
  }
  g_test_message ("test object not found\n");
  g_test_fail ();

  kill (child_pid, SIGTERM);
  return NULL;
}
