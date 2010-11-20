/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 *           
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

#ifndef _ATSPI_TEXT_H_
#define _ATSPI_TEXT_H_

#include "glib-object.h"

#include "atspi-constants.h"

#include "atspi-types.h"

typedef struct _AtspiRange AtspiRange;
struct _AtspiRange
{
  gint start_offset;
  gint end_offset;
};

/**
 * ATSPI_TYPE_RANGE:
 * 
 * The #GType for a boxed type holding a range within a text bock.
 */
#define	ATSPI_TYPE_RANGE atspi_range_get_type ()

GType atspi_range_get_type ();

AtspiRange *
atspi_range_copy (AtspiRange *src);

typedef struct _AtspiRangedAttributeSet AtspiRangedAttributeSet;
struct _AtspiRangedAttributeSet
{
  GHashTable *attributes;
  gint start_offset;
  gint end_offset;
};

/**
 * ATSPI_TYPE_RANGED_ATTRIBUTE_SET:
 * 
 * The #GType for a boxed type holding an attribute set within a text block.
 */
#define	ATSPI_TYPE_RANGED_ATTRIBUTE_SET atspi_ranged_attribute_set_get_type ()

GType atspi_ranged_attribute_set_get_type ();

typedef struct _AtspiTextRange AtspiTextRange;
struct _AtspiTextRange
{
  gint start_offset;
  gint end_offset;
  gchar *text;
};

/**
 * ATSPI_TYPE_TEXT_RANGE:
 * 
 * The #GType for a boxed type holding a range within a text bock.
 */
#define	ATSPI_TYPE_TEXT_RANGE atspi_text_range_get_type ()

#define ATSPI_TYPE_TEXT                    (atspi_text_get_type ())
#define ATSPI_IS_TEXT(obj)                 G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATSPI_TYPE_TEXT)
#define ATSPI_TEXT(obj)                    G_TYPE_CHECK_INSTANCE_CAST ((obj), ATSPI_TYPE_TEXT, AtspiText)
#define ATSPI_TEXT_GET_IFACE(obj)          (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATSPI_TYPE_TEXT, AtspiText))

GType atspi_text_get_type ();

struct _AtspiText
{
  GTypeInterface parent;
};


GType atspi_text_range_get_type ();

gint atspi_text_get_character_count (AtspiText *obj, GError **error);

gchar * atspi_text_get_text (AtspiText *obj, gint start_offset, gint end_offset, GError **error);

gint atspi_text_get_caret_offset (AtspiText *obj, GError **error);

AtspiRangedAttributeSet * atspi_text_get_attributes (AtspiText *obj, gint offset, GError **error);

AtspiRangedAttributeSet * atspi_text_get_attribute_run (AtspiText *obj, gint offset, gboolean include_defaults, GError **error);

gchar * atspi_text_get_attribute_value (AtspiText *obj, gint offset, gchar *attribute_name, GError **error);

GHashTable * atspi_text_get_default_attributes (AtspiAccessible *obj, GError **error);

gboolean atspi_text_set_caret_offset (AtspiText *obj, gint new_offset, GError **error);

AtspiTextRange * atspi_text_get_text_before_offset (AtspiText *obj, gint offset, AtspiTextBoundaryType type, GError **error);

AtspiTextRange * atspi_text_get_text_at_offset (AtspiText *obj, gint offset, AtspiTextBoundaryType type, GError **error);

AtspiTextRange * atspi_text_get_text_after_offset (AtspiText *obj, gint offset, AtspiTextBoundaryType type, GError **error);

guint atspi_text_get_character_at_offset (AtspiText *obj, gint offset, GError **error);

AtspiRect * atspi_text_get_character_extents (AtspiText *obj, gint offset, AtspiCoordType type, GError **error);

gint atspi_text_get_offset_at_point (AtspiText *obj, gint x, gint y, AtspiCoordType type, GError **error);

AtspiRect * atspi_text_get_range_extents (AtspiText *obj, gint start_offset, gint end_offset, AtspiCoordType type, GError **error);

GArray * atspi_text_get_bounded_ranges (AtspiText *obj, gint x, gint y, gint width, gint height, AtspiCoordType type, AtspiTextClipType clipTypeX, AtspiTextClipType clipTypeY, GError **error);

gint atspi_text_get_n_selections (AtspiText *obj, GError **error);

AtspiRange * atspi_text_get_selection (AtspiText *obj, gint selection_num, GError **error);

gboolean atspi_text_add_selection (AtspiText *obj, gint start_offset, gint end_offset, GError **error);

gboolean atspi_text_remove_selection (AtspiText *obj, gint selection_num, GError **error);

gboolean atspi_text_set_selection (AtspiText *obj, gint selection_num, gint start_offset, gint end_offset, GError **error);
#endif	/* _ATSPI_TEXT_H_ */
