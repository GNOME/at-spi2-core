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

#ifndef __ATK_HYPERLINK_H__
#define __ATK_HYPERLINK_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <atk/atkaction.h>

/*
 * AtkHyperlink encapsulates a link or set of links in a hypertext document.
 *
 * It implements the AtkAction interface.
 */

#define ATK_TYPE_HYPERLINK                        (atk_hyperlink_get_type ())
#define ATK_HYPERLINK(obj)                        (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_HYPERLINK, AtkHyperlink))
#define ATK_HYPERLINK_CLASS(klass)                (G_TYPE_CHECK_CLASS_CAST ((klass), ATK_TYPE_HYPERLINK, AtkHyperlinkClass))
#define ATK_IS_HYPERLINK(obj)                     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_HYPERLINK))
#define ATK_IS_HYPERLINK_CLASS(klass)             (G_TYPE_CHECK_CLASS_TYPE ((klass), ATK_TYPE_HYPERLINK))
#define ATK_HYPERLINK_GET_CLASS(obj)              (G_TYPE_INSTANCE_GET_CLASS ((obj), ATK_TYPE_HYPERLINK, AtkHyperlinkClass))

typedef struct _AtkHyperlink                      AtkHyperlink;
typedef struct _AtkHyperlinkClass                 AtkHyperlinkClass;

struct _AtkHyperlink
{
  GObject parent;
};

struct _AtkHyperlinkClass
{
  GObjectClass parent;

  /*
   * Returns a string specifying the URI associated with the nth anchor
   * of this link.
   */
  gchar*           (* get_uri)             (AtkHyperlink     *link,
                                            gint             i);
  /*
   * Returns an object which represents the link action, as appropriate for 
   * that link.
   */
  AtkObject*       (* get_object)          (AtkHyperlink     *link,
                                            gint             i);
  /*
   * Gets the index with the hypertext document at which this link ends
   */
  gint             (* get_end_index)       (AtkHyperlink     *link);

  /* 
   * Gets the index with the hypertext document at which this link begins 
   */
  gint             (* get_start_index)     (AtkHyperlink     *link);

  /*
   * Since the document a link is associated with may have changed, this 
   * method returns whether or not this link is still valid (with respect
   * to the document is references)
   */
  gboolean         (* is_valid)            (AtkHyperlink     *link);

  /* 
   * Returns the number of anchors associated with this link
   */
  gint	           (* get_n_anchors)	   (AtkHyperlink     *link);

  AtkFunction      pad1;
  AtkFunction      pad2;
  AtkFunction      pad3;
  AtkFunction      pad4;
};

GType            atk_hyperlink_get_type             (void);

gchar*           atk_hyperlink_get_uri              (AtkHyperlink     *link,
                                                     gint             i);

AtkObject*       atk_hyperlink_get_object           (AtkHyperlink     *link,
                                                     gint             i);

gint             atk_hyperlink_get_end_index        (AtkHyperlink     *link);

gint             atk_hyperlink_get_start_index      (AtkHyperlink     *link);

gboolean         atk_hyperlink_is_valid             (AtkHyperlink     *link);

gint		 atk_hyperlink_get_n_anchors        (AtkHyperlink     *link);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_HYPERLINK_H__ */
