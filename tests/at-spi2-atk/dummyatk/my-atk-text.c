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
#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "my-atk-object.h"
#include "my-atk-text.h"

typedef struct _MyAtkTextInfo MyAtkTextInfo;

static void atk_text_interface_init (AtkTextIface *iface);

typedef struct _MyAtkTextSelection MyAtkTextSelection;

struct _MyAtkTextSelection
{
  gint start;
  gint end;
};

G_DEFINE_TYPE_WITH_CODE (MyAtkText,
                         my_atk_text,
                         MY_TYPE_ATK_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATK_TYPE_TEXT,
                                                atk_text_interface_init));

guint
my_atk_set_text (AtkText *obj,
                 const gchar *text,
                 const gint x,
                 const gint y,
                 const gint width,
                 const gint height,
                 AtkAttributeSet *attrSet)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), -1);

  MyAtkText *self = MY_ATK_TEXT (obj);
  self->text = g_strdup (text);
  self->x = x;
  self->y = y;
  self->width = width;
  self->height = height;
  self->attributes = g_slist_copy (attrSet);

  return 0;
}

MyAtkText *
my_atk_text_new (void)
{
  return g_object_new (MY_TYPE_ATK_TEXT, NULL);
}

static gchar *
my_atk_text_get_text (AtkText *obj, gint start_offset, gint end_offset)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), NULL);
  gchar *str = MY_ATK_TEXT (obj)->text;

  if ((end_offset < start_offset) || start_offset < 0 || !str)
    return NULL;
  if (strlen (str) < end_offset)
    return NULL;

  return g_strndup (str + start_offset, end_offset - start_offset);
}

static gint
my_atk_text_get_character_count (AtkText *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), -1);
  gchar *str = MY_ATK_TEXT (obj)->text;
  if (!str)
    return 0;
  return (gint) strlen (str);
}

static int
my_atk_text_get_caret_offset (AtkText *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), -1);
  return MY_ATK_TEXT (obj)->caret_offset;
}

static gboolean
my_atk_text_set_caret_offset (AtkText *obj, gint offset)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), FALSE);
  MyAtkText *self = MY_ATK_TEXT (obj);
  if (offset < 0 && strlen (self->text) <= offset)
    return FALSE;
  self->caret_offset = offset;
  return TRUE;
}

static gunichar
my_atk_text_get_character_at_offset (AtkText *obj, gint offset)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), 255);
  return MY_ATK_TEXT (obj)->text[offset];
}

static void
my_atk_text_get_character_extents (AtkText *obj, gint offset, gint *x, gint *y, gint *width, gint *height, AtkCoordType coords)
{
  g_return_if_fail (MY_IS_ATK_TEXT (obj));
  MyAtkText *self = MY_ATK_TEXT (obj);
  *x = self->x;
  *y = self->y;
  *width = self->width;
  *height = self->height;
}

static void
my_atk_text_get_range_extents (AtkText *obj, gint start_offset, gint stop_offset, AtkCoordType coords, AtkTextRectangle *rect)
{
  g_return_if_fail (MY_IS_ATK_TEXT (obj));
  MyAtkText *self = MY_ATK_TEXT (obj);
  rect->x = self->x;
  rect->y = self->y;
  rect->width = self->width;
  rect->height = self->height;
}

static gint
my_atk_text_get_n_selections (AtkText *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), -1);
  return g_list_length (MY_ATK_TEXT (obj)->selection);
}

static gboolean
my_atk_text_add_selection (AtkText *obj, gint start_offset, gint end_offset)
{
  MyAtkText *self = MY_ATK_TEXT (obj);
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), FALSE);

  MyAtkTextSelection *node = g_malloc (sizeof (MyAtkTextSelection));

  node->start = start_offset;
  node->end = end_offset;

  self->selection = g_list_append (self->selection, node);

  return TRUE;
}

static gchar *
my_atk_text_get_selection (AtkText *obj, gint selection_num, gint *start_offset, gint *end_offset)
{
  MyAtkText *self = MY_ATK_TEXT (obj);
  gchar *str = NULL;
  GList *it;
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), NULL);

  if (selection_num < 0)
    return NULL;

  it = g_list_nth (self->selection, selection_num);
  if (!it)
    return NULL;

  str = my_atk_text_get_text (obj, ((MyAtkTextSelection *) it->data)->start, ((MyAtkTextSelection *) it->data)->end);
  if (!str)
    return NULL;
  *start_offset = ((MyAtkTextSelection *) it->data)->start;
  *end_offset = ((MyAtkTextSelection *) it->data)->end;

  return str;
}

static gboolean
my_atk_text_set_selection (AtkText *obj, gint selection_num, gint start_offset, gint end_offset)
{
  MyAtkText *self = MY_ATK_TEXT (obj);
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), FALSE);

  GList *it;

  if (selection_num < 0)
    return FALSE;

  it = g_list_nth (self->selection, selection_num);
  if (!it)
    return FALSE;

  ((MyAtkTextSelection *) it->data)->start = start_offset;
  ((MyAtkTextSelection *) it->data)->end = end_offset;

  return TRUE;
}

static gboolean
my_atk_text_remove_selection (AtkText *obj, gint selection_num)
{
  MyAtkText *self = MY_ATK_TEXT (obj);
  GList *it;
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), FALSE);

  if (selection_num < 0)
    return FALSE;

  it = g_list_nth (self->selection, selection_num);
  if (!it)
    return FALSE;

  self->selection = g_list_delete_link (self->selection, it);
  return TRUE;
}

static gint
my_atk_text_get_offset_at_point (AtkText *obj, gint x, gint y, AtkCoordType coords)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), -1);
  return 5;
}

static AtkAttributeSet *
my_atk_text_get_default_attributes (AtkText *obj)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), NULL);
  return MY_ATK_TEXT (obj)->attributes;
}

static AtkAttributeSet *
my_atk_text_get_run_attributes (AtkText *obj, gint offset, gint *start_offset, gint *end_offset)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), NULL);
  AtkAttributeSet *attributes;
  AtkAttribute *attr;

  attr = g_malloc (sizeof (AtkAttribute));
  attr->name = g_strdup ("text_test_attr1");
  attr->value = g_strdup ("on");
  attributes = g_slist_append (NULL, attr);

  attr = g_malloc (sizeof (AtkAttribute));
  attr->name = g_strdup ("text_test_attr2");
  attr->value = g_strdup ("off");
  attributes = g_slist_append (attributes, attr);

  *start_offset = 5;
  *end_offset = 10;

  return attributes;
}

static void
setSentenceStartEnd (MyAtkText *self, gint *_offset, gint *start_offset, gint *end_offset, const gchar *fstr)
{
  gchar *p_str_begin = NULL;
  gchar *p_str_end = NULL;
  const gint length = strlen (self->text);
  gint offset = *_offset;
  gint star_correction = 1;
  /*
   * In case if offset is in the middle of the word rewind to 1 character before.
   */
  for (; g_ascii_isalpha (self->text[offset]) && 0 < offset; offset--)
    ;
  /*
   * if [char]  rewind to word after by passing none alpha
   * else  try to find last [string] in range [0,offset]
   *   if  found then correct position
   *   else not found so this is first sentence find first word
   */
  if (self->text[offset] == fstr[0])
    {
      for (; !g_ascii_isalpha (self->text[offset]) && offset < length; offset++)
        ;
      p_str_begin = self->text + offset;
    }
  else
    {
      p_str_begin = g_strrstr_len (self->text, offset, fstr);
      if (p_str_begin)
        {
          for (; !g_ascii_isalpha (self->text[offset]) && length < offset; offset++)
            ;
        }
      else
        {
          for (offset = 0; !g_ascii_isalpha (self->text[offset]) && length < offset; offset++)
            ;
          star_correction = 0;
        }
      p_str_begin = self->text + offset;
    }
  /*
   * try find ending
   * if not found set ending at text end.
   * */
  p_str_end = g_strstr_len (self->text + offset, length - offset, fstr);
  if (!p_str_end)
    {
      p_str_end = self->text + (length - 1);
    }
  if (p_str_begin && p_str_end)
    {
      *start_offset = p_str_begin - self->text + star_correction;
      *end_offset = p_str_end - self->text + 1;
      *_offset = offset;
    }
}

static gchar *
my_atk_text_get_string_at_offset (AtkText *obj, gint offset, AtkTextGranularity granularity, gint *start_offset, gint *end_offset)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), NULL);
  MyAtkText *self = MY_ATK_TEXT (obj);
  gint cnt;
  gint length;
  gint myoffset = 0;
  *start_offset = -1;
  *end_offset = -1;

  switch (granularity)
    {
    case ATK_TEXT_GRANULARITY_CHAR:
      *start_offset = offset;
      *end_offset = *start_offset + 1;
      break;
    case ATK_TEXT_GRANULARITY_WORD:
      length = strlen (self->text);
      for (; !g_ascii_isalpha (self->text[offset]) && offset < length; offset++)
        ;
      for (cnt = offset; cnt < length; cnt++)
        {
          if (!g_ascii_isalpha (self->text[cnt]))
            {
              *start_offset = offset;
              *end_offset = cnt - 1;
              myoffset = 1;
              break;
            }
        }
      for (cnt = offset; 0 < cnt; cnt--)
        {
          if (!g_ascii_isalpha (self->text[cnt]))
            {
              *start_offset = cnt + 1;
              break;
            }
        }
      break;
    case ATK_TEXT_GRANULARITY_SENTENCE:
      setSentenceStartEnd (self, &offset, start_offset, end_offset, ".");
      break;
    case ATK_TEXT_GRANULARITY_LINE:
      setSentenceStartEnd (self, &offset, start_offset, end_offset, "/n");
      break;
    case ATK_TEXT_GRANULARITY_PARAGRAPH:
      /* Not implemented */
      *start_offset = 0;
      *end_offset = 0;
      break;
    default:
      break;
    }
  return my_atk_text_get_text (obj, *start_offset, *end_offset + myoffset);
}

AtkTextRange **
my_atk_get_bounded_ranges (AtkText *obj, AtkTextRectangle *rect, AtkCoordType ctype, AtkTextClipType xclip, AtkTextClipType yclip)
{
  g_return_val_if_fail (MY_IS_ATK_TEXT (obj), NULL);
  AtkTextRange **range = g_new (AtkTextRange *, 3);

  *range = g_new (AtkTextRange, 1);
  (*range)->start_offset = 0;
  (*range)->end_offset = 5;
  (*range)->content = my_atk_text_get_text (obj, (*range)->start_offset, (*range)->end_offset);

  *(range + 1) = g_new (AtkTextRange, 1);
  (*(range + 1))->start_offset = 6;
  (*(range + 1))->end_offset = 10;
  (*(range + 1))->content = my_atk_text_get_text (obj, (*(range + 1))->start_offset, (*(range + 1))->end_offset);

  *(range + 2) = NULL;

  return range;
}

static void
atk_text_interface_init (AtkTextIface *iface)
{
  if (!iface)
    return;

  iface->get_text = my_atk_text_get_text;
  iface->get_character_count = my_atk_text_get_character_count;
  iface->get_caret_offset = my_atk_text_get_caret_offset;
  iface->set_caret_offset = my_atk_text_set_caret_offset;
  iface->get_character_at_offset = my_atk_text_get_character_at_offset;
  iface->get_character_extents = my_atk_text_get_character_extents;
  iface->get_range_extents = my_atk_text_get_range_extents;
  iface->get_n_selections = my_atk_text_get_n_selections;
  iface->add_selection = my_atk_text_add_selection;
  iface->get_selection = my_atk_text_get_selection;
  iface->set_selection = my_atk_text_set_selection;
  iface->remove_selection = my_atk_text_remove_selection;
  iface->get_offset_at_point = my_atk_text_get_offset_at_point;
  iface->get_default_attributes = my_atk_text_get_default_attributes;
  iface->get_string_at_offset = my_atk_text_get_string_at_offset;
  iface->get_bounded_ranges = my_atk_get_bounded_ranges;
  iface->get_run_attributes = my_atk_text_get_run_attributes;
}

static void
my_atk_text_init (MyAtkText *self)
{
  self->text = NULL;
  self->caret_offset = -1;
  self->x = -1;
  self->y = -1;
  self->width = -1;
  self->height = -1;
  self->selection = NULL;
  self->attributes = NULL;
}

static void
my_atk_text_class_initialize (AtkObject *obj, gpointer data)
{
}

static void
my_atk_text_class_finalize (GObject *obj)
{
}

static void
my_atk_text_class_init (MyAtkTextClass *my_class)
{
  AtkObjectClass *atk_class = ATK_OBJECT_CLASS (my_class);
  GObjectClass *gobject_class = G_OBJECT_CLASS (my_class);

  gobject_class->finalize = my_atk_text_class_finalize;
  atk_class->initialize = my_atk_text_class_initialize;
}
