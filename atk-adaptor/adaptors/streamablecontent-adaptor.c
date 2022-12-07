/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap) *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

/* streamablecontent.c : implements the StreamableContent interface */

#include "config.h"

#include <libspi/component.h>
#include <libspi/streamablecontent.h>

#include <stdio.h>
#include <string.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_TYPE_BASE

/* A pointer to our parent object class */
static GObjectClass *spi_streamable_parent_class;

#define SPI_CONTENT_STREAM_TYPE (spi_content_stream_get_type ())
#define SPI_CONTENT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), SPI_CONTENT_STREAM_TYPE, SpiContentStream))
#define SPI_CONTENT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SPI_CONTENT_STREAM_TYPE, SpiContentStreamClass))
#define SPI_IS_CONTENT_STREAM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SPI_CONTENT_STREAM_TYPE))
#define SPI_IS_CONTENT_STREAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SPI_CONTENT_STREAM_TYPE))

typedef struct _SpiContentStream SpiContentStream;
typedef struct _SpiContentStreamClass SpiContentStreamClass;

struct _SpiContentStream
{
  BonoboObject parent;
  GIOChannel *gio;
};

struct _SpiContentStreamClass
{
  BonoboObjectClass parent_class;
  POA_Accessibility_ContentStream__epv epv;
};

GType spi_content_stream_get_type (void);

static SpiContentStream *
spi_content_stream_new (GIOChannel *gio)
{
  SpiContentStream *new_stream = g_object_new (SPI_CONTENT_STREAM_TYPE, NULL);
  new_stream->gio = gio;
  return new_stream;
}

static void
spi_content_stream_dispose (GObject *o)
{
  if (SPI_IS_CONTENT_STREAM (o))
    {
      SpiContentStream *stream = SPI_CONTENT_STREAM (o);
      if (stream->gio)
        g_io_channel_unref (stream->gio);
    }
}

static CORBA_long
impl_content_stream_seek (PortableServer_Servant servant,
                          const CORBA_long offset,
                          const Accessibility_ContentStream_SeekType whence,
                          CORBA_Environment *ev)
{
  SpiContentStream *stream =
      SPI_CONTENT_STREAM (bonobo_object_from_servant (servant));
  if (stream && stream->gio)
    {
      GError *err;
      GSeekType seektype = G_SEEK_SET;
      switch (whence)
        {
        case Accessibility_ContentStream_SEEK_CURRENT:
          seektype = G_SEEK_CUR;
          break;
        case Accessibility_ContentStream_SEEK_END:
          seektype = G_SEEK_END;
          break;
        }
      if (g_io_channel_seek_position (stream->gio, (gint64) offset,
                                      seektype, &err) == G_IO_STATUS_NORMAL)
        return offset;
      else
        return -1;
    }
  else
    return -1;
}

static CORBA_long
impl_content_stream_read (PortableServer_Servant servant,
                          const CORBA_long count,
                          Accessibility_ContentStream_iobuf **buffer,
                          CORBA_Environment *ev)
{
  SpiContentStream *stream =
      SPI_CONTENT_STREAM (bonobo_object_from_servant (servant));
  CORBA_long realcount = 0;

  if (stream && stream->gio)
    {
      gchar *gbuf = NULL;
      GIOStatus status;
      GError *err = NULL;

      /* read the giochannel and determine the actual bytes read... */
      if (count != -1)
        {
          gbuf = g_malloc (count + 1);
          status =
              g_io_channel_read_chars (stream->gio, gbuf, count, &realcount,
                                       &err);
        }
      else
        status =
            g_io_channel_read_to_end (stream->gio, &gbuf, &realcount, &err);

      if (status == G_IO_STATUS_NORMAL || status == G_IO_STATUS_EOF)
        {
          *buffer = Bonobo_Stream_iobuf__alloc ();
          CORBA_sequence_set_release (*buffer, TRUE);

          (*buffer)->_buffer =
              CORBA_sequence_CORBA_octet_allocbuf (realcount);
          (*buffer)->_length = realcount;

          g_memmove ((*buffer)->_buffer, gbuf, realcount);
        }

      g_free (gbuf);
    }

  return realcount;
}

static void
impl_content_stream_close (PortableServer_Servant servant,
                           CORBA_Environment *ev)
{
  GIOStatus status;
  GError *err;
  SpiContentStream *stream =
      SPI_CONTENT_STREAM (bonobo_object_from_servant (servant));
  if (stream && stream->gio)
    {
      status = g_io_channel_shutdown (stream->gio, TRUE, &err);
      g_io_channel_unref (stream->gio);
    }
  if (err)
    g_free (err);
}

static void
spi_content_stream_class_init (SpiContentStreamClass *klass)
{
  POA_Accessibility_ContentStream__epv *epv = &klass->epv;
  GObjectClass *object_class = (GObjectClass *) klass;

  epv->seek = impl_content_stream_seek;
  epv->read = impl_content_stream_read;
  epv->close = impl_content_stream_close;

  object_class->dispose = spi_content_stream_dispose;
}

static void
spi_content_stream_init (SpiContentStream *stream)
{
}

BONOBO_TYPE_FUNC_FULL (SpiContentStream,
                       Accessibility_ContentStream,
                       BONOBO_TYPE_OBJECT,
                       spi_content_stream)
static AtkStreamableContent
    *
    get_streamable_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (ATK_IS_STREAMABLE_CONTENT (object->gobj), NULL);
  return ATK_STREAMABLE_CONTENT (object->gobj);
}

/*
 * CORBA Accessibility::StreamableContent::getContentTypes method implementation
 */
static Accessibility_StringSeq *
impl_accessibility_streamable_get_content_types (PortableServer_Servant
                                                     servant,
                                                 CORBA_Environment *ev)
{
  Accessibility_StringSeq *typelist = Accessibility_StringSeq__alloc ();
  AtkStreamableContent *streamable = get_streamable_from_servant (servant);
  int n_types, i;

  typelist->_length = typelist->_maximum = 0;

  g_return_val_if_fail (streamable != NULL, typelist);

  n_types = atk_streamable_content_get_n_mime_types (streamable);

  if (n_types)
    {
      typelist->_length = typelist->_maximum = n_types;
      typelist->_buffer = Accessibility_StringSeq_allocbuf (n_types);
      for (i = 0; i < n_types; ++i)
        {
          const gchar *mimetype =
              atk_streamable_content_get_mime_type (streamable, i);
          typelist->_buffer[i] = CORBA_string_dup (mimetype ? mimetype : "");
        }
    }
  return typelist;
}

/*
 * CORBA Accessibility::StreamableContent::getContent method implementation
 */
static Bonobo_Stream
impl_accessibility_streamable_get_content (PortableServer_Servant servant,
                                           const CORBA_char *content_type,
                                           CORBA_Environment *ev)
{
  Bonobo_Stream stream;
  AtkStreamableContent *streamable = get_streamable_from_servant (servant);
  GIOChannel *gio;

  g_return_val_if_fail (streamable != NULL, NULL);

  gio = atk_streamable_content_get_stream (streamable, content_type);

  stream = CORBA_OBJECT_NIL; /* deprecated,
                              * and it was never implemented,
                              * so don't bother fixing this
                              */
  return stream;
}

/*
 * CORBA Accessibility::StreamableContent::getStream method implementation
 */
static Accessibility_ContentStream
impl_accessibility_streamable_get_stream (PortableServer_Servant servant,
                                          const CORBA_char *content_type,
                                          CORBA_Environment *ev)
{
  SpiContentStream *stream;
  AtkStreamableContent *streamable = get_streamable_from_servant (servant);
  GIOChannel *gio;

  g_return_val_if_fail (streamable != NULL, NULL);

  gio = atk_streamable_content_get_stream (streamable, content_type);

  stream = spi_content_stream_new (gio);

  return bonobo_object_dup_ref (BONOBO_OBJREF (stream), ev);
}

/*
 * CORBA Accessibility::StreamableContent::GetURI method implementation
 */
static CORBA_string
impl_accessibility_streamable_get_uri (PortableServer_Servant servant,
                                       const CORBA_char *content_type,
                                       CORBA_Environment *ev)
{
  gchar *uri;
  AtkStreamableContent *streamable = get_streamable_from_servant (servant);

  g_return_val_if_fail (streamable != NULL, NULL);

  uri = atk_streamable_content_get_uri (streamable, content_type);

  return (uri != NULL ? CORBA_string_dup (uri) : CORBA_string_dup (""));
}

static void
spi_streamable_class_init (SpiStreamableClass *klass)
{
  POA_Accessibility_StreamableContent__epv *epv = &klass->epv;
  spi_streamable_parent_class = g_type_class_peek_parent (klass);

  epv->getContentTypes = impl_accessibility_streamable_get_content_types;
  epv->getContent = impl_accessibility_streamable_get_content;
  epv->getStream = impl_accessibility_streamable_get_stream;
  epv->GetURI = impl_accessibility_streamable_get_uri;
}

static void
spi_streamable_init (SpiStreamable *streamable)
{
}

SpiStreamable *
spi_streamable_interface_new (AtkObject *o)
{
  SpiStreamable *retval = g_object_new (SPI_STREAMABLE_TYPE, NULL);

  spi_base_construct (SPI_BASE (retval), G_OBJECT (o));

  return retval;
}

BONOBO_TYPE_FUNC_FULL (SpiStreamable,
                       Accessibility_StreamableContent,
                       PARENT_TYPE,
                       spi_streamable)
