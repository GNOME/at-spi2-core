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

/* streamablecontent.c : implements the StreamableContent interface */

#include <config.h>
#include <stdio.h>
#include <libspi/accessible.h>
#include <libspi/streamablecontent.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_TYPE_BASE

/* A pointer to our parent object class */
static GObjectClass *spi_streamable_parent_class;

static AtkStreamableContent *
get_streamable_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (ATK_IS_STREAMABLE_CONTENT(object->gobj), NULL);
  return ATK_STREAMABLE_CONTENT (object->gobj);
}

/*
 * CORBA Accessibility::StreamableContent::getContentTypes method implementation
 */
static Accessibility_StringSeq*
impl_accessibility_streamable_get_content_types (PortableServer_Servant servant,
						 CORBA_Environment     *ev)
{
  Accessibility_StringSeq *typelist = Accessibility_StringSeq__alloc ();
  AtkStreamableContent *streamable = get_streamable_from_servant (servant);
  int n_types, i;

  typelist->_length = 0;
  g_return_val_if_fail (streamable != NULL, typelist);

  n_types = atk_streamable_content_get_n_mime_types (streamable);
  typelist->_length = n_types;
  typelist->_buffer = Accessibility_StringSeq_allocbuf (n_types);
  for (i = 0; i < n_types; ++i) {
    const gchar *mimetype = atk_streamable_content_get_mime_type (streamable, i);
    typelist->_buffer[i] = CORBA_string_dup (mimetype ? mimetype : "");
  }

  return typelist;
}

/*
 * CORBA Accessibility::StreamableContent::getContent method implementation
 */
static Bonobo_Stream
impl_accessibility_streamable_get_content (PortableServer_Servant servant,
					   const CORBA_char * content_type,
					   CORBA_Environment     *ev)
{
  Bonobo_Stream stream;
  AtkStreamableContent *streamable = get_streamable_from_servant (servant);
  GIOChannel *gio;

  g_return_val_if_fail (streamable != NULL, NULL);

  gio = atk_streamable_content_get_stream (streamable, content_type);

  stream = CORBA_OBJECT_NIL; /* FIXME! */

  return stream;
}

static void
spi_streamable_class_init (SpiStreamableClass *klass)
{
        POA_Accessibility_StreamableContent__epv *epv = &klass->epv;
        spi_streamable_parent_class = g_type_class_peek_parent (klass);

        epv->getContentTypes = impl_accessibility_streamable_get_content_types;
        epv->getContent = impl_accessibility_streamable_get_content;
}

static void
spi_streamable_init (SpiStreamable *streamable)
{
}

BONOBO_TYPE_FUNC_FULL (SpiStreamable,
		       Accessibility_StreamableContent,
		       PARENT_TYPE,
		       spi_streamable)

SpiStreamable *
spi_streamable_interface_new (AtkObject *o)
{
    SpiStreamable *retval = g_object_new (SPI_STREAMABLE_TYPE, NULL);

    spi_base_construct (SPI_BASE (retval), G_OBJECT(o));

    return retval;
}
