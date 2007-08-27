/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Ximian Inc.
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

/* base.c: the base object managing an AtkObject proxy */

#include <config.h>
#include <stdio.h>
#include <libspi/libspi.h>

/* Our parent Gtk object type  */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

/* A pointer to our parent object class */
static GObjectClass *spi_base_parent_class;

/*
 * Implemented GObject::dispose
 */
static void
spi_base_object_dispose (GObject *gobject)
{
  SpiBase *object = SPI_BASE (gobject);

  if (object->gobj)
    {
      g_assert (G_IS_OBJECT (object->gobj));
      g_object_unref (object->gobj);
      object->gobj = NULL;
    }

  spi_base_parent_class->dispose (gobject);
}

static void
spi_base_class_init (SpiBaseClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;

        spi_base_parent_class = g_type_class_peek_parent (klass);

        object_class->dispose = spi_base_object_dispose;
}

static void
spi_base_init (SpiBase *object)
{
	object->gobj = NULL;
}

BONOBO_TYPE_FUNC (SpiBase, PARENT_TYPE, spi_base)

void
spi_base_construct (SpiBase *object, GObject *gobject)
{
 g_return_if_fail (G_IS_OBJECT (gobject));
 object->gobj = g_object_ref (gobject);
}

void
spi_base_construct_default (SpiBase *object)
{
  object->gobj = g_object_new (G_TYPE_OBJECT, NULL);
}

GObject *
spi_base_get_gobject (SpiBase *object)
{
  return object->gobj;
}
