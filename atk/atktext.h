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

#include <glib-object.h>
#include <atk/atkobject.h>
#include <atk/atkutil.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * AtkAttributeSet:
 *
 * This is a singly-linked list (a #GSList) of #AtkAttribute. It is
 * used by atk_text_ref_run_attributes() and atk_editable_text_set_run_attributes()
 **/
typedef GSList AtkAttributeSet;

/**
 * AtkAttribute:
 * @name: The attribute name. See the ATK_ATTRIBUTE macros, eg #ATK_ATTRIBUTE_LEFT_MARGIN for examples.
 * @value: the value of the attribute, represented as a string. See the ATK_ATTRIBUTE macros, eg #ATK_ATTRIBUTE_LEFT_MARGIN for example.
 *
 * A string name/value pair representing a text attribute. 
 **/
typedef struct _AtkAttribute AtkAttribute;

struct _AtkAttribute {
  gchar* name;
  gchar* value;
};

/**
 * ATK_ATTRIBUTE_LEFT_MARGIN:
 *
 * An #AtkAttribute name/value pair. The pixel width of the left margin
 **/
#define ATK_ATTRIBUTE_LEFT_MARGIN        "left_margin"

/**
 * ATK_ATTRIBUTE_RIGHT_MARGIN:
 *
 * An #AtkAttribute name/value pair. The pixel width of the right margin
 **/
#define ATK_ATTRIBUTE_RIGHT_MARGIN        "right_margin"

/**
 * ATK_ATTRIBUTE_INDENT:
 *
 * An #AtkAttribute name/value pair. The number of pixels that the text is indented
 **/
#define ATK_ATTRIBUTE_INDENT              "indent"

/**
 * ATK_ATTRIBUTE_INVISIBLE:
 *
 * An #AtkAttribute name/value pair. 
 * Either "true" or "false" indicating whether text is visible or not
 **/
#define ATK_ATTRIBUTE_INVISIBLE          "invisible"

/**
 * ATK_ATTRIBUTE_EDITABLE:
 *
 * An #AtkAttribute name/value pair. 
 * Either "true" or "false" indicating whether text is editable or not
 **/
#define ATK_ATTRIBUTE_EDITABLE           "editable"

/**
 * ATK_ATTRIBUTE_PIXELS_ABOVE_LINES:
 *
 * An #AtkAttribute name/value pair.
 * Pixels of blank space to leave above each newline-terminated line. 
 **/
#define ATK_ATTRIBUTE_PIXELS_ABOVE_LINES "pixels_above_lines"

/**
 * ATK_ATTRIBUTE_PIXELS_BELOW_LINES:
 *
 * An #AtkAttribute name/value pair. 
 * Pixels of blank space to leave below each newline-terminated line.
 **/
#define ATK_ATTRIBUTE_PIXELS_BELOW_LINES "pixels_below_lines"

/**
 * ATK_ATTRIBUTE_PIXELS_INSIDE_WRAP:
 *
 * An #AtkAttribute name/value pair.
 * Pixels of blank space to leave between wrapped lines inside the same newline-terminated line (paragraph).
 **/
#define ATK_ATTRIBUTE_PIXELS_INSIDE_WRAP "pixels_inside_wrap"

/**
 * ATK_ATTRIBUTE_BG_FULL_HEIGHT:
 *
 * An #AtkAttribute name/value pair. 
 * "true" or "false" whether to make the background color for each character the height of the highest font used on the current line, or the height of the font used for the current character.
 **/
#define ATK_ATTRIBUTE_BG_FULL_HEIGHT     "bg_full_height"

/**
 * ATK_ATTRIBUTE_RISE:
 *
 * An #AtkAttribute name/value pair. 
 * Number of pixels that the characters are risen above the baseline
 * The value is a string representation of an integer 
 **/
#define ATK_ATTRIBUTE_RISE               "rise"

/**
 * ATK_ATTRIBUTE_UNDERLINE:
 *
 * An #AtkAttribute name/value pair. 
 * "true" or "false" whether the text is underlined
 **/
#define ATK_ATTRIBUTE_UNDERLINE          "underline"

/**
 * ATK_ATTRIBUTE_STRIKETHROUGH:
 *
 * An #AtkAttribute name/value pair. 
 * "true" or "false" whether the text is strikethrough 
 **/
#define ATK_ATTRIBUTE_STRIKETHROUGH      "strikethrough"

/**
 * ATK_ATTRIBUTE_SIZE:
 *
 * An #AtkAttribute name/value pair. 
 * The size of the characters. 
 * The value is a string representation of an integer 
 **/
#define ATK_ATTRIBUTE_SIZE		 "size"

/**
 * ATK_ATTRIBUTE_SCALE:
 *
 * An #AtkAttribute name/value pair. 
 * The scale of the characters. The value is a string representation of a double 
 **/
#define ATK_ATTRIBUTE_SCALE		 "scale"

/**
 * ATK_ATTRIBUTE_WEIGHT:
 *
 * An #AtkAttribute name/value pair. 
 * The weight of the characters. The value is a string representation of an integer 
 **/
#define ATK_ATTRIBUTE_WEIGHT		 "weight"

/**
 * ATK_ATTRIBUTE_LANGUAGE:
 *
 * An #AtkAttribute name/value pair. 
 * The language used
 **/
#define ATK_ATTRIBUTE_LANGUAGE		 "language"

/**
 * ATK_ATTRIBUTE_FAMILY_NAME:
 *
 * An #AtkAttribute name/value pair. 
 * The font family name
 **/
#define ATK_ATTRIBUTE_FAMILY_NAME	 "family_name"

/**
 * ATK_ATTRIBUTE_BG_COLOR:
 *
 * An #AtkAttribute name/value pair. 
 * The background color. The value is an RGB value of the format "%u,%u,%u"
 **/
#define ATK_ATTRIBUTE_BG_COLOR   	 "bg_color"

/**
 * ATK_ATTRIBUTE_FG_COLOR:
 *
 * An #AtkAttribute name/value pair. 
 * The foreground color. The value is an RGB value of the format "%u,%u,%u"
 **/
#define ATK_ATTRIBUTE_FG_COLOR   	 "fg_color"

/**
 * ATK_ATTRIBUTE_BG_STIPPLE:
 *
 * An #AtkAttribute name/value pair. 
 * "true" if a #GdkBitmap is set for stippling the background color.
 **/
#define ATK_ATTRIBUTE_BG_STIPPLE   	 "bg_stipple"

/**
 * ATK_ATTRIBUTE_FG_STIPPLE:
 *
 * An #AtkAttribute name/value pair. 
 * "true" if a #GdkBitmap is set for stippling the foreground color.
 **/
#define ATK_ATTRIBUTE_FG_STIPPLE   	 "fg_stipple"

/**
 * ATK_ATTRIBUTE_WRAP_MODE:
 *
 * An #AtkAttribute name/value pair. 
 * The wrap mode of the text, if any. Values are "none", "char" or "word" 
 **/
#define ATK_ATTRIBUTE_WRAP_MODE   	 "wrap_mode"

/**
 * ATK_ATTRIBUTE_DIRECTION:
 *
 * An #AtkAttribute name/value pair. 
 * The direction of the text, if set. Values are "none", "ltr" or "rtl" 
 **/
#define ATK_ATTRIBUTE_DIRECTION   	 "direction"

/**
 * ATK_ATTRIBUTE_JUSTIFICATION:
 *
 * An #AtkAttribute name/value pair. 
 * The justification of the text, if set. Values are "left", "right", "center" or "fill" 
 **/
#define ATK_ATTRIBUTE_JUSTIFICATION   	 "justification"

/**
 * ATK_ATTRIBUTE_STRETCH:
 *
 * An #AtkAttribute name/value pair. 
 * The stretch of the text, if set. Values are "ultra_condensed", "extra_condensed",
 * or "ultra_expanded"
 **/
#define ATK_ATTRIBUTE_STRETCH   	 "stretch"

/**
 * ATK_ATTRIBUTE_VARIANT:
 *
 * An #AtkAttribute name/value pair. 
 * The capitalization variant of the text, if set. Values are "normal" or "small_caps"
 **/
#define ATK_ATTRIBUTE_VARIANT   	 "variant"

/**
 * ATK_ATTRIBUTE_STYLE:
 *
 * An #AtkAttribute name/value pair. 
 * The slant style of the text, if set. Values are "normal", "oblique" or "italic"
 **/
#define ATK_ATTRIBUTE_STYLE      	 "slant_style"

#define ATK_TYPE_TEXT                    (atk_text_get_type ())
#define ATK_IS_TEXT(obj)                 G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_TEXT)
#define ATK_TEXT(obj)                    G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_TEXT, AtkText)
#define ATK_TEXT_GET_IFACE(obj)          (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATK_TYPE_TEXT, AtkTextIface))

#ifndef _TYPEDEF_ATK_TEXT_
#define _TYPEDEF_ATK_TEXT_
typedef struct _AtkText AtkText;
#endif
typedef struct _AtkTextIface AtkTextIface;

/**
 *AtkTextBoundary:
 *@ATK_TEXT_BOUNDARY_CHAR:
 *@ATK_TEXT_BOUNDARY_WORD_START:
 *@ATK_TEXT_BOUNDARY_WORD_END:
 *@ATK_TEXT_BOUNDARY_SENTENCE_START:
 *@ATK_TEXT_BOUNDARY_SENTENCE_END:
 *@ATK_TEXT_BOUNDARY_LINE_START:
 *@ATK_TEXT_BOUNDARY_LINE_END:
 *
 *Text boundary types used for specifying boundaries for regions of text
 **/
typedef enum {
  ATK_TEXT_BOUNDARY_CHAR,
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

  gchar*         (* get_text)                     (AtkText          *text,
                                                   gint             start_offset,
                                                   gint             end_offset);
  gchar*         (* get_text_after_offset)        (AtkText          *text,
                                                   gint             offset,
                                                   AtkTextBoundary  boundary_type,
						   gint             *startOffset,
						   gint             *endOffset);
  gchar*         (* get_text_at_offset)           (AtkText          *text,
                                                   gint             offset,
                                                   AtkTextBoundary  boundary_type,
						   gint             *startOffset,
						   gint             *endOffset);
  gunichar       (* get_character_at_offset)      (AtkText          *text,
                                                   gint             offset);
  gchar*         (* get_text_before_offset)       (AtkText          *text,
                                                   gint             offset,
                                                   AtkTextBoundary  boundary_type,
 						   gint             *startOffset,
						   gint             *endOffset);
  gint           (* get_caret_offset)             (AtkText          *text);
  AtkAttributeSet* (* ref_run_attributes)         (AtkText	    *text,
						   gint	  	    offset,
						   gint             *start_offset,
						   gint	 	    *end_offset);
  void           (* get_character_extents)        (AtkText          *text,
                                                   gint             offset,
                                                   gint             *x,
                                                   gint             *y,
                                                   gint             *length,
                                                   gint             *width,
                                                   AtkCoordType	    coords);
  gint           (* get_character_count)          (AtkText          *text);
  gint           (* get_offset_at_point)          (AtkText          *text,
                                                   gint             x,
                                                   gint             y,
                                                   AtkCoordType	    coords);
  gint		 (* get_n_selections)		  (AtkText          *text);
  gchar*         (* get_selection)	          (AtkText          *text,
						   gint		    selection_num,
						   gint		    *start_offset,
						   gint		    *end_offset);
  gboolean       (* add_selection)		  (AtkText          *text,
						   gint		    start_offset,
						   gint		    end_offset);
  gboolean       (* remove_selection)		  (AtkText          *text,
						   gint             selection_num);
  gboolean       (* set_selection)		  (AtkText          *text,
						   gint		    selection_num,
						   gint		    start_offset,
						   gint		    end_offset);
  gboolean       (* set_caret_offset)             (AtkText          *text,
                                                   gint             offset);

  /*
   * signal handlers
   */
  void		 (* text_changed)                 (AtkText          *text);
  void           (* caret_changed)                (AtkText          *text,
                                                   gint             location);
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
                                                           AtkTextBoundary  boundary_type,
							   gint             *startOffset,
							   gint	            *endOffset);
gchar*        atk_text_get_text_at_offset                 (AtkText          *text,
                                                           gint             offset,
                                                           AtkTextBoundary  boundary_type,
							   gint             *startOffset,
							   gint             *endOffset);
gchar*        atk_text_get_text_before_offset             (AtkText          *text,
                                                           gint             offset,
                                                           AtkTextBoundary  boundary_type,
							   gint             *startOffset,
							   gint	            *endOffset);
gint          atk_text_get_caret_offset                   (AtkText          *text);
void          atk_text_get_character_extents              (AtkText          *text,
                                                           gint             offset,
                                                           gint             *x,
                                                           gint             *y,
                                                           gint             *length,
                                                           gint             *width,
                                                           AtkCoordType	    coords);
AtkAttributeSet* atk_text_ref_run_attributes              (AtkText	    *text,
						           gint	  	    offset,
						           gint             *start_offset,
						           gint	 	    *end_offset);
gint          atk_text_get_character_count                (AtkText          *text);
gint          atk_text_get_offset_at_point                (AtkText          *text,
                                                           gint             x,
                                                           gint             y,
                                                           AtkCoordType	    coords);
gint          atk_text_get_n_selections			  (AtkText          *text);
gchar*        atk_text_get_selection			  (AtkText          *text,
							   gint		    selection_num,
							   gint             *start_offset,
							   gint             *end_offset);
gboolean      atk_text_add_selection                      (AtkText          *text,
							   gint             start_offset,
							   gint             end_offset);
gboolean      atk_text_remove_selection                   (AtkText          *text,
							   gint		    selection_num);
gboolean      atk_text_set_selection                      (AtkText          *text,
							   gint		    selection_num,
							   gint             start_offset,
							   gint             end_offset);
gboolean      atk_text_set_caret_offset                   (AtkText          *text,
                                                           gint             offset);
void 	      AtkAttributeSet_free	                  (AtkAttributeSet  *attrib_set);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_TEXT_H__ */
