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

static void
atk_suite_build (void)
{
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

int
main(int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

  clean_exit_on_fail ();

  atk_suite_build ();

  return g_test_run ();
}
