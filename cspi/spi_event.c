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

/**
 * createAccessibleEventListener:
 * @callback : an #AccessibleEventListenerCB callback function, or NULL.
 *
 * Create a new #AccessibleEventListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #AccessibleEventListener.
 *
 **/
AccessibleEventListener *
createAccessibleEventListener (AccessibleEventListenerCB callback)
{
  AccessibleEventListener *listener = spi_accessible_event_listener_new ();
  if (callback)
    {
      spi_accessible_event_listener_add_callback (listener, callback);
    }
  return listener;
}

/**
 * AccessibleEventListener_addCallback:
 * @listener: the #AccessibleEventListener instance to modify.
 * @callback: an #AccessibleEventListenerCB function pointer.
 *
 * Add an in-process callback function to an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleEventListener_addCallback (AccessibleEventListener *listener,
				     AccessibleEventListenerCB callback)
{
  spi_accessible_event_listener_add_callback (listener, callback);
  return TRUE;
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
boolean
AccessibleEventListener_removeCallback (AccessibleEventListener *listener,
					AccessibleEventListenerCB callback)
{
  spi_accessible_event_listener_remove_callback (listener, callback);
  return TRUE;
}

/**
 * createAccessibleKeystrokeListener:
 * @callback : an #AccessibleKeystrokeListenerCB callback function, or NULL.
 *
 * Create a new #AccessibleKeystrokeListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #AccessibleKeystrokeListener.
 *
 **/
AccessibleKeystrokeListener *
createAccessibleKeystrokeListener (AccessibleKeystrokeListenerCB callback)
{
  SpiKeystrokeListener *listener = spi_keystroke_listener_new ();
  if (callback)
    {
      spi_keystroke_listener_add_callback (listener, callback);
    }
  return (AccessibleKeystrokeListener *)listener;
}

/**
 * AccessibleKeystrokeListener_addCallback:
 * @listener: the #AccessibleKeystrokeListener instance to modify.
 * @callback: an #AccessibleKeystrokeListenerCB function pointer.
 *
 * Add an in-process callback function to an existing #AccessibleKeystrokeListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
AccessibleKeystrokeListener_addCallback (AccessibleKeystrokeListener *listener,
					 AccessibleKeystrokeListenerCB callback)
{
  spi_keystroke_listener_add_callback (listener, callback);
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
boolean
AccessibleKeystrokeListener_removeCallback (AccessibleKeystrokeListener *listener,
					    AccessibleKeystrokeListenerCB callback)
{
  keystroke_listener_remove_callback (listener, callback);
  return TRUE;
}

