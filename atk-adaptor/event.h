/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

void spi_atk_register_event_listeners (void);
void spi_atk_deregister_event_listeners (void);
void spi_atk_tidy_windows (void);

gboolean spi_event_is_subtype (gchar **needle, gchar **haystack);

extern GMainContext *spi_context;
guint spi_idle_add (GSourceFunc function, gpointer data);
guint spi_timeout_add_seconds (gint interval, GSourceFunc function, gpointer data);
guint spi_timeout_add_full (gint priority, guint interval, GSourceFunc function, gpointer data, GDestroyNotify notify);
