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
#include "atkmarshal.h"

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
	g_signal_new ("text_changed",
		      ATK_TYPE_TEXT,
		      G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
		      G_STRUCT_OFFSET (AtkTextIface, text_changed), 
		      (GSignalAccumulator) NULL, NULL,
		      atk_marshal_VOID__INT_INT,
		      G_TYPE_NONE,
		      2, G_TYPE_INT, G_TYPE_INT);
      
      atk_text_signals[CARET_MOVED] =
	g_signal_new ("text_caret_moved",
		      ATK_TYPE_TEXT,
		      G_SIGNAL_RUN_LAST,
		      G_STRUCT_OFFSET (AtkTextIface, caret_changed),
		      (GSignalAccumulator) NULL, NULL,
		      g_cclosure_marshal_VOID__INT,
		      G_TYPE_NONE,
		      1, G_TYPE_INT);
      
      initialized = TRUE;
    }
}

/**
 * atk_text_get_text:
 * @text: an #AtkText
 * @start_offset: start position
 * @end_offset: end position
 *
 * Gets the specified text.
 *
 * Returns: the text from @start_offset up to, but not including @end_offset.
 **/
gchar*
atk_text_get_text (AtkText      *text,
                   gint         start_offset,
                   gint         end_offset)
{
  AtkTextIface *iface;
  
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text)
    return (*(iface->get_text)) (text, start_offset, end_offset);
  else
    return NULL;
}

/**
 * atk_text_get_character_at_offset:
 * @text: an #AtkText
 * @offset: position
 *
 * Gets the specified text.
 *
 * Returns: the character at @offset.
 **/
gunichar
atk_text_get_character_at_offset (AtkText      *text,
                                  gint         offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), (gunichar) 0);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_at_offset)
    return (*(iface->get_character_at_offset)) (text, offset);
  else
    return (gunichar) 0;
}

/**
 * atk_text_get_text_after_offset:
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 * @start_offset: the start offset of the returned string.
 * @end_offset: the end offset of the returned string.
 *
 * Gets the specified text.
 * If the boundary type is ATK_TEXT_BOUNDARY_WORD_START or
 * ATK_TEXT_BOUNDARY_WORD_END part of a word may be returned.
 * If the boundary type is ATK_TEXT_BOUNDARY_SENTENCE_START the start point 
 * will be the offset and will continue to the start of the next sentence. 
 * The first word may not be a complete word. Similarly for 
 * ATK_TEXT_BOUNDARY_SENTENCE_END, ATK_TEXT_BOUNDARY_LINE_START and
 * ATK_TEXT_BOUNDARY_LINE_END
 *
 * Returns: the text after @offset up to the specified @boundary_type.
 **/
gchar*
atk_text_get_text_after_offset (AtkText          *text,
                                gint             offset,
                                AtkTextBoundary  boundary_type,
 				gint             *start_offset,
				gint		 *end_offset)
{
  AtkTextIface *iface;
  gint local_start_offset, local_end_offset;
  gint *real_start_offset, *real_end_offset;

  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  if (start_offset)
    real_start_offset = start_offset;
  else
    real_start_offset = &local_start_offset;
  if (end_offset)
    real_end_offset = end_offset;
  else
    real_end_offset = &local_end_offset;

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_after_offset)
    return (*(iface->get_text_after_offset)) (text, offset, boundary_type, real_start_offset, real_end_offset);
  else
    return NULL;
}

/**
 * atk_text_get_text_at_offset:
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 * @start_offset: the start offset of the returned string.
 * @end_offset: the end offset of the returned string.
 *
 * Gets the specified text.
 * If the boundary_type is ATK_TEXT_BOUNDARY_WORD_START or 
 * ATK_TEXT_BOUNDARY_WORD_END a complete word is returned; 
 * if the boundary type is  ATK_TEXT_BOUNDARY_SENTENCE_START or 
 * ATK_TEXT_BOUNDARY_SENTENCE_END a complete sentence
 * is returned; if the boundary type is ATK_TEXT_BOUNDARY_LINE_START or
 * ATK_TEXT_BOUNDARY_LINE_END a complete line is returned.
 *
 * Returns: the text at @offset up to the specified @boundary_type.
 **/
gchar*
atk_text_get_text_at_offset (AtkText          *text,
                             gint             offset,
                             AtkTextBoundary  boundary_type,
			     gint             *start_offset,
			     gint             *end_offset)
{
  AtkTextIface *iface;
  gint local_start_offset, local_end_offset;
  gint *real_start_offset, *real_end_offset;

  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  if (start_offset)
    real_start_offset = start_offset;
  else
    real_start_offset = &local_start_offset;
  if (end_offset)
    real_end_offset = end_offset;
  else
    real_end_offset = &local_end_offset;

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_at_offset)
    return (*(iface->get_text_at_offset)) (text, offset, boundary_type, real_start_offset, real_end_offset);
  else
    return NULL;
}

/**
 * atk_text_get_text_before_offset:
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 * @start_offset: the start offset of the returned string.
 * @end_offset: the end offset of the returned string.
 *
 * Gets the specified text.
 * If the boundary type is ATK_TEXT_BOUNDARY_WORD_START or
 * ATK_TEXT_BOUNDARY_WORD_END part of a word may be returned.
 * If the boundary type is ATK_TEXT_BOUNDARY_SENTENCE_START the start point 
 * will be at the start of the sentence, and will continue to the offset. 
 * The last word may not be a complete word. Similarly for 
 * ATK_TEXT_BOUNDARY_SENTENCE_END, ATK_TEXT_BOUNDARY_LINE_START and
 * ATK_TEXT_BOUNDARY_LINE_END
 *
 * Returns: the text before @offset starting from the specified @boundary_type.
 **/
gchar*
atk_text_get_text_before_offset (AtkText          *text,
                                 gint             offset,
                                 AtkTextBoundary  boundary_type,
				 gint             *start_offset,
				 gint		  *end_offset)
{
  AtkTextIface *iface;
  gint local_start_offset, local_end_offset;
  gint *real_start_offset, *real_end_offset;

  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  if (start_offset)
    real_start_offset = start_offset;
  else
    real_start_offset = &local_start_offset;
  if (end_offset)
    real_end_offset = end_offset;
  else
    real_end_offset = &local_end_offset;

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_before_offset)
    return (*(iface->get_text_before_offset)) (text, offset, boundary_type, real_start_offset, real_end_offset);
  else
    return NULL;
}

/**
 * atk_text_get_caret_offset:
 * @text: an #AtkText
 *
 * Gets the offset position of the caret (cursor).
 *
 * Returns: the offset position of the caret (cursor).
 **/
gint
atk_text_get_caret_offset (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_caret_offset)
    return (*(iface->get_caret_offset)) (text);
  else
    return -1;
}

/**
 * atk_text_get_character_extents:
 * @text: an #AtkText
 * @offset: position
 * @x: x-position of character
 * @y: y-position of character
 * @width: width of character
 * @height: height of character
 * @coords: specify whether coordinates are relative to the screen or widget window 
 *
 * Given an @offset, the @x, @y, @width, and @height values are filled
 * appropriately. 
 **/
void
atk_text_get_character_extents (AtkText *text,
                                gint offset,
                                gint *x,
                                gint *y,
                                gint *width,
                                gint *height,
			        AtkCoordType coords)
{
  AtkTextIface *iface;
  gint local_x, local_y, local_width, local_height;
  gint *real_x, *real_y, *real_width, *real_height;

  g_return_if_fail (ATK_IS_TEXT (text));

  if (x)
    real_x = x;
  else
    real_x = &local_x;
  if (y)
    real_y = y;
  else
    real_y = &local_y;
  if (width)
    real_width = width;
  else
    real_width = &local_width;
  if (height)
    real_height = height;
  else
    real_height = local_height;

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_extents)
    (*(iface->get_character_extents)) (text, offset, real_x, real_y, real_width, real_height, coords);
  else
    {
      *real_x = 0;
      *real_y = 0;
      *real_width = 0;
      *real_height = 0;
    }
}

/**
 *atk_text_ref_run_attributes:
 *@text: an #AtkText
 *@offset: the offset at which to get the attributes
 *@start_offset: the address to put the start offset of the range
 *@end_offset: the address to put the end offset of the range
 *
 *Creates an #AtkAttributeSet which consists of the attributes explicitly
 *set at the position @offset in the text. @start_offset and @end_offset are
 *set to the start and end of the range around @offset where the attributes are
 *invariant. See the ATK_ATTRIBUTE macros, such as #ATK_ATTRIBUTE_LEFT_MARGIN
 *for types of text attributes that can be returned. Note that other 
 *attributes that do not have corresponding macros may also be returned.
 *
 *Returns: an #AtkAttributeSet which contains the attributes explicitly set
 *at @offset
 **/
AtkAttributeSet* atk_text_ref_run_attributes              (AtkText          *text,
                                                           gint             offset,
                                                           gint             *start_offset,
                                                           gint             *end_offset)
{
  AtkTextIface *iface;
  gint local_start_offset, local_end_offset;
  gint *real_start_offset, *real_end_offset;

  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  if (start_offset)
    real_start_offset = start_offset;
  else
    real_start_offset = &local_start_offset;
  if (end_offset)
    real_end_offset = end_offset;
  else
    real_start_offset = &local_end_offset;

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->ref_run_attributes)
    return (*(iface->ref_run_attributes)) (text, offset, real_start_offset, real_end_offset);
  else
    return NULL;
}

/**
 * atk_text_get_character_count:
 * @text: an #AtkText
 *
 * Gets the character count.
 *
 * Returns: the number of characters.
 **/
gint
atk_text_get_character_count (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_count)
    return (*(iface->get_character_count)) (text);
  else
    return -1;
}

/**
 * atk_text_get_offset_at_point:
 * @text: an #AtkText
 * @x: screen x-position of character
 * @y: screen y-position of character
 * @coords: specify whether coordinates are relative to the screen or
 * widget window 
 *
 * Gets the offset of the character located at coordinates @x and @y. @x and @y
 * are interpreted as being relative to the screen or this widget's window
 * depending on @coords.
 *
 * Returns: the offset to the character which is located at
 * the specified @x and @y coordinates.
 **/
gint
atk_text_get_offset_at_point (AtkText *text,
                              gint x,
                              gint y,
			      AtkCoordType coords)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_offset_at_point)
    return (*(iface->get_offset_at_point)) (text, x, y, coords);
  else
    return -1;
}

/**
 * atk_text_get_n_selections:
 * @text: an #AtkText
 *
 * Gets the number of selected regions.
 *
 * Returns: The number of selected regions, or -1 if a failure
 *   occurred.
 **/
gint
atk_text_get_n_selections (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_n_selections)
    return (*(iface->get_n_selections)) (text);
  else
    return -1;
}

/**
 * atk_text_get_selection:
 * @text: an #AtkText
 * @selection_num: The selection number.  The selected regions are
 * assigned numbers that correspond to how far the region is from the
 * start of the text.  The selected region closest to the beginning
 * of the text region is assigned the number 0, etc.  Note that adding,
 * moving or deleting a selected region can change the numbering.
 * @start_offset: passes back the start position of the selected region
 * @end_offset: passes back the end position of the selected region
 *
 * Gets the text from the specified selection.
 *
 * Returns: the selected text.
 **/
gchar*
atk_text_get_selection (AtkText *text, 
                        gint    selection_num,
                        gint    *start_offset,
                        gint    *end_offset)
{
  AtkTextIface *iface;
  gint local_start_offset, local_end_offset;
  gint *real_start_offset, *real_end_offset;

  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  if (start_offset)
    real_start_offset = start_offset;
  else
    real_start_offset = &local_start_offset;
  if (end_offset)
    real_end_offset = end_offset;
  else
    real_start_offset = &local_end_offset;

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_selection)
  {
    return (*(iface->get_selection)) (text, selection_num,
       real_start_offset, real_end_offset);
  }
  else
    return NULL;
}

/**
 * atk_text_add_selection:
 * @text: an #AtkText
 * @start_offset: the start position of the selected region
 * @end_offset: the end position of the selected region
 *
 * Adds a selection bounded by the specified offsets.
 *
 * Returns: %TRUE if success, %FALSE otherwise
 **/
gboolean
atk_text_add_selection (AtkText *text, 
                        gint    start_offset,
                        gint    end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->add_selection)
    return (*(iface->add_selection)) (text, start_offset, end_offset);
  else
    return FALSE;
}

/**
 * atk_text_remove_selection:
 * @text: an #AtkText
 * @selection_num: The selection number.  The selected regions are
 * assigned numbers that correspond to how far the region is from the
 * start of the text.  The selected region closest to the beginning
 * of the text region is assigned the number 0, etc.  Note that adding,
 * moving or deleting a selected region can change the numbering.
 *
 * Removes the specified selection.
 *
 * Returns: %TRUE if success, %FALSE otherwise
 **/
gboolean
atk_text_remove_selection (AtkText *text, 
                           gint    selection_num)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->remove_selection)
    return (*(iface->remove_selection)) (text, selection_num);
  else
    return FALSE;
}

/**
 * atk_text_set_selection:
 * @text: an #AtkText
 * @selection_num: The selection number.  The selected regions are
 * assigned numbers that correspond to how far the region is from the
 * start of the text.  The selected region closest to the beginning
 * of the text region is assigned the number 0, etc.  Note that adding,
 * moving or deleting a selected region can change the numbering.
 * @start_offset: the new start position of the selection
 * @end_offset: the new end position of the selection
 *
 * Changes the start and end offset of the specified selection.
 *
 * Returns: %TRUE if success, %FALSE otherwise
 **/
gboolean
atk_text_set_selection (AtkText *text, 
                        gint    selection_num,
                        gint    start_offset, 
                        gint    end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->set_selection)
  {
    return (*(iface->set_selection)) (text, selection_num,
       start_offset, end_offset);
  }
  else
    return FALSE;
}

/**
 * atk_text_set_caret_offset:
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

/**
 * atk_attribute_set_free:
 * @attrib_set: The #AtkAttributeSet to free
 *
 * Frees the memory used by an #AtkAttributeSet, including all its
 * #AtkAttributes.
 **/
void
atk_attribute_set_free(AtkAttributeSet *attrib_set)
{
  gint index;

  if (attrib_set == NULL)
    return;

  for (index = 0; index < g_slist_length(attrib_set); index++)
  {
    g_free(((AtkAttribute*) (g_slist_nth(attrib_set,index)->data))->name);
    g_free(((AtkAttribute*) (g_slist_nth(attrib_set,index)->data))->value);
  }
  g_slist_free(attrib_set);
}
