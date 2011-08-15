/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2010 Codethink Ltd.
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

#ifndef ACCESSIBLE_CACHE_H
#define ACCESSIBLE_CACHE_H

#include <glib.h>
#include <glib-object.h>

typedef struct _SpiCache SpiCache;
typedef struct _SpiCacheClass SpiCacheClass;

G_BEGIN_DECLS

#define SPI_CACHE_TYPE        (spi_cache_get_type ())
#define SPI_CACHE(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_CACHE_TYPE, SpiCache))
#define SPI_CACHE_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_CACHE_TYPE, SpiCacheClass))
#define SPI_IS_CACHE(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_CACHE_TYPE))
#define SPI_IS_CACHE_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_CACHE_TYPE))

struct _SpiCache
{
  GObject parent;

  GHashTable * objects;
  GQueue *add_traversal;
  gint add_pending_idle;

  guint child_added_listener;
};

struct _SpiCacheClass
{
  GObjectClass parent_class;
};

GType spi_cache_get_type (void);

extern SpiCache *spi_global_cache;

void
spi_cache_foreach (SpiCache * cache, GHFunc func, gpointer data);

gboolean
spi_cache_in (SpiCache * cache, GObject * object);

G_END_DECLS
#endif /* ACCESSIBLE_CACHE_H */
