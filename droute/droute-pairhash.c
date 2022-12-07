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

#include "droute-pairhash.h"

/*---------------------------------------------------------------------------*/

static guint
str_hash (guint32 h, const char *p)
{
  for (p += 1; *p != '\0'; p++)
    h = (h << 5) - h + *p;

  return h;
}

/*---------------------------------------------------------------------------*/

StrPair *
str_pair_new (const gchar *one, const gchar *two)
{
  StrPair *pair;

  pair = g_new (StrPair, 1);
  pair->one = one;
  pair->two = two;
  return pair;
}

guint
str_pair_hash (gconstpointer key)
{
  StrPair *pair = (StrPair *) key;
  guint hash = 0;

  /*g_return_val_if_fail (pair      != NULL, 0);
    g_return_val_if_fail (pair->one != NULL, 0);
    g_return_val_if_fail (pair->two != NULL, 0);
  */

  if (*(pair->two) != '\0')
    {
      hash = *(pair->two);
      hash = str_hash (hash, pair->two);
      hash = str_hash (hash, pair->one);
    }

  return hash;
}

gboolean
str_pair_equal (gconstpointer a, gconstpointer b)
{
  StrPair *ap = (StrPair *) a;
  StrPair *bp = (StrPair *) b;

  if (g_str_equal (ap->one, bp->one) &&
      g_str_equal (ap->two, bp->two))
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

/*END------------------------------------------------------------------------*/
