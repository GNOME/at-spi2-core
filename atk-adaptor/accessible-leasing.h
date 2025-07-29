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

typedef struct _SpiLeasing SpiLeasing;
typedef struct _SpiLeasingClass SpiLeasingClass;

G_BEGIN_DECLS

#define SPI_LEASING_TYPE (spi_leasing_get_type ())
#define SPI_LEASING(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_LEASING_TYPE, SpiLeasing))
#define SPI_LEASING_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), SPI_LEASING_TYPE, SpiLeasingClass))
#define SPI_IS_LEASING(o) (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_LEASING_TYPE))
#define SPI_IS_LEASING_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_LEASING_TYPE))

struct _SpiLeasing
{
  GObject parent;

  GQueue *expiry_queue;
  GHashTable *expiry_queue_hash;
  guint expiry_func_id;
};

struct _SpiLeasingClass
{
  GObjectClass parent_class;
};

GType spi_leasing_get_type (void);

extern SpiLeasing *spi_global_leasing;

GObject *spi_leasing_take (SpiLeasing *leasing, GObject *object);

G_END_DECLS
