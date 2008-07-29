/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Novell, Inc.
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

#ifndef _BITARRAY_H
#define _BITARRAY_H

#include "dbus/dbus.h"
#include "glib.h"

#define BITARRAY_SEQ_TERM 0xffffffff

#define BITARRAY_SET(p, n) ((p)[n/32] |= (1<<(n)))

gint bitarray_to_seq(dbus_uint32_t *array, gint array_size, gint **out);
dbus_uint32_t bitarray_from_seq(gint *seq, dbus_uint32_t **out);
#endif	/* _BITARRAY_H */
