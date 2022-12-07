/*
 * Copyright 2008 Codethink Ltd.
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <atk/atk.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "my-atk-hyperlink.h"
#include "my-atk-hypertext.h"
#include "my-atk-object.h"

typedef struct _MyAtkHypertextInfo MyAtkHypertextInfo;

static void atk_hypertext_interface_init (AtkHypertextIface *iface);
static void GDestroyNotifyGOBJptrArray (gpointer data);

G_DEFINE_TYPE_WITH_CODE (MyAtkHypertext,
                         my_atk_hypertext,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_HYPERTEXT,
                                                atk_hypertext_interface_init));

gint
my_atk_set_hypertext (AtkHypertext *obj, const gchar *text)
{
  MyAtkHypertext *self = MY_ATK_HYPERTEXT (obj);
  MyAtkHyperlink *link;
  gchar *ptr;
  const gchar *fstr = "href=";
  gint len = strlen (fstr);
  gint text_len = strlen (text);
  gint index = 0;
  gint start_offset;
  g_return_val_if_fail (MY_IS_ATK_HYPERTEXT (obj), -1);

  if (text_len < len)
    return -1;

  while (text)
    {
      ptr = g_strstr_len (text + index, text_len - index, fstr);
      index = ptr - text + len;
      if (ptr)
        {
          if (text_len < index || index < len)
            break;
          if (text[index] == '\'')
            {
              start_offset = index + 1;
              while (++index < text_len && text[index] != '\'')
                ;
              if (text[index] != '\'')
                break;
              link = new_MyAtkHyperlink ();
              my_atk_set_hyperlink (ATK_HYPERLINK (link),
                                    g_strndup (text + start_offset, index - start_offset),
                                    start_offset,
                                    index);
              g_ptr_array_add (self->array, link);
            }
        }
      else
        break;
    }

  return self->array->len > 0 ? 0 : -1;
}

static gint
my_atk_hypertext_get_n_links (AtkHypertext *obj)
{
  MyAtkHypertext *self = MY_ATK_HYPERTEXT (obj);
  g_return_val_if_fail (MY_IS_ATK_HYPERTEXT (obj), -1);
  return self->array->len;
}

static AtkHyperlink *
my_atk_hypertext_get_link (AtkHypertext *obj, gint link_index)
{
  MyAtkHypertext *self = MY_ATK_HYPERTEXT (obj);
  AtkHyperlink *link = NULL;
  g_return_val_if_fail (MY_IS_ATK_HYPERTEXT (obj), NULL);
  if (0 <= link_index && link_index < self->array->len)
    link = g_ptr_array_index (self->array, link_index);
  return link;
}

static gint
my_atk_hypertext_get_link_index (AtkHypertext *obj, gint char_index)
{
  MyAtkHypertext *self = MY_ATK_HYPERTEXT (obj);
  gint i;
  MyAtkHyperlink *link;
  g_return_val_if_fail (MY_IS_ATK_HYPERTEXT (obj), -1);

  for (i = 0; i < self->array->len; i++)
    {
      link = g_ptr_array_index (self->array, i);
      if (link->start <= char_index && char_index <= link->end)
        return i;
    }
  return -1;
}

static void
my_atk_hypertext_link_selected (AtkHypertext *obj, gint link_index)
{
  g_return_if_fail (MY_IS_ATK_HYPERTEXT (obj));
}

static void
GDestroyNotifyGOBJptrArray (gpointer data)
{
  g_object_unref (data);
}

static void
atk_hypertext_interface_init (AtkHypertextIface *iface)
{
  if (!iface)
    return;
  iface->get_n_links = my_atk_hypertext_get_n_links;
  iface->get_link = my_atk_hypertext_get_link;
  iface->get_link_index = my_atk_hypertext_get_link_index;
  iface->link_selected = my_atk_hypertext_link_selected;
}

static void
my_atk_hypertext_init (MyAtkHypertext *self)
{
  self->array = g_ptr_array_new_full (2, GDestroyNotifyGOBJptrArray);
}

static void
my_atk_hypertext_class_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_hypertext_class_finalize (GObject *obj)
{
}

static void
my_atk_hypertext_class_init (MyAtkHypertextClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_hypertext_class_finalize;

  atk_class->initialize = my_atk_hypertext_class_initialize;
}
