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

#include "bitarray.h"

gint bitarray_to_seq(dbus_uint32_t *array, gint array_size, gint **out)
{
  gint *seq = NULL;
  gint seq_size = 0;
  int i, j;

  for (i = 0; i < array_size; i++)
  {
    for (j = 0; j < 32; j++)
    {
      if (array[i] & (1 << j))
      {
	if (!(seq_size % 4))
	{
	  gint *new_seq = (gint *)realloc(seq, (seq_size + 4) * sizeof(gint));
	  if (!new_seq)
	  {
	    *out = seq;
	    return seq_size;
	  }
	  seq = new_seq;
	}
	seq[++seq_size] = i * 32 + j;
      }
    }
  }
  *out = seq;
  return seq_size;
}

dbus_uint32_t bitarray_from_seq(gint *seq, dbus_uint32_t **out)
{
  dbus_uint32_t *array = NULL;
  dbus_uint32_t array_size = 0;
  dbus_uint32_t array_max_size = 0;
  int i;

  for (i = 0; seq[i] != BITARRAY_SEQ_TERM; i++)
  {
    gint pos = seq[i] / 32, val = seq[i] % 32;
    if (pos >= array_max_size)
    {
      gint *new_array;
      while (array_max_size <= pos) array_max_size += 4;
      new_array = (dbus_uint32_t *)realloc(array, array_max_size * sizeof(dbus_uint32_t));
      if (!new_array)
      {
	*out = array;
	return array_size;
      }
      array = new_array;
    }
    array[pos] &= (1 << val);
    if (pos > array_size) array_size = pos;
  }
  *out = array;
  return array_size;
}
