/* ATK -  Accessibility Toolkit
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

#pragma once

#if defined(ATK_DISABLE_SINGLE_INCLUDES) && !defined(__ATK_H_INSIDE__) && !defined(ATK_COMPILATION)
#error "Only <atk/atk.h> can be included directly."
#endif

#include <atk/atkobject.h>
#include <atk/atkutil.h>

G_BEGIN_DECLS

/*
 * The AtkDocument interface should be supported by any object that is a container
 * for 'document content' as opposed to a collection of user interface elements.
 *
 */

#define ATK_TYPE_DOCUMENT (atk_document_get_type ())
#define ATK_IS_DOCUMENT(obj) G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_DOCUMENT)
#define ATK_DOCUMENT(obj) G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_DOCUMENT, AtkDocument)
#define ATK_DOCUMENT_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATK_TYPE_DOCUMENT, AtkDocumentIface))

/**
 * AtkTextSelection:
 * @start_object: the AtkText containing the start of the selection.
 * @start_offset: the text offset of the beginning of the selection within
 *                @start_object.
 * @end_object: the AtkText containing the end of the selection.
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
typedef struct _AtkTextSelection AtkTextSelection;
struct _AtkTextSelection
{
  AtkObject *start_object;
  gint start_offset;
  AtkObject *end_object;
  gint end_offset;
  gboolean start_is_active;
};

#ifndef _TYPEDEF_ATK_DOCUMENT_
#define _TYPEDEF_ATK_DOCUMENT_
typedef struct _AtkDocument AtkDocument;
#endif
typedef struct _AtkDocumentIface AtkDocumentIface;

/**
 * AtkDocumentIface:
 * @get_document_type: gets a string indicating the document
 *   type. This virtual function is deprecated since 2.12 and it
 *   should not be overriden.
 * @get_document: a #GObject instance that implements
 *   AtkDocumentIface. This virtual method is deprecated since 2.12
 *   and it should not be overriden.
 * @get_document_locale: gets locale. This virtual function is
 *   deprecated since 2.7.90 and it should not be overriden.
 * @get_document_attributes: gets an AtkAttributeSet which describes
 *   document-wide attributes as name-value pairs.
 * @get_document_attribute_value: returns a string value assocciated
 *   with the named attribute for this document, or NULL
 * @set_document_attribute: sets the value of an attribute. Returns
 *   TRUE on success, FALSE otherwise
 * @get_current_page_number: gets the current page number. Since 2.12
 * @get_page_count: gets the page count of the document. Since 2.12
 */
struct _AtkDocumentIface
{
  GTypeInterface parent;
  const gchar *(*get_document_type) (AtkDocument *document);
  gpointer (*get_document) (AtkDocument *document);

  const gchar *(*get_document_locale) (AtkDocument *document);
  AtkAttributeSet *(*get_document_attributes) (AtkDocument *document);
  const gchar *(*get_document_attribute_value) (AtkDocument *document,
                                                const gchar *attribute_name);
  gboolean (*set_document_attribute) (AtkDocument *document,
                                      const gchar *attribute_name,
                                      const gchar *attribute_value);
  gint (*get_current_page_number) (AtkDocument *document);
  gint (*get_page_count) (AtkDocument *document);
  GArray *(*get_text_selections) (AtkDocument *document);
  gboolean (*set_text_selections) (AtkDocument *document, GArray *selections);
};

ATK_AVAILABLE_IN_ALL
GType atk_document_get_type (void);

ATK_DEPRECATED_IN_2_12
const gchar *atk_document_get_document_type (AtkDocument *document);

ATK_DEPRECATED_IN_2_12
gpointer atk_document_get_document (AtkDocument *document);

ATK_DEPRECATED_IN_2_8_FOR (atk_object_get_object_locale)
const gchar *atk_document_get_locale (AtkDocument *document);

ATK_AVAILABLE_IN_ALL
AtkAttributeSet *atk_document_get_attributes (AtkDocument *document);
ATK_AVAILABLE_IN_ALL
const gchar *atk_document_get_attribute_value (AtkDocument *document,
                                               const gchar *attribute_name);
ATK_AVAILABLE_IN_ALL
gboolean atk_document_set_attribute_value (AtkDocument *document,
                                           const gchar *attribute_name,
                                           const gchar *attribute_value);
ATK_AVAILABLE_IN_2_12
gint atk_document_get_current_page_number (AtkDocument *document);
ATK_AVAILABLE_IN_2_12
gint atk_document_get_page_count (AtkDocument *document);

G_END_DECLS

ATK_AVAILABLE_IN_2_52
GArray *atk_document_get_text_selections (AtkDocument *document);

ATK_AVAILABLE_IN_2_52
gboolean atk_document_set_text_selections (AtkDocument *document, GArray *selections);
