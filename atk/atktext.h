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


#ifndef __ATK_TEXT_H__
#define __ATK_TEXT_H__

#include <pango/pango.h>
#include <glib-object.h>
#include <atk/atkobject.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ATK_TYPE_TEXT                    (atk_text_get_type ())
#define ATK_IS_TEXT(obj)                 G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_TEXT)
#define ATK_TEXT(obj)                    G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_TEXT, AtkText)
#define ATK_TEXT_GET_IFACE(obj)          (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATK_TYPE_TEXT, AtkTextIface))

#ifndef _TYPEDEF_ATK_TEXT_
#define _TYPEDEF_ATK_TEXT_
typedef struct _AtkText AtkText;
#endif

typedef enum {
  ATK_TEXT_BOUNDARY_CHAR,
  ATK_TEXT_BOUNDARY_CURSOR_POS,
  ATK_TEXT_BOUNDARY_WORD_START,
  ATK_TEXT_BOUNDARY_WORD_END,
  ATK_TEXT_BOUNDARY_SENTENCE_START,
  ATK_TEXT_BOUNDARY_SENTENCE_END,
  ATK_TEXT_BOUNDARY_LINE_START,
  ATK_TEXT_BOUNDARY_LINE_END
} AtkTextBoundary;

struct _AtkTextIface
{
  GTypeInterface parent;

  gchar* (* get_text)                     (AtkText          *text,
                                           gint             start_offset,
                                           gint             end_offset);
  gchar* (* get_text_after_offset)        (AtkText          *text,
                                           gint             offset,
                                           AtkTextBoundary  boundary_type);
  gchar* (* get_text_at_offset)           (AtkText          *text,
                                           gint             offset,
                                           AtkTextBoundary  boundary_type);
  gunichar (* get_character_at_offset)    (AtkText          *text,
                                           gint             offset);
  gchar* (* get_text_before_offset)       (AtkText          *text,
                                           gint             offset,
                                           AtkTextBoundary  boundary_type);
  gint   (* get_caret_offset)             (AtkText          *text);
  void   (* get_row_col_at_offset)        (AtkText          *text,
                                           gint             offset,
                                           gint             *row,
                                           gint             *col);
  PangoAttrList* (* get_range_attributes) (AtkText          *text,
                                           gint             start_offset,
                                           gint             end_offset);
  void   (* get_character_extents)        (AtkText          *text,
                                           gint             offset,
                                           gint             *x,
                                           gint             *y,
                                           gint             *length,
                                           gint             *width);
  gint   (* get_character_count)          (AtkText          *text);
  gint   (* get_offset_at_point)          (AtkText          *text,
                                           gint             x,
                                           gint             y);
  gchar* (* get_selected_text)            (AtkText          *text);
  gboolean (* get_selection_bounds)       (AtkText          *text,
                                           gint             *start_offset,
                                           gint             *end_offset);
  void     (*  set_selection_bounds)      (AtkText         *text,
                                           gint            start_offset,
                                           gint            end_offset);

};
GType            atk_text_get_type (void);


/*
 * Additional AtkObject properties used by AtkText:
 *    "accessible_text" (accessible text has changed)
 *    "accessible_caret" (accessible text cursor position changed:
 *                         editable text only)
 */

gchar*        atk_text_get_text                           (AtkText          *text,
                                                           gint             start_offset,
                                                           gint             end_offset);

gunichar      atk_text_get_character_at_offset            (AtkText          *text,
                                                           gint             offset);

gchar*        atk_text_get_text_after_offset              (AtkText          *text,
                                                           gint             offset,
                                                           AtkTextBoundary  boundary_type);
gchar*        atk_text_get_text_at_offset                 (AtkText          *text,
                                                           gint             offset,
                                                           AtkTextBoundary  boundary_type);
gchar*        atk_text_get_text_before_offset             (AtkText          *text,
                                                           gint             offset,
                                                           AtkTextBoundary  boundary_type);
gint          atk_text_get_caret_offset                   (AtkText          *text);
void          atk_text_get_row_col_at_offset              (AtkText          *text,
                                                           gint             offset,
                                                           gint             *row,
                                                           gint             *col);
PangoAttrList* atk_text_get_range_attributes              (AtkText          *text,
                                                           gint             start_offset,
                                                           gint             end_offset);
void          atk_text_get_character_extents              (AtkText          *text,
                                                           gint             offset,
                                                           gint             *x,
                                                           gint             *y,
                                                           gint             *length,
                                                           gint             *width);
gint          atk_text_get_character_count                (AtkText          *text);
gint          atk_text_get_offset_at_point                (AtkText          *text,
                                                           gint             x,
                                                           gint             y);
gchar*        atk_text_get_selected_text                  (AtkText          *text);
gboolean      atk_text_get_selection_bounds               (AtkText          *text,
                                                           gint             *start_offset,
                                                           gint             *end_offset);
void          atk_text_set_selection_bounds               (AtkText          *text,
                                                           gint             start_offset,
                                                           gint             end_offset);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_TEXT_H__ */
