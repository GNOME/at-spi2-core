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

#include <string.h>
#include <libbonobo.h>
#include <cspi/spi-private.h>


/* TODO: factor/wrap Bonobo_Stream dependency to cspi/bonobo */

struct StreamCacheItem {
  Bonobo_Stream stream;
  gchar *mimetype;
};

static gboolean
streams_equal_func (gconstpointer a, gconstpointer b)
{
  const struct StreamCacheItem *c1 = a, *c2 = b;
  return CORBA_Object_is_equivalent (c1->stream, c2->stream, cspi_ev ());
}

static void
stream_cache_item_free (gpointer a)
{
  struct StreamCacheItem *cache_item = a;

  cspi_release_unref (cache_item->stream);
  SPI_freeString (cache_item->mimetype);
  g_free (cache_item);
}

static GHashTable *streams = NULL;

GHashTable *
get_streams (void) 
{
  if (streams == NULL)
      streams = g_hash_table_new_full (g_direct_hash, streams_equal_func, 
				       NULL, stream_cache_item_free);
  return streams;
}

static CORBA_long
accessible_bonobo_stream_client_seek (const Bonobo_Stream stream,
				      CORBA_long offset,
				      Bonobo_Stream_SeekType seek_type,
				      CORBA_Environment  *opt_ev)
{
	Bonobo_StorageInfo *info;
	CORBA_Environment  *ev, temp_ev;
	CORBA_long ret_offset;
       
	if (!opt_ev) {
		CORBA_exception_init (&temp_ev);
		ev = &temp_ev;
	} else
		ev = opt_ev;

	ret_offset = Bonobo_Stream_seek (stream, offset, seek_type, ev);
	if (BONOBO_EX (ev))
	        ret_offset = -1;

	if (!opt_ev)
		CORBA_exception_free (&temp_ev);
	
	return ret_offset;
}

/* internal use only, declared in cspi-private.h */
void
cspi_streams_close_all (void)
{
  if (streams)
    {
      g_hash_table_destroy (streams);
      streams = NULL;
    }
}

/**
 * AccessibleStreamableContent_ref:
 * @obj: a pointer to the #AccessibleStreamableContent implementor on which to
 *       operate.
 *
 * Increment the reference count for an #AccessibleStreamableContent object.
 *
 **/
void
AccessibleStreamableContent_ref (AccessibleStreamableContent *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleStreamableContent_unref:
 * @obj: a pointer to the #AccessibleStreamableContent implementor
 *       on which to operate. 
 *
 * Decrement the reference count for an #AccessibleStreamableContent object.
 *
 **/
void
AccessibleStreamableContent_unref (AccessibleStreamableContent *obj)
{
  cspi_object_unref (obj);
}

/**
 * AccessibleStreamableContent_getContentTypes:
 * @obj: a pointer to the #AccessibleStreamableContent implementor on which to operate.
 *
 * Get a list of strings containing the content mimetypes available from an
 *       #AccessibleStreamableContent implementor.
 *
 * Returns: an array of strings, terminated by a NULL string, specifying the
 *       mimetypes for which the streamed content is available.
 *
 **/

char **
AccessibleStreamableContent_getContentTypes (AccessibleStreamableContent *obj)
{
  Accessibility_StringSeq *mimeseq;
  char **content_types;
  int i;

  mimeseq = Accessibility_StreamableContent_getContentTypes (CSPI_OBJREF (obj),
							     cspi_ev ());
  cspi_return_val_if_ev ("getContentTypes", NULL); 

  content_types = g_new0 (char *, mimeseq->_length + 1);
  for (i = 0; i < mimeseq->_length; ++i)
    content_types[i] = CORBA_string_dup (mimeseq->_buffer[i]);
  content_types [mimeseq->_length] = NULL;
  CORBA_free (mimeseq);

  return content_types;
}

void
AccessibleStreamableContent_freeContentTypesList (AccessibleStreamableContent *obj,
						  char **content_types)
{
  if (content_types) 
    {
      gint i = 0;
      while (content_types[i])
	{
	  g_free (content_types[i]);
	  i++;
	}
      g_free (content_types);
    }
}

/**
 * AccessibleStreamableContent_open:
 * @obj: a pointer to the #AccessibleStreamableContent implementor on which to operate.
 * @content_type: a string specifying the content type to retrieve (should match one
 * of the return strings from #AccessibleStreamableContent_getContentTypes ()).
 *
 * Open a streaming connection to an AccessibleStreamableContent implementor,
 *       of a particular content type
 *
 * Returns: #TRUE if successful, #FALSE if unsuccessful.
 *
 **/
SPIBoolean
AccessibleStreamableContent_open (AccessibleStreamableContent *obj,
				  const char *content_type)
{
  Bonobo_Stream stream;
  struct StreamCacheItem *cache;
  stream = Accessibility_StreamableContent_getContent (CSPI_OBJREF (obj),
						       content_type,
						       cspi_ev ());
  cspi_return_val_if_ev ("getContent", FALSE); 

  if (stream != CORBA_OBJECT_NIL) {
    cache = g_new0 (struct StreamCacheItem, 1);
    cache->stream = stream;
    cache->mimetype = CORBA_string_dup (content_type);
    g_hash_table_replace (get_streams (), stream, cache);
    return TRUE;
  }
  return FALSE;
}

/**
 * AccessibleStreamableContent_close:
 * @obj: a pointer to the #AccessibleStreamableContent implementor on which to operate.
 *
 * Close the current streaming connection to an AccessibleStreamableContent implementor.
 * This must be called before any subsequent AccessibleStreamableContent_open
 * calls on the same object.
 *
 * Returns: #TRUE if successful, #FALSE if unsuccessful.
 *
 **/
SPIBoolean
AccessibleStreamableContent_close (AccessibleStreamableContent *obj)
{
  if (CSPI_OBJREF (obj) != CORBA_OBJECT_NIL) {
    if (g_hash_table_remove (get_streams (), CSPI_OBJREF (obj)))
      return TRUE;
  }
  return FALSE;
}

/**
 * AccessibleStreamableContent_seek:
 * @obj: a pointer to the #AccessibleStreamableContent implementor on which to operate.
 * @offset: a long int specifying the offset into the stream.
 * @seek_type: an enum indicating the seek offset type, may be SEEK_SET,
 *            SEEK_CUR, SEEK_END (as in the lseek() libc command).
 *
 * Cause the current streamable content connection (obtained via
 *     #AccessibleStreamableContent_open()) to seek to a particular offset in the
 *     stream.
 *
 * Returns: #TRUE if successful, #FALSE if unsuccessful.
 *
 **/
long int
AccessibleStreamableContent_seek (AccessibleStreamableContent *obj,
				  long int offset,
				  AccessibleStreamableContentSeekType seek_type)
{
  Bonobo_Stream stream;
  long int ret_offset = 0;
  struct StreamCacheItem *cached; 
  Bonobo_Stream_SeekType bonobo_seek_type;

  cached = g_hash_table_lookup (get_streams (), CSPI_OBJREF (obj));
  if (cached)
    {
      stream = cached->stream;
      if (stream != CORBA_OBJECT_NIL)
	{
          guint8 *mem;
	  switch (seek_type) {
	  case SPI_STREAM_SEEK_SET:
	    bonobo_seek_type = Bonobo_Stream_SeekSet; 
	    break;
	  case SPI_STREAM_SEEK_END:
	    bonobo_seek_type = Bonobo_Stream_SeekEnd; 
	    break;
	  case SPI_STREAM_SEEK_CUR:
	  default:
	    bonobo_seek_type = Bonobo_Stream_SeekCur; 
	    break;
	  }
	  /* bonobo-client doesn't wrap seek yet, so we have to. */
	  ret_offset = accessible_bonobo_stream_client_seek (stream, offset, 
							     bonobo_seek_type, cspi_ev ());
	  cspi_return_val_if_ev ("seek", FALSE);
	}
    }
  return ret_offset;
}

/**
 * AccessibleStreamableContent_read:
 * @obj: a pointer to the #AccessibleStreamableContent implementor on which to operate.
 * @buff: a pointer to a buffer into which the resulting bytes read from the stream
 *        are to be written.
 * @nbytes: a long integer indicating the number of bytes to read/write.
 * @read_type: currently unused, specifies behavior of reads for streamed content
 *        if blocking is not allowed, etc.
 *
 * Copy (read) bytes from the currently open streamable content connection
 *     to a buffer.  This is a blocking API, in the sense that it does not 
 *     return until the bytes have been read, or an error condition is 
 *     detected.
 *
 * Returns: an integer indicating the number of bytes read, or -1 on error.
 *
 **/
SPIBoolean
AccessibleStreamableContent_read (AccessibleStreamableContent *obj,
				  void *buff,
				  long int nbytes,
				  unsigned int read_type)
{
  Bonobo_Stream stream;
  struct StreamCacheItem *cached; 
  cached = g_hash_table_lookup (get_streams (), CSPI_OBJREF (obj));
  if (cached)
    {
      CORBA_long len_read;
      stream = cached->stream;
      if (stream != CORBA_OBJECT_NIL)
	{
          guint8 *mem;
	  mem = bonobo_stream_client_read (stream, (size_t) nbytes, &len_read, cspi_ev ());
	  cspi_return_val_if_ev ("read", FALSE);
	  if (mem)
            {
              memcpy (buff, mem, len_read);
	      g_free (mem);
	      if ((nbytes == -1) || (len_read == nbytes))
                return TRUE;
	    }
	}
    }
  return FALSE;
}

