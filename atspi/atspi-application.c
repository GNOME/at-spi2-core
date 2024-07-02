/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include "atspi-private.h"

/**
 * AtspiApplication:
 *
 * An interface identifying the root object associated
 * with a running application.
 *
 * An interface identifying an object which is the root of the
 * hierarchy associated with a running application.
 */

G_DEFINE_TYPE (AtspiApplication, atspi_application, G_TYPE_OBJECT)

static void
atspi_application_init (AtspiApplication *application)
{
}

static void
ref_object (void *obj, void *user_data)
{
  g_object_ref (obj);
}

static void
unref_object (void *obj, void *user_data)
{
  g_object_unref (obj);
}

static void
dispose_object (void *obj, void *user_data)
{
  g_object_run_dispose (obj);
}

static void
atspi_application_dispose (GObject *object)
{
  AtspiApplication *application = ATSPI_APPLICATION (object);

  if (application->bus)
    {
      if (application->bus != _atspi_bus ())
        dbus_connection_close (application->bus);
      dbus_connection_unref (application->bus);
      application->bus = NULL;
    }

  if (application->hash)
    {
      GList *list = g_hash_table_get_values (application->hash);
      g_list_foreach (list, ref_object, NULL);
      g_list_foreach (list, dispose_object, NULL);
      g_list_foreach (list, unref_object, NULL);
      g_list_free (list);
      g_hash_table_unref (application->hash);
      application->hash = NULL;
    }

  if (application->root)
    {
      g_clear_object (&application->root->parent.app);
      g_object_unref (application->root);
      application->root = NULL;
    }

  G_OBJECT_CLASS (atspi_application_parent_class)->dispose (object);
}

static void
atspi_application_finalize (GObject *object)
{
  AtspiApplication *application = ATSPI_APPLICATION (object);

  g_free (application->bus_name);
  g_free (application->toolkit_name);
  g_free (application->toolkit_version);
  g_free (application->atspi_version);

  G_OBJECT_CLASS (atspi_application_parent_class)->finalize (object);
}

static void
atspi_application_class_init (AtspiApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = atspi_application_dispose;
  object_class->finalize = atspi_application_finalize;
}

AtspiApplication *
_atspi_application_new (const gchar *bus_name)
{
  AtspiApplication *application;

  application = g_object_new (ATSPI_TYPE_APPLICATION, NULL);
  application->hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  application->bus_name = g_strdup (bus_name);
  application->root = NULL;
  return application;
}
