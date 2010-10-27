/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include "atspi-private.h"

G_DEFINE_TYPE (AtspiApplication, atspi_application, G_TYPE_OBJECT)

static void
atspi_application_init (AtspiApplication *application)
{
}

static void
atspi_application_finalize (AtspiApplication *application)
{
  if (application->bus_name)
    g_free (application->bus_name);
}

static void
atspi_application_class_init (AtspiApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = atspi_application_finalize;
}

AtspiApplication *
_atspi_application_new (const gchar *bus_name)
{
  AtspiApplication *application;
  
  application = g_object_new (ATSPI_TYPE_APPLICATION, NULL);
  if (application)
    application->bus_name = g_strdup (bus_name);
  return application;
}
