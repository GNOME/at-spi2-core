/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
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

#include <cspi/spi-private.h>

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
 * createAccessibleKeystrokeListener:
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
  AccessibleKeystrokeListener *listener = cspi_keystroke_listener_new ();
  if (callback)
    {
      AccessibleKeystrokeListener_addCallback (listener, callback, user_data);
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
  cspi_keystroke_listener_add_cb (listener, callback, user_data);
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
  cspi_keystroke_listener_remove_cb (listener, callback);
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
  cspi_keystroke_listener_unref (listener);
}
