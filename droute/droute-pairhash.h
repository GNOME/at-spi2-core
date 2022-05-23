/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2008 Codethink Ltd.
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
#ifndef _DROUTE_PAIRHASH_H
#define _DROUTE_PAIRHASH_H

#include <glib.h>

typedef struct _StrPair StrPair;
struct _StrPair
{
    const gchar *one;
    const gchar *two;
};

StrPair *str_pair_new     (const gchar *one,
                           const gchar *two);

guint    str_pair_hash    (gconstpointer key);
gboolean str_pair_equal   (gconstpointer a,
                           gconstpointer b);

#endif /* _DROUTE_PAIRHASH_H */
