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
  char **content_types = malloc (sizeof (char *));
  content_types [0] = NULL;

  /* TODO: connect this to the correct libspi implementation code */
  return content_types;
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
  /* TODO: connect this to the correct libspi implementation code */
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
SPIBoolean
AccessibleStreamableContent_seek (AccessibleStreamableContent *obj,
				  long int offset,
				  unsigned int seek_type)
{
  /* TODO: connect this to the correct libspi implementation code */
  return FALSE;
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
 *     to a buffer.
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
  /* TODO: connect this to the correct libspi implementation code */
  return -1;
}

