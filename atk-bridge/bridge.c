/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <libbonobo.h>
#include <orbit/orbit.h>
#include <atk/atk.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>
#include "accessible.h"
#include "application.h"

typedef struct _ArgStruct ArgStruct;

struct _ArgStruct {
  gint c;
  char **v;
};

static CORBA_Environment ev;
static Accessibility_Registry registry;

static gboolean bridge_register_app (gpointer p);
static void bridge_focus_tracker (AtkObject *object);

int
gtk_module_init(gint argc, char* argv[])
{
  ArgStruct *args = (ArgStruct *) g_new0(ArgStruct, 1);
  args->c = argc;
  args->v = argv;
  g_idle_add (bridge_register_app, args);
}

static gboolean
bridge_register_app (gpointer gp)
{
  CORBA_Object oclient;
  AtkObject *atko;
  char *obj_id;
  ArgStruct *args = (ArgStruct *)gp;

  Application *app;

  CORBA_exception_init(&ev);

  if (!bonobo_init (&(args->c), args->v))
    {
      g_error ("Could not initialize Bonobo");
    }

  /* Create the accesssible application server object */
  app = application_new(atk_get_root ());

  obj_id = "OAFIID:Accessibility_Registry:proto0.1";

  oclient = bonobo_activation_activate_from_id (obj_id, 0, NULL, &ev);
  if (ev._major != CORBA_NO_EXCEPTION) {
    fprintf(stderr,
            ("Accessibility app error: exception during registry activation from id: %s\n"),
            CORBA_exception_id(&ev));
    CORBA_exception_free(&ev);
    exit(-1);
  }

  if (CORBA_Object_is_nil (oclient, &ev))
    {
      g_error ("Could not locate registry");
    }

  registry = (Accessibility_Registry) oclient;

  fprintf(stderr, "About to register application\n");

  bonobo_activate ();

  /* Register for focus event notifications, and register app with central registry  */
  atk_add_focus_tracker (bridge_focus_tracker);

  Accessibility_Registry_registerApplication (registry,
                                              bonobo_object_corba_objref (bonobo_object (app)),
                                              &ev);
  return FALSE;
}

static void bridge_focus_tracker (AtkObject *object)
{
  Accessibility_Event *e = g_new0(Accessibility_Event, 1);
  e->type = CORBA_string_dup ("focus:");
  e->target = bonobo_object_corba_objref (bonobo_object (accessible_new (object)));
  e->detail1 = 0;
  e->detail2 = 0;
  Accessibility_Registry_notifyEvent (registry, e, &ev);
}

static Accessibility_Registry bridge_get_registry ()
{
  return registry;
}
