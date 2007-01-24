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

#define CORBA_BLOCK_SIZE 65536 /* see libbonobo, dunno where this is officially dictated */

struct StreamCacheItem {
  Accessibility_ContentStream stream;
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

static GHashTable *
get_streams (void) 
{
  if (streams == NULL)
      streams = g_hash_table_new_full (g_direct_hash, streams_equal_func, 
				       NULL, stream_cache_item_free);
  return streams;
}

static CORBA_long
accessible_content_stream_client_seek (const Accessibility_ContentStream stream,
				      CORBA_long offset,
				      Accessibility_ContentStream_SeekType seek_type,
				      CORBA_Environment  *opt_ev)
{
	CORBA_Environment  *ev, temp_ev;
	CORBA_long ret_offset;
       
	if (!opt_ev) {
		CORBA_exception_init (&temp_ev);
		ev = &temp_ev;
	} else
		ev = opt_ev;

	ret_offset = Accessibility_ContentStream_seek (stream, offset, seek_type, ev);
	if (BONOBO_EX (ev))
	        ret_offset = -1;

	if (!opt_ev)
		CORBA_exception_free (&temp_ev);
	
	return ret_offset;
}

static guint8*
accessible_content_stream_client_read (const Accessibility_ContentStream stream,
				       const size_t size,
				       CORBA_long *length_read,
				       CORBA_Environment  *ev)
{
	size_t  pos;
	guint8 *mem;
	size_t  length;

	g_return_val_if_fail (ev != NULL, NULL);

	if (length_read)
		*length_read = size;

	length = size;

	if (length == 0)
		return NULL;

	mem = g_try_malloc (length);
	if (!mem) {
		CORBA_exception_set_system (ev, ex_CORBA_NO_MEMORY,
					    CORBA_COMPLETED_NO);
		return NULL;
	}

	*length_read = 0;

	for (pos = 0; pos < length;) {
		Accessibility_ContentStream_iobuf *buf;
		CORBA_long           len;

		len = (pos + CORBA_BLOCK_SIZE < length) ?
			CORBA_BLOCK_SIZE : length - pos;

		Accessibility_ContentStream_read (stream, len, &buf, ev);

		if (BONOBO_EX (ev) || !buf)
			goto io_error;

		if (buf->_length > 0) {
			memcpy (mem + pos, buf->_buffer, buf->_length);
			pos += buf->_length;
			*length_read += buf->_length;
			/* we assume a short read equals EOF ... is that right? */
			if (buf->_length < len || *length_read == size)
			    return mem;
		} else {
			g_warning ("Buffer length %d", buf->_length);
			goto io_error;
		}
		*length_read += buf->_length;

		CORBA_free (buf);
	}

	return mem;

 io_error:
	return NULL;
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
 * @Since: AT-SPI 1.4
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
 * @Since: AT-SPI 1.4
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
 * @Since: AT-SPI 1.4
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

  g_return_val_if_fail (obj != NULL, NULL);

  mimeseq = Accessibility_StreamableContent_getContentTypes (CSPI_OBJREF (obj),
							     cspi_ev ());
  cspi_return_val_if_ev ("getContentTypes", NULL); 
  content_types = g_new0 (char *, mimeseq->_length + 1);
  for (i = 0; i < mimeseq->_length; ++i)
    content_types[i] = g_strdup (mimeseq->_buffer[i]);
  content_types [mimeseq->_length] = NULL;
  CORBA_free (mimeseq);
  
  return content_types;
}
/**
* AccessibleStreamableContent_freeContentTypesList:
* @obj: the AccessibleStreamableContent implementor on which to operate.
* @content_types: a list of content types previously returned by 
*     #AccessibleStreamableContent_getContentTypes.
*
* Free the memory associated with a call to #AccessibleStreamableContent_getContentTypes, once 
* the result has been used.
*
* Since: AT-SPI 1.4
**/
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
 *       of a particular content type.  Note that a client may only have one
 *       open stream per streamable interface instance in the current 
 *       implementation.
 *
 * @Since: AT-SPI 1.4
 *
 * Returns: #TRUE if successful, #FALSE if unsuccessful.
 *
 **/
SPIBoolean
AccessibleStreamableContent_open (AccessibleStreamableContent *obj,
				  const char *content_type)
{
  Accessibility_ContentStream stream;
  struct StreamCacheItem *cache;
  stream = Accessibility_StreamableContent_getStream (CSPI_OBJREF (obj),
						      content_type,
						      cspi_ev ());
  cspi_return_val_if_ev ("getContent", FALSE); 

  if (stream != CORBA_OBJECT_NIL) {
    cache = g_new0 (struct StreamCacheItem, 1);
    cache->stream = stream;
    cache->mimetype = CORBA_string_dup (content_type);

    g_hash_table_replace (get_streams (), CSPI_OBJREF (obj), cache);
    /* FIXME 
     * This limits us to one concurrent stream per streamable interface
     * for a given client.
     * It might be reasonable for a client to open more than one stream
     * to content, in different mime-types, at the same time.
     */

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
 * @Since: AT-SPI 1.4
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
 * @Since: AT-SPI 1.4
 *
 * Returns: #TRUE if successful, #FALSE if unsuccessful.
 *
 **/
long int
AccessibleStreamableContent_seek (AccessibleStreamableContent *obj,
				  long int offset,
				  AccessibleStreamableContentSeekType seek_type)
{
  Accessibility_ContentStream stream;
  long int ret_offset = 0;
  struct StreamCacheItem *cached; 
  Accessibility_ContentStream_SeekType content_seek_type;

  cached = g_hash_table_lookup (get_streams (), CSPI_OBJREF (obj));
  if (cached)
    {
      stream = cached->stream;
      if (stream != CORBA_OBJECT_NIL)
	{
	  switch (seek_type) {
	  case SPI_STREAM_SEEK_SET:
	    content_seek_type = Accessibility_ContentStream_SEEK_SET; 
	    break;
	  case SPI_STREAM_SEEK_END:
	    content_seek_type = Accessibility_ContentStream_SEEK_END; 
	    break;
	  case SPI_STREAM_SEEK_CUR:
	  default:
	    content_seek_type = Accessibility_ContentStream_SEEK_CURRENT; 
	    break;
	  }
	  ret_offset = accessible_content_stream_client_seek (stream, offset, 
							     content_seek_type, cspi_ev ());
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
 * @Since: AT-SPI 1.4
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
  Accessibility_ContentStream stream;
  struct StreamCacheItem *cached; 
  cached = g_hash_table_lookup (get_streams (), CSPI_OBJREF (obj));
  if (cached)
    {
      CORBA_long len_read = 0;
      stream = cached->stream;
      if (stream != CORBA_OBJECT_NIL)
	{
          guint8 *mem;

	  mem = accessible_content_stream_client_read (stream, (size_t) nbytes, &len_read, cspi_ev ());
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
  else g_message ("no matching stream was opened...");
  return FALSE;
}

