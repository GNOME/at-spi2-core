/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 *
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

#pragma once

#include "glib-object.h"

#include "atspi-constants.h"

#include "atspi-types.h"

G_BEGIN_DECLS

#define ATSPI_TYPE_DOCUMENT (atspi_document_get_type ())
#define ATSPI_IS_DOCUMENT(obj) G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATSPI_TYPE_DOCUMENT)
#define ATSPI_DOCUMENT(obj) G_TYPE_CHECK_INSTANCE_CAST ((obj), ATSPI_TYPE_DOCUMENT, AtspiDocument)
#define ATSPI_DOCUMENT_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATSPI_TYPE_DOCUMENT, AtspiDocument))

GType atspi_document_get_type ();

/**
 * AtspiTextSelection:
 * @start_object: the AtspiAccessible containing the start of the selection.
 * @start_offset: the text offset of the beginning of the selection within
 *                @start_object.
 * @end_object: the AtspiAccessible containing the end of the selection.
 * @end_offset: the text offset of the end of the selection within @end_object.
 * @start_is_active: a gboolean indicating whether the start of the selection
 *                  is the active point.
 *
 * This structure represents a single  text selection within a document. This
 * selection is defined by two points in the content, where each one is defined
 * by an AtkObject supporting the AtkText interface and a character offset
 * relative to it.
 *
 * The end object must appear after the start object in the accessibility tree,
 * i.e. the end object must be reachable from the start object by navigating
 * forward (next, first child etc).
 *
 * This struct also contains a @start_is_active boolean, to communicate if the
 * start of the selection is the active point or not.
 *
 * The active point corresponds to the user's focus or point of interest. The
 * user moves the active point to expand or collapse the range. The anchor
 * point is the other point of the range and typically remains constant. In
 * most cases, anchor is the start of the range and active is the end. However,
 * when selecting backwards (e.g. pressing shift+left arrow in a text field),
 * the start of the range is the active point, as the user moves this to
 * manipulate the selection.
 *
 * Since: 2.52
 */
typedef struct _AtspiTextSelection AtspiTextSelection;
struct _AtspiTextSelection
{
  AtspiAccessible *start_object;
  gint start_offset;
  AtspiAccessible *end_object;
  gint end_offset;
  gboolean start_is_active;
};

struct _AtspiDocument
{
  GTypeInterface parent;
};

gchar *atspi_document_get_locale (AtspiDocument *obj, GError **error);

#ifndef ATSPI_DISABLE_DEPRECATED
gchar *atspi_document_get_attribute_value (AtspiDocument *obj, const gchar *attribute, GError **error);
#endif

gchar *atspi_document_get_document_attribute_value (AtspiDocument *obj, const gchar *attribute, GError **error);

#ifndef ATSPI_DISABLE_DEPRECATED
GHashTable *atspi_document_get_attributes (AtspiDocument *obj, GError **error);
#endif

GHashTable *atspi_document_get_document_attributes (AtspiDocument *obj, GError **error);

gint atspi_document_get_page_count (AtspiDocument *obj, GError **error);
gint atspi_document_get_current_page_number (AtspiDocument *obj, GError **error);

GArray *atspi_document_get_text_selections (AtspiDocument *document, GError **error);

gboolean atspi_document_set_text_selections (AtspiDocument *document, GArray *selections, GError **error);
G_END_DECLS
