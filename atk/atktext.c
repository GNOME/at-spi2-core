/* ATK - The Accessibility Toolkit for GTK+
 * Copyright 2001 Sun Microsystems Inc.
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

#include "atktext.h"

enum {
  TEXT_CHANGED,
  CARET_MOVED,
  LAST_SIGNAL
};

struct _AtkTextIfaceClass
{
  GObjectClass parent;
};

typedef struct _AtkTextIfaceClass AtkTextIfaceClass;

static void atk_text_interface_init (AtkTextIfaceClass *klass);

static gpointer parent_class = NULL;

static guint atk_text_signals[LAST_SIGNAL] = { 0, 0, 0};

GType
atk_text_get_type ()
{
  static GType type = 0;

  if (!type) 
    {
      static const GTypeInfo tinfo =
      {
        sizeof (AtkTextIface),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) atk_text_interface_init,
        (GClassFinalizeFunc) NULL,

      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtkText", &tinfo, 0);
    }

  return type;
}

/*
 * Additional GObject properties exported by AtkText:
 *    "accessible_text" (accessible text has changed)
 *    "accessible_caret" (accessible text cursor position changed:
 *                         editable text only)
 */

static void
atk_text_interface_init (AtkTextIfaceClass *klass)
{
  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  /* 
   * Note that text_changed signal supports details "insert", "delete", 
   * possibly "replace". 
   */

  atk_text_signals[TEXT_CHANGED] =
    g_signal_newc ("text_changed",
                   G_TYPE_FROM_CLASS (klass),
                   G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                   G_STRUCT_OFFSET (AtkTextIface, text_changed), 
                   NULL,
                   g_cclosure_marshal_VOID__VOID,
                   G_TYPE_NONE,
                   0, G_TYPE_NONE);

  atk_text_signals[CARET_MOVED] =
    g_signal_newc ("text_caret_moved",
                   G_TYPE_FROM_CLASS (klass),
                   G_SIGNAL_RUN_LAST,
                   G_STRUCT_OFFSET (AtkTextIface, caret_changed),
                   NULL,
                   g_cclosure_marshal_VOID__INT,
                   G_TYPE_NONE,
                   1, G_TYPE_INT);
}

gchar*
atk_text_get_text (AtkText      *text,
                   gint         start_offset,
                   gint         end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text)
    return (*(iface->get_text)) (text, start_offset, end_offset);
  else
    return NULL;
}

gunichar
atk_text_get_character_at_offset (AtkText      *text,
                                  gint         offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, (gunichar) 0);
  g_return_val_if_fail (ATK_IS_TEXT (text), (gunichar) 0);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_at_offset)
    return (*(iface->get_character_at_offset)) (text, offset);
  else
    return (gunichar) 0;
}

gchar*
atk_text_get_text_after_offset (AtkText          *text,
                                gint             offset,
                                AtkTextBoundary  boundary_type)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_after_offset)
    return (*(iface->get_text_after_offset)) (text, offset, boundary_type);
  else
    return NULL;
}

gchar*
atk_text_get_text_at_offset (AtkText          *text,
                             gint             offset,
                             AtkTextBoundary  boundary_type)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_at_offset)
    return (*(iface->get_text_at_offset)) (text, offset, boundary_type);
  else
    return NULL;
}

gchar*
atk_text_get_text_before_offset (AtkText          *text,
                                 gint             offset,
                                 AtkTextBoundary  boundary_type)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_before_offset)
    return (*(iface->get_text_before_offset)) (text, offset, boundary_type);
  else
    return NULL;
}

gint
atk_text_get_caret_offset (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, -1);
  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_caret_offset)
    return (*(iface->get_caret_offset)) (text);
  else
    return -1;
}

void
atk_text_get_row_col_at_offset (AtkText *text,
                                gint offset,
                                gint *row,
                                gint *col)
{
  AtkTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_TEXT (text));

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_row_col_at_offset)
    (*(iface->get_row_col_at_offset)) (text, offset, row, col);
  else
    {
      *row = 0;
      *col = 0;
    }
}

PangoAttrList*
atk_text_get_range_attributes (AtkText *text,
                               gint start_offset,
                               gint end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_range_attributes)
    return (*(iface->get_range_attributes)) (text, start_offset, end_offset);
  else
    return NULL;
}

void
atk_text_get_character_extents (AtkText *text,
                                gint offset,
                                gint *x,
                                gint *y,
                                gint *length,
                                gint *width)
{
  AtkTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_TEXT (text));

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_extents)
    (*(iface->get_character_extents)) (text, offset, x, y, length, width);
  else
    {
      *x = 0;
      *x = 0;
      *length = 0;
      *width = 0;
    }
}

gint
atk_text_get_character_count (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, -1);
  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_count)
    return (*(iface->get_character_count)) (text);
  else
    return -1;
}

gint
atk_text_get_offset_at_point (AtkText *text,
                              gint x,
                              gint y)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, -1);
  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_offset_at_point)
    return (*(iface->get_offset_at_point)) (text, x, y);
  else
    return -1;
}

gchar*
atk_text_get_selected_text (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_selected_text)
    return (*(iface->get_selected_text)) (text);
  else
    return NULL;
}

void
atk_text_get_selection_bounds (AtkText *text,
                               gint    *start_offset,
                               gint    *end_offset)
{
  AtkTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_TEXT (text));

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_selection_bounds)
    (*(iface->get_selection_bounds)) (text, start_offset, end_offset);
  else
  {
    *start_offset = 0;
    *end_offset = 0;
  }
}

gboolean
atk_text_set_selection_bounds (AtkText *text,
                               gint    start_offset,
                               gint    end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->set_selection_bounds)
    {
      return (*(iface->set_selection_bounds)) (text, start_offset, end_offset);
    }
  else
    {
      return FALSE;
    }
}

gboolean
atk_text_set_caret_offset (AtkText *text,
                           gint    offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->set_caret_offset)
    {
      return (*(iface->set_caret_offset)) (text, offset);
    }
  else
    {
      return FALSE;
    }
}
