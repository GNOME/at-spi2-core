/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; https://wiki.gnome.org/Accessibility)
 *
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
#include <glib-object.h>
#include <stdio.h>
#include <string.h>

#include "my-atk-editable-text.h"
#include "my-atk-object.h"

typedef struct _MyAtkEditableTextInfo MyAtkEditableTextInfo;

static void atk_editable_text_interface_init (AtkEditableTextIface *iface);

G_DEFINE_TYPE_WITH_CODE (MyAtkEditableText,
                         my_atk_editable_text,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_EDITABLE_TEXT,
                                                atk_editable_text_interface_init));

guint
my_atk_set_editable_text (AtkEditableText *editable_text, const gchar *text)
{
  g_return_val_if_fail (MY_IS_ATK_EDITABLE_TEXT (editable_text), -1);

  return 0;
}

static void
my_atk_editable_text_init (MyAtkEditableText *obj)
{
  obj->text = NULL;
}

static gboolean
my_atk_set_editable_text_set_run_attributes (AtkEditableText *text,
                                             AtkAttributeSet *attrib_set,
                                             gint start_offset,
                                             gint end_offset)
{
  return FALSE;
}

static void
my_atk_set_editable_text_set_text_contents (AtkEditableText *text,
                                            const gchar *string)
{
}

static void
my_atk_set_editable_text_insert_text (AtkEditableText *text,
                                      const gchar *string,
                                      gint length,
                                      gint *position)
{
}

static void
my_atk_set_editable_text_copy_text (AtkEditableText *text,
                                    gint start_pos,
                                    gint end_pos)
{
}

static void
my_atk_set_editable_text_cut_text (AtkEditableText *text,
                                   gint start_pos,
                                   gint end_pos)
{
}

static void
my_atk_set_editable_text_delete_text (AtkEditableText *text,
                                      gint start_pos,
                                      gint end_pos)
{
}

static void
my_atk_set_editable_text_paste_text (AtkEditableText *text,
                                     gint position)
{
}

static void
atk_editable_text_interface_init (AtkEditableTextIface *iface)
{
  if (!iface)
    return;
  iface->set_run_attributes = my_atk_set_editable_text_set_run_attributes;
  iface->set_text_contents = my_atk_set_editable_text_set_text_contents;
  iface->insert_text = my_atk_set_editable_text_insert_text;
  iface->copy_text = my_atk_set_editable_text_copy_text;
  iface->cut_text = my_atk_set_editable_text_cut_text;
  iface->delete_text = my_atk_set_editable_text_delete_text;
  iface->paste_text = my_atk_set_editable_text_paste_text;
}

static void
my_atk_editable_text_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_editable_text_finalize (GObject *object)
{
}

static void
my_atk_editable_text_class_init (MyAtkEditableTextClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_editable_text_finalize;

  atk_class->initialize = my_atk_editable_text_initialize;
}
