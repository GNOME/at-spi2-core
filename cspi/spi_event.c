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
#include <cspi/bonobo/cspi-bonobo-listener.h>

/**
 * SPI_freeAccessibleKeySet:
 * @keyset: An AccessibleKeyset to free.
 *
 * Release the memory used by an AccessibleKeySet.
 *
 **/
void
SPI_freeAccessibleKeySet (AccessibleKeySet *keyset)
{
  int i = 0;	
  g_free (keyset->keysyms);
  g_free (keyset->keycodes);
  while (keyset->keystrings [i])
    {
      g_free (keyset->keystrings [i++]);
    }
  g_free (keyset->keystrings);
  g_free (keyset);
}

/**
 * SPI_createAccessibleKeySet:
 * @len: the number of key values in the key set.
 * @keysyms: a UTF-8 string containing symbolic key values to be matched, or NULL if
 *           matching is performed against other key values instead.
 * @keycodes: an array of unsigned short values which are the hardware keycodes
 *           to be matched, or NULL if the keyset is specified solely by keysyms
 *           and/or keystrings.
 * @keystrings: an array of null-terminated character strings which specify key
 *             name values to match, or NULL if the keyset is specified solely by
 *             keycodes and/or keysyms.
 *
 * Create a new #AccessibleKeySet of a specified length.
 * A KeySet is used typically to match key event values, and a matches are made
 * using the following criteria: a match exists with a key event if all non-null
 * i-th members of the keyset match the key event.
 * If both keystring and keysym values are NULL, a keycode value match is
 * forced, thus the match for keysym=0, keycode=0, keystring=NULL is
 * keycode 0.
 *
 * Returns: a pointer to a newly-created #AccessibleKeySet.
 *
 **/
AccessibleKeySet *
SPI_createAccessibleKeySet (int len, const char *keysyms, short *keycodes,
			    const char **keystrings)
{
  AccessibleKeySet *keyset = g_new0 (AccessibleKeySet, 1);
  int i, keysym_len = 0;
  const char *keysym_ptr = keysyms;
  keyset->len = len;
  keyset->keysyms = g_new0 (unsigned long, len);
  keyset->keycodes = g_new0 (unsigned short, len);
  keyset->keystrings = g_new0 (char *, len);
  if (keysyms)
    {
      keysym_len = g_utf8_strlen (keysyms, -1);
    }
  for (i = 0; i < len; ++i)
    {
      if (i < keysym_len)
        {
	  keyset->keysyms [i] = (unsigned long) g_utf8_get_char (keysym_ptr);
	  keysym_ptr = g_utf8_find_next_char (keysym_ptr, NULL);
        }
      else
        {
          keyset->keysyms [i] = 0;
        }
      if (keycodes)
        {
	  keyset->keycodes [i] = keycodes [i];
	}
      if (keystrings)
        {
	  keyset->keystrings [i] = g_strdup (keystrings [i]);
	}
    }
  return keyset;	
}

/**
 * SPI_createAccessibleEventListener:
 * @callback : an #AccessibleEventListenerCB callback function, or NULL.
 * @user_data: a pointer to data which will be passed to the callback when invoked.
 *
 * Create a new #AccessibleEventListener with a specified (in-process) callback function.
 *
 * Returns: a pointer to a newly-created #AccessibleEventListener.
 *
 **/
AccessibleEventListener *
SPI_createAccessibleEventListener (AccessibleEventListenerCB callback,
				   void                     *user_data)
{
  AccessibleEventListener *listener = cspi_event_listener_new ();
  if (callback)
    {
      AccessibleEventListener_addCallback (listener, callback, user_data);
    }
  return listener;
}

/**
 * AccessibleEventListener_addCallback:
 * @listener: the #AccessibleEventListener instance to modify.
 * @callback: an #AccessibleEventListenerCB function pointer.
 * @user_data: a pointer to data which will be passed to the callback when invoked.
 *
 * Add an in-process callback function to an existing AccessibleEventListener.
 * Note that the callback function must live in the same address
 * space as the AccessibleEventListener implementation code, thus one should not
 * use this function to attach callbacks to a 'remote' event listener
 * (that is, one that was not created by a client call to
 * createAccessibleEventListener ();
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleEventListener_addCallback (AccessibleEventListener *listener,
				     AccessibleEventListenerCB callback,
				     void                     *user_data)
{
  cspi_event_listener_add_cb (listener, callback, user_data);
  return TRUE;
}

/**
 * AccessibleEventListener_unref:
 * @listener: a pointer to the #AccessibleEventListener being operated on.
 *
 * Decrements an #AccessibleEventListener's reference count.
 **/
void
AccessibleEventListener_unref (AccessibleEventListener *listener)
{
  cspi_event_listener_unref (listener);
}

/**
 * AccessibleEventListener_removeCallback:
 * @listener: the #AccessibleEventListener instance to modify.
 * @callback: an #AccessibleEventListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleEventListener_removeCallback (AccessibleEventListener  *listener,
					AccessibleEventListenerCB callback)
{
  cspi_event_listener_remove_cb (listener, callback);
  return TRUE;
}

/**
 * SPI_createAccessibleKeystrokeListener:
 * @callback : an #AccessibleKeystrokeListenerCB callback function, or NULL.
 * @user_data: a pointer to data which will be passed to the callback when invoked.
 *
 * Create a new #AccessibleKeystrokeListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #AccessibleKeystrokeListener.
 *
 **/
AccessibleKeystrokeListener *
SPI_createAccessibleKeystrokeListener (AccessibleKeystrokeListenerCB callback,
				       void                         *user_data)
{
  AccessibleDeviceListener *listener = cspi_device_listener_new ();
  if (callback)
    {
      AccessibleDeviceListener_addCallback (listener, callback, user_data);
    }
  return listener;
}

/**
 * AccessibleKeystrokeListener_addCallback:
 * @listener: the #AccessibleKeystrokeListener instance to modify.
 * @callback: an #AccessibleKeystrokeListenerCB function pointer.
 * @user_data: a pointer to data which will be passed to the callback when invoked.
 *
 * Add an in-process callback function to an existing #AccessibleKeystrokeListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleKeystrokeListener_addCallback (AccessibleKeystrokeListener *listener,
					 AccessibleKeystrokeListenerCB callback,
					 void                         *user_data)
{
  cspi_device_listener_add_cb (listener, callback, user_data);
  return TRUE;
}

/**
 * AccessibleKeystrokeListener_removeCallback:
 * @listener: the #AccessibleKeystrokeListener instance to modify.
 * @callback: an #AccessibleKeystrokeListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing #AccessibleKeystrokeListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleKeystrokeListener_removeCallback (AccessibleKeystrokeListener *listener,
					    AccessibleKeystrokeListenerCB callback)
{
  cspi_device_listener_remove_cb (listener, callback);
  return TRUE;
}

/**
 * AccessibleKeystrokeListener_unref:
 * @listener: a pointer to the #AccessibleKeystrokeListener being operated on.
 *
 * Decrements an #AccessibleKeystrokeListener's reference count.
 **/
void
AccessibleKeystrokeListener_unref (AccessibleKeystrokeListener *listener)
{
  cspi_device_listener_unref (listener);
}

/**
 * SPI_createAccessibleDeviceListener:
 * @callback : an #AccessibleDeviceListenerCB callback function, or NULL.
 * @user_data: a pointer to data which will be passed to the callback when invoked.
 *
 * Create a new #AccessibleDeviceListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #AccessibleDeviceListener.
 *
 **/
AccessibleDeviceListener *
SPI_createAccessibleDeviceListener (AccessibleDeviceListenerCB callback,
				       void                         *user_data)
{
  AccessibleDeviceListener *listener = cspi_device_listener_new ();
  if (callback)
    {
      AccessibleDeviceListener_addCallback (listener, callback, user_data);
    }
  return listener;
}

/**
 * AccessibleDeviceListener_addCallback:
 * @listener: the #AccessibleDeviceListener instance to modify.
 * @callback: an #AccessibleDeviceListenerCB function pointer.
 * @user_data: a pointer to data which will be passed to the callback when invoked.
 *
 * Add an in-process callback function to an existing #AccessibleDeviceListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleDeviceListener_addCallback (AccessibleDeviceListener *listener,
					 AccessibleDeviceListenerCB callback,
					 void                         *user_data)
{
  cspi_device_listener_add_cb (listener, callback, user_data);
  return TRUE;
}

/**
 * AccessibleDeviceListener_removeCallback:
 * @listener: the #AccessibleDeviceListener instance to modify.
 * @callback: an #AccessibleDeviceListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing #AccessibleDeviceListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleDeviceListener_removeCallback (AccessibleDeviceListener *listener,
					    AccessibleDeviceListenerCB callback)
{
  cspi_device_listener_remove_cb (listener, callback);
  return TRUE;
}

/**
 * AccessibleDeviceListener_unref:
 * @listener: a pointer to the #AccessibleDeviceListener being operated on.
 *
 * Decrements an #AccessibleDeviceListener's reference count.
 **/
void
AccessibleDeviceListener_unref (AccessibleDeviceListener *listener)
{
  cspi_device_listener_unref (listener);
}
