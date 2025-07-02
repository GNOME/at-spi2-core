/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008, 2009 Codethink Ltd.
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

#include <glib-object.h>
#include <glib.h>

typedef struct _SpiRegister SpiRegister;
typedef struct _SpiRegisterClass SpiRegisterClass;

G_BEGIN_DECLS

#define SPI_REGISTER_TYPE (spi_register_get_type ())
#define SPI_REGISTER(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_REGISTER_TYPE, SpiRegister))
#define SPI_REGISTER_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SPI_REGISTER_TYPE, SpiRegisterClass))
#define SPI_IS_REGISTER(o) (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_REGISTER_TYPE))
#define SPI_IS_REGISTER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_REGISTER_TYPE))

struct _SpiRegister
{
  GObject parent;

  GHashTable *ref2ptr;
  guint reference_counter;
};

struct _SpiRegisterClass
{
  GObjectClass parent_class;
};

GType spi_register_get_type (void);

extern SpiRegister *spi_global_register;

/*---------------------------------------------------------------------------*/

GObject *
spi_register_path_to_object (SpiRegister *reg, const char *path);

GObject *
spi_global_register_path_to_object (const char *path);

gchar *
spi_register_object_to_path (SpiRegister *reg, GObject *gobj);

guint
spi_register_object_to_ref (GObject *gobj);

gchar *
spi_register_root_object_path ();

void
spi_register_deregister_object (SpiRegister *reg, GObject *gobj, gboolean unref);

/*---------------------------------------------------------------------------*/
