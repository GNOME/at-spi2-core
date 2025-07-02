/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 * Copyright 2010, 2011 Novell, Inc.
 *
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

#include <glib.h>

#include "atspi-accessible.h"
#include "atspimarshal.h"

G_BEGIN_DECLS

struct _AtspiAccessiblePrivate
{
  GHashTable *cache;
  guint cache_ref_count;
  guint iteration_stamp;
  gboolean holds_cache_ref;
};

GHashTable *
_atspi_accessible_ref_cache (AtspiAccessible *accessible);

void
_atspi_accessible_unref_cache (AtspiAccessible *accessible);

void
_atspi_accessible_set_cached (AtspiAccessible *accessible, gboolean cached);
G_END_DECLS
