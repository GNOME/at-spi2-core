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

static void atk_text_base_init (gpointer *g_class);

static guint atk_text_signals[LAST_SIGNAL] = { 0 };

GType
atk_text_get_type ()
{
  static GType type = 0;

  if (!type) 
    {
      static const GTypeInfo tinfo =
      {
        sizeof (AtkTextIface),
        (GBaseInitFunc) atk_text_base_init,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) NULL /* atk_text_interface_init */ ,
        (GClassFinalizeFunc) NULL,

      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtkText", &tinfo, 0);
    }

  return type;
}

static void
atk_text_base_init (gpointer *g_class)
{
  static gboolean initialized = FALSE;

  if (! initialized)
    {

  /* 
   * Note that text_changed signal supports details "insert", "delete", 
   * possibly "replace". 
   */

    atk_text_signals[TEXT_CHANGED] =
      g_signal_newc ("text_changed",
                     ATK_TYPE_TEXT,
                     G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                     G_STRUCT_OFFSET (AtkTextIface, text_changed), 
                     (GSignalAccumulator) NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE,
                     0, G_TYPE_NONE);

    atk_text_signals[CARET_MOVED] =
      g_signal_newc ("text_caret_moved",
                     ATK_TYPE_TEXT,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTextIface, caret_changed),
                     (GSignalAccumulator) NULL, NULL,
                     g_cclosure_marshal_VOID__INT,
                     G_TYPE_NONE,
                     1, G_TYPE_INT);
  }
}

/**
 * atk_text_get_text:
 * @text: an #AtkText
 * @start_offset: start position
 * @end_offset: end position
 *
 * Returns: the text from @start_offset up to, but not including @end_offset.
 **/
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

/**
 * atk_text_get_character_at_offset
 * @text: an #AtkText
 * @offset: position
 *
 * Returns: the character at @offset.
 **/
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

/**
 * atk_text_get_text_after_offset
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 *
 * Returns: the text after @offset up to the specified @boundary_type.
 * If the boundary type is ATK_TEXT_BOUNDARY_WORD_START or
 * ATK_TEXT_BOUNDARY_WORD_END part of a word may be returned.
 * If the boundary type is ATK_TEXT_BOUNDARY_SENTENCE_START the start point 
 * will be the offset and will continue to the start of the next sentence. 
 * The first word may not be a complete word. Similarly for 
 * ATK_TEXT_BOUNDARY_SENTENCE_END, ATK_TEXT_BOUNDARY_LINE_START and
 * ATK_TEXT_BOUNDARY_LINE_END
 **/
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

/**
 * atk_text_get_text_at_offset
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 *
 * Returns: the text at @offset up to the specified @boundary_type.
 * If the boundary_type is ATK_TEXT_BOUNDARY_WORD_START or 
 * ATK_TEXT_BOUNDARY_WORD_END a complete word is returned; 
 * if the boundary type is  ATK_TEXT_BOUNDARY_SENTENCE_START or 
 * ATK_TEXT_BOUNDARY_SENTENCE_END a complete sentence
 * is returned; if the boundary type is ATK_TEXT_BOUNDARY_LINE_START or
 * ATK_TEXT_BOUNDARY_LINE_END a complete line is returned.
 **/
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

/**
 * atk_text_get_text_before_offset
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 *
 * Returns: the text before @offset starting from the specified @boundary_type.
 * If the boundary type is ATK_TEXT_BOUNDARY_WORD_START or
 * ATK_TEXT_BOUNDARY_WORD_END part of a word may be returned.
 * If the boundary type is ATK_TEXT_BOUNDARY_SENTENCE_START the start point 
 * will be at the start of the sentence, and will continue to the offset. 
 * The last word may not be a complete word. Similarly for 
 * ATK_TEXT_BOUNDARY_SENTENCE_END, ATK_TEXT_BOUNDARY_LINE_START and
 * ATK_TEXT_BOUNDARY_LINE_END
 **/
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

/**
 * atk_text_get_caret_offset
 * @text: an #AtkText
 *
 * Returns: the position of the caret (cursor).
 **/
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

/**
 * atk_text_get_row_col_at_offset
 * @text: an #AtkText
 * @offset: position
 * @row: row number
 * @col: column number
 *
 * Given an @offset, the @row and @col arguments are filled appropriately.
 **/
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

/**
 * atk_text_get_range_attributes
 * @text: an #AtkText
 * @start_offset: start position
 * @end_offset: end position
 *
 * Returns a #PangoAttrList with the text attributes between the
 * @start_offset and the @end_offset.
 **/
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

/**
 * atk_text_get_character_extents
 * @text: an #AtkText
 * @offset: position
 * @x: x-position of character
 * @y: y-position of character
 * @length: length of character
 * @width: width of character
 *
 * Given an @offset, the @x, @y, @length, and @width values are filled
 * appropriately.
 **/
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

/**
 * atk_text_get_character_count
 * @text: an #AtkText
 *
 * Returns: the number of characters.
 **/
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

/**
 * atk_text_get_offset_at_point
 * @text: an #AtkText
 * @x: x-position of character
 * @y: y-position of character
 *
 * Returns: the offset to the character which is located at
 * the specified @x and @y coordinates.
 **/
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

/**
 * atk_text_get_selected_text
 * @text: an #AtkText
 *
 * Returns: the selected text.
 **/
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

/**
 * atk_text_get_selection_bounds
 * @text: an #AtkText
 * @start_offset: start position
 * @end_offset: end position
 *
 * @start_offset and @end_offset are filled with the
 * current selection bounds.
 **/
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

/**
 * atk_text_set_selection_bounds
 * @text: an #AtkText
 * @start_offset: start position
 * @end_offset: end position
 *
 * The selection bounds are set to the specified @start_offset
 * and @end_offset values.
 * 
 * Returns: %TRUE if success, %FALSE otherwise.
 **/
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

/**
 * atk_text_set_caret_offset
 * @text: an #AtkText
 * @offset: position
 *
 * Sets the caret (cursor) position to the specified @offset.
 *
 * Returns: %TRUE if success, %FALSE otherwise.
 **/
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
