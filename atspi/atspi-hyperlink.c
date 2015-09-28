/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#include "atspi-private.h"

G_DEFINE_TYPE (AtspiHyperlink, atspi_hyperlink, ATSPI_TYPE_OBJECT)

static void
atspi_hyperlink_init (AtspiHyperlink *hyperlink)
{
}

static void
atspi_hyperlink_class_init (AtspiHyperlinkClass *klass)
{
}

AtspiHyperlink *
_atspi_hyperlink_new (AtspiApplication *app, const gchar *path)
{
  AtspiHyperlink *hyperlink;
  
  hyperlink = g_object_new (ATSPI_TYPE_HYPERLINK, NULL);
  hyperlink->parent.app = g_object_ref (app);
  hyperlink->parent.path = g_strdup (path);

  return hyperlink;
}

/**
 * atspi_hyperlink_get_n_anchors:
 * @obj: a pointer to the #AtspiHyperlink object on which to operate.
 *
 * Gets the total number of anchors which an #AtspiHyperlink implementor has.
 * Though typical hyperlinks have only one anchor, client-side image maps and
 * other hypertext objects may potentially activate or refer to multiple
 * URIs.  For each anchor there is a corresponding URI and object.
 *
 * see: #atspi_hyperlink_get_uri and #atspi_hyperlink_get_object.
 *
 * Returns: a #gint indicating the number of anchors in this hyperlink.
 **/
gint
atspi_hyperlink_get_n_anchors (AtspiHyperlink *obj, GError **error)
{
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_get_property (obj, atspi_interface_hyperlink, "NAnchors", error, "i", &retval);

  return retval;
}

/**
 * atspi_hyperlink_get_uri:
 * @obj: a pointer to the #AtspiHyperlink implementor on which to operate.
 * @i: a (zero-index) integer indicating which hyperlink anchor to query.
 *
 * Gets the URI associated with a particular hyperlink anchor.
 *
 * Returns: a UTF-8 string giving the URI of the @ith hyperlink anchor.
 **/
gchar *
atspi_hyperlink_get_uri (AtspiHyperlink *obj, int i, GError **error)
{
  dbus_int32_t d_i = i;
  char *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_hyperlink, "GetURI", error, "i=>s", d_i, &retval);

  if (!retval)
    retval = g_strdup ("");

  return retval;
}

/**
 * atspi_hyperlink_get_object:
 * @obj: a pointer to the #AtspiHyperlink implementor on which to operate.
 * @i: a (zero-index) #gint indicating which hyperlink anchor to query.
 *
 * Gets the object associated with a particular hyperlink anchor, as an
 * #AtspiAccessible.
 *
 * Returns: (transfer full): an #AtspiAccessible that represents the object
 *        associated with the @ith anchor of the specified #AtspiHyperlink.
 **/
AtspiAccessible*
atspi_hyperlink_get_object (AtspiHyperlink *obj, gint i, GError **error)
{
  dbus_int32_t d_i = i;
  DBusMessage *reply;

  g_return_val_if_fail (obj != NULL, NULL);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_hyperlink, "GetObject", error, "i", d_i);

  return _atspi_dbus_return_accessible_from_message (reply);
}

/**
 * atspi_hyperlink_get_index_range:
 * @obj: a pointer to the #AtspiHyperlink implementor on which to operate.
 *
 *
 * Gets the starting and ending character offsets of the text range
 * associated with an #AtspiHyperlink, in its originating #AtspiHypertext.
 **/
AtspiRange *
atspi_hyperlink_get_index_range (AtspiHyperlink *obj, GError **error)
{
  dbus_int32_t d_start_offset = -1;
  dbus_int32_t d_end_offset = -1;
  AtspiRange *ret = g_new (AtspiRange, 1);

  ret->start_offset = ret->end_offset = -1;

  if (!obj)
    return ret;

  _atspi_dbus_call (obj, atspi_interface_hyperlink, "GetIndexRange", error, "=>ii", &d_start_offset, &d_end_offset);

  ret->start_offset = d_start_offset;
  ret->end_offset = d_end_offset;
  return ret;
}

/**
 * atspi_hyperlink_get_start_index:
 * @obj: a pointer to the #AtspiHyperlink implementor on which to operate.
 *
 *
 * Gets the starting character offset of the text range associated with
 *       an #AtspiHyperlink, in its originating #AtspiHypertext.
 **/
gint
atspi_hyperlink_get_start_index (AtspiHyperlink *obj, GError **error)
{
  dbus_int32_t d_start_offset = -1;

  if (!obj)
    return -1;

  _atspi_dbus_get_property (obj, atspi_interface_hyperlink, "StartIndex",
                            error, "i", &d_start_offset);

  return d_start_offset;
}
/**
 * atspi_hyperlink_get_end_index:
 * @obj: a pointer to the #AtspiHyperlink implementor on which to operate.
 *
 *
 * Gets the ending character offset of the text range associated with
 *       an #AtspiHyperlink, in its originating #AtspiHypertext.
 **/
gint
atspi_hyperlink_get_end_index (AtspiHyperlink *obj, GError **error)
{
  dbus_int32_t d_end_offset = -1;

  if (!obj)
    return -1;

  _atspi_dbus_get_property (obj, atspi_interface_hyperlink, "EndIndex", error,
                            "i", &d_end_offset);

  return d_end_offset;
}


/**
 * atspi_hyperlink_is_valid:
 * @obj: a pointer to the #AtspiHyperlink on which to operate.
 *
 * Tells whether an #AtspiHyperlink object is still valid with respect to its
 *          originating hypertext object.
 *
 * Returns: #TRUE if the specified #AtspiHyperlink is still valid with respect
 *          to its originating #AtspiHypertext object, #FALSE otherwise.
 **/
gboolean
atspi_hyperlink_is_valid (AtspiHyperlink *obj, GError **error)
{
  dbus_bool_t retval = FALSE;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_hyperlink, "IsValid", error, "=>b", &retval);

  return retval;
}
