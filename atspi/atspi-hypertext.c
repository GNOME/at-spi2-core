/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#include "atspi-private.h"

/**
 * AtspiHypertext:
 *
 * An interface used for objects which implement linking between
 * multiple resource locations.
 *
 * An interface used for objects which implement linking between
 * multiple resource or content locations, or multiple 'markers'
 * within a single document. A hypertext instance is associated
 * with one or more hyperlinks which are associated with particular
 * offsets within the hypertext's content.
 */

/**
 * atspi_hypertext_get_n_links:
 * @obj: a pointer to the #AtspiHypertext implementor on which to operate.
 *
 * Gets the total number of #AtspiHyperlink objects that an
 * #AtspiHypertext implementor has.
 *
 * Returns: a #gint indicating the number of #AtspiHyperlink objects
 *        of the #AtspiHypertext implementor, or -1 if
 *        the number cannot be determined (for example, if the
 *        #AtspiHypertext object is so large that it is not
 *        all currently in the memory cache).
 **/
gint
atspi_hypertext_get_n_links (AtspiHypertext *obj, GError **error)
{
  dbus_int32_t retval = 0;

  g_return_val_if_fail (obj != NULL, FALSE);

  _atspi_dbus_call (obj, atspi_interface_hypertext, "GetNLinks", error, "=>i", &retval);

  return retval;
}

/**
 * atspi_hypertext_get_link:
 * @obj: a pointer to the #AtspiHypertext implementor on which to operate.
 * @link_index: a (zero-index) #gint indicating which hyperlink to query.
 *
 * Gets the #AtspiHyperlink object at a specified index.
 *
 * Returns: (nullable) (transfer full): the #AtspiHyperlink object
 *          specified by @link_index.
 **/
AtspiHyperlink *
atspi_hypertext_get_link (AtspiHypertext *obj, gint link_index, GError **error)
{
  dbus_int32_t d_link_index = link_index;
  DBusMessage *reply;

  g_return_val_if_fail (obj != NULL, NULL);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_hypertext, "GetLink", error, "i", d_link_index);

  return _atspi_dbus_return_hyperlink_from_message (reply);
}

/**
 * atspi_hypertext_get_link_index:
 * @obj: a pointer to the #AtspiHypertext implementor on which to operate.
 * @character_offset: a #gint specifying the character offset to query.
 *
 * Gets the index of the #AtspiHyperlink object at a specified
 *        character offset.
 *
 * Returns: the linkIndex of the #AtspiHyperlink active at
 *        character offset @character_offset, or -1 if there is
 *        no hyperlink at the specified character offset.
 **/
int
atspi_hypertext_get_link_index (AtspiHypertext *obj,
                                gint character_offset,
                                GError **error)
{
  dbus_int32_t d_character_offset = character_offset;
  dbus_int32_t retval = -1;

  g_return_val_if_fail (obj != NULL, -1);

  _atspi_dbus_call (obj, atspi_interface_hypertext, "GetLinkIndex", error, "i=>i", d_character_offset, &retval);

  return retval;
}

static void
atspi_hypertext_base_init (AtspiHypertext *klass)
{
}

GType
atspi_hypertext_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo tinfo = {
        sizeof (AtspiHypertext),
        (GBaseInitFunc) atspi_hypertext_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtspiHypertext", &tinfo, 0);
    }
  return type;
}
