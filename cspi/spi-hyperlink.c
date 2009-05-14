/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <cspi/spi-private.h>

/**
 * AccessibleHyperlink_ref:
 * @obj: a pointer to the #AccessibleHyperlink object on which to operate.
 *
 * Increment the reference count for an #AccessibleHyperlink object.
 **/
void
AccessibleHyperlink_ref (AccessibleHyperlink *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleHyperlink_unref:
 * @obj: a pointer to the #AccessibleHyperlink object on which to operate.
 *
 * Decrement the reference count for an #AccessibleHyperlink object.
 **/
void
AccessibleHyperlink_unref (AccessibleHyperlink *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleHyperlink_getNAnchors:
 * @obj: a pointer to the #AccessibleHyperlink object on which to operate.
 *
 * Get the total number of anchors which an #AccessibleHyperlink implementor has.
 *       Though typical hyperlinks have only one anchor, client-side image maps and
 *       other hypertext objects may potentially activate or refer to multiple
 *       URIs.  For each anchor there is a corresponding URI and object.
 * @see AccessibleHyperlink_getURI() and AccessibleHyperlink_getObject().
 *
 * Returns: a #long indicating the number of anchors in this hyperlink.
 **/
long
AccessibleHyperlink_getNAnchors (AccessibleHyperlink *obj)
{
  dbus_int16_t retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  cspi_dbus_get_property (obj, spi_interface_hyperlink, "nAnchors", NULL, "n", &retval);

  cspi_return_val_if_ev ("getNAnchors", -1);

  return retval;
}

/**
 * AccessibleHyperlink_getURI:
 * @obj: a pointer to the #AccessibleHyperlink implementor on which to operate.
 * @i: a (zero-index) long integer indicating which hyperlink anchor to query.
 *
 * Get the URI associated with a particular hyperlink anchor.  
 *
 * Returns: a UTF-8 string giving the URI of the @ith hyperlink anchor.
 **/
char *
AccessibleHyperlink_getURI (AccessibleHyperlink *obj,
                            long int             i)
{
  dbus_int32_t d_i = i;
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_hyperlink, "getURI", NULL, "i=>s", d_i, &retval);

  cspi_return_val_if_ev ("getURI", NULL);

  return retval;
}

/**
 * AccessibleHyperlink_getObject:
 * @obj: a pointer to the #AccessibleHyperlink implementor on which to operate.
 * @i: a (zero-index) long integer indicating which hyperlink anchor to query.
 *
 * Get the object associated with a particular hyperlink anchor, as an #Accessible. 
 *
 * Returns: an #Accessible that represents the object associated with the @ith anchor
 *        of the specified #AccessibleHyperlink.
 **/
Accessible*
AccessibleHyperlink_getObject (AccessibleHyperlink *obj,
                               long int             i)
{
  dbus_int32_t d_i = i;
  char *path;
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_hyperlink, "getObject", NULL, "i=>o", d_i, &path);
  retval = cspi_ref_related_accessible (obj, path);
  g_free (path);
  return retval;
}

/**
 * AccessibleHyperlink_getIndexRange:
 * @obj: a pointer to the #AccessibleHyperlink implementor on which to operate.
 * @startIndex: a pointer to a long integer into which the starting
 *       offset of the text associated with this #AccessibleHyperlink is returned.
 * @endIndex: a pointer to a long integer into which the offset of the first character
 *       after the text associated with this #AccessibleHyperlink is returned.
 *
 *
 * Get the starting and ending character offsets of the text range associated with
 *       a #AccessibleHyperlink, in its originating #AccessibleHypertext.
 **/
void
AccessibleHyperlink_getIndexRange (AccessibleHyperlink *obj,
                                   long int *startIndex,
                                   long int *endIndex)
{
  dbus_int32_t si, ei;

  cspi_return_if_fail (obj != NULL);

  cspi_dbus_get_property (obj, spi_interface_hyperlink, "startIndex", NULL, "i", &si);
 cspi_return_if_ev ("startIndex");
  cspi_dbus_get_property (obj, spi_interface_hyperlink, "endIndex", NULL, "i", &ei);
 cspi_return_if_ev ("endIndex");
 *startIndex = si;
 *endIndex = ei; 
}

/**
 * AccessibleHyperlink_isValid:
 * @obj: a pointer to the #AccessibleHyperlink on which to operate.
 *
 * Tell whether an #AccessibleHyperlink object is still valid with respect to its
 *          originating hypertext object.
 *
 * Returns: #TRUE of the specified #AccessibleHyperlink is still valid with respect
 *          to its originating #AccessibleHypertext object, #FALSE otherwise.
 **/
SPIBoolean
AccessibleHyperlink_isValid (AccessibleHyperlink *obj)
{
  dbus_bool_t retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  cspi_dbus_call (obj, spi_interface_hyperlink, "isValid", NULL, "=>b", &retval);

  cspi_return_val_if_ev ("isValid", FALSE);

  return retval;
}


