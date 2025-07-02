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

#pragma once

#include "atk_suite.h"
#include <glib.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

typedef enum
{
  FIXTURE_STATE_WAITING_FOR_CHILD,
  FIXTURE_STATE_CHILD_ACQUIRED,
} FixtureState;

typedef struct
{
  FixtureState state;

  char *name_to_claim;

  guint wait_for_test_app_timeout;
  gboolean test_app_timed_out;
  pid_t child_pid;

  AtspiAccessible *root_obj;
} TestAppFixture;

extern pid_t child_pid;

void fixture_listener_init (void);
void fixture_listener_destroy (void);
void fixture_setup (TestAppFixture *fixture, gconstpointer user_data);
void fixture_teardown (TestAppFixture *fixture, gconstpointer user_data);

void check_name (AtspiAccessible *accessible, const char *expected_name);
