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

G_DEFINE_TYPE (AtspiObject, atspi_object, G_TYPE_OBJECT)

static void
atspi_object_init (AtspiObject *obj)
{
}

static void
atspi_object_dispose (GObject *object)
{
  AtspiObject *aobj = ATSPI_OBJECT (object);

  if (aobj->app)
  {
    g_object_unref (aobj->app);
    aobj->app = NULL;
  }

  G_OBJECT_CLASS (atspi_object_parent_class)->dispose (object);
}

static void
atspi_object_finalize (GObject *object)
{
  AtspiObject *aobj = ATSPI_OBJECT (object);

  g_free (aobj->path);

  G_OBJECT_CLASS (atspi_object_parent_class)->finalize (object);
}

static void
atspi_object_class_init (AtspiObjectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = atspi_object_dispose;
  object_class->finalize = atspi_object_finalize;
}
