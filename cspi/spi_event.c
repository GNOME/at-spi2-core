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

static GSList *_cspi_event_queue = NULL;

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

static char *
cspi_internal_event_get_text (const InternalEvent *e)
{
  CORBA_any *any;
  g_return_val_if_fail (e, NULL);
  g_return_val_if_fail (e->data, NULL);
  any = (CORBA_any *) e->data;
  if (CORBA_TypeCode_equivalent (any->_type, TC_CORBA_string, NULL)) 
    {
      return * (char **) any->_value;
    } 
  else
    {
#ifdef EVENT_CONTEXT_DEBUG
      fprintf (stderr, "requested string, TC is not TC_CORBA_string! (%u)\n",
	       (unsigned) any->_type);
#endif
      return NULL;
    }
}

static Accessible *
cspi_internal_event_get_object (const InternalEvent *e)
{
  CORBA_any *any;
  Accessible *accessible;
  g_return_val_if_fail (e, NULL);
  g_return_val_if_fail (e->data, NULL);
  any = (CORBA_any *) e->data;
  if (CORBA_TypeCode_equal (any->_type, TC_CORBA_Object, cspi_ev()))
    return cspi_object_take (* (CORBA_Object *) any->_value);
  else 
    return NULL;
}

/**
 * AccessibleTextChangedEvent_getChangeString:
 * @event: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:text-changed", 
 *         returning the text inserted or deleted.
 *
 * Returns: a UTF-8 text string indicating the text inserted,
 *          deleted, or substituted by this event.
 **/
char *
AccessibleTextChangedEvent_getChangeString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type? expensive... */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleTextSelectionChangedEvent_getSelectionString:
 * @event: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:text-selection-changed", 
 *         returning the newly added, removed, or modified selection string.
 *
 * Returns: a UTF-8 text string indicating the recently changed selection.
 **/
char *
AccessibleTextSelectionChangedEvent_getSelectionString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type? expensive... */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleWindowEvent_getTitleString:
 * @event: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "window:", 
 *         returning the window title.
 *
 * Returns: a UTF-8 text string representing the title of the 
 *         recently changed window.
 **/
char *
AccessibleWindowEvent_getTitleString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type? expensive... */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleChildChangedEvent_getChildAccessible:
 * @event: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:children_changed"
 *         to get a reference to the changed #Accessible.
 *         Note that context #Accessibles are not guaranteed to outlive
 *         event delivery, in which case this call may return %NULL
 *         even if the object existed at the time of dispatch.
 *
 * Returns: the context #Accessible for the event, or %NULL if
 *          there is no longer a valid context #Accessible 
 *          object for the event.
 **/
Accessible *
AccessibleChildChangedEvent_getChildAccessible (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  return (Accessible *) cspi_internal_event_get_object (foo);
}

/**
 * AccessibleParentChangedEvent_getParentAccessible:
 * @event: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:parent_changed"
 *         to get a reference to the changed #Accessible.
 *         Note that context #Accessibles are not guaranteed to outlive
 *         event delivery, in which case this call may return %NULL
 *         even if the object existed at the time of dispatch.
 *
 * Returns: an #Accessible pointer representing the new parent object.
 **/
Accessible *
AccessibleParentChangedEvent_getParentAccessible (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  return (Accessible *) cspi_internal_event_get_object (foo);
}

/** NEED TO DOCUMENT THESE **/

Accessible *
AccessibleActiveDescendantChangedEvent_getActiveDescendant (const AccessibleEvent *e) 
{
  const InternalEvent *foo = (InternalEvent *) e;
  return (Accessible *) cspi_internal_event_get_object (foo);
}

Accessible *
AccessibleTableSummaryChangedEvent_getSummaryAccessible (const AccessibleEvent *e) 
{
  const InternalEvent *foo = (InternalEvent *) e;
  return (Accessible *) cspi_internal_event_get_object (foo);
}

Accessible *
AccessibleTableHeaderChangedEvent_getHeaderAccessible (const AccessibleEvent *e)
{
  return NULL;
}


char *
AccessibleTableCaptionChangedEvent_getCaptionString (const AccessibleEvent *e)
{
  return NULL;
}

char *
AccessibleTableRowDescriptionChangedEvent_getDescriptionString (const AccessibleEvent *e)
{
  return NULL;
}

char *
AccessibleTableColumnDescriptionChangedEvent_getDescriptionString (const AccessibleEvent *e)
{
  return NULL;
}

char *
AccessibleDescriptionChangedEvent_getDescriptionString (const AccessibleEvent *e)
{
  return NULL;
}

static gint
cspi_event_compare (gconstpointer p1, gconstpointer p2)
{
  const InternalEvent *e1 = p1, *e2 = p2;
  return (gint) ((long) e2->id  - (long) e1->id);
}

static InternalEvent *
cspi_internal_event_lookup (const InternalEvent *e)
{
  InternalEvent *internal = NULL;
  GSList *p =
    g_slist_find_custom (_cspi_event_queue, e, cspi_event_compare);
  if (p)
    internal = p->data;
  return internal;
}

static const InternalEvent *
cspi_internal_event_check (const AccessibleEvent *e)
{
  InternalEvent *internal = (InternalEvent *) e;
  if (internal->magic == SPI_INTERNAL_EVENT_MAGIC) 
    return internal;
  else
    return NULL;
}

static InternalEvent *
cspi_internal_event_add (const InternalEvent *e)
{
  _cspi_event_queue = g_slist_prepend (_cspi_event_queue, (gpointer) e);
  return (InternalEvent *) e;
}

static void
cspi_internal_event_remove (const InternalEvent *e)
{
  GSList *link = g_slist_find_custom (_cspi_event_queue, e, cspi_event_compare);
  if (link)
    _cspi_event_queue = g_slist_remove_link (_cspi_event_queue, link);
}

char *
AccessibleNameChangedEvent_getNameString (const AccessibleEvent *e)
{
  return NULL;
}

SPIBoolean
AccessibleEvent_ref (const AccessibleEvent *e)
{
  const InternalEvent *private = cspi_internal_event_check (e);
  if (private)
    {
      InternalEvent *event = cspi_internal_event_lookup (private);
      /* 
       * put event in the cache if it's not there already, 
       * and increment refcount 
       */
      if (!event)
	{
	  event = cspi_internal_event_add (private);
	}
      event->ref_count++;
      return TRUE;
    }
  else
    return FALSE;
}

void
AccessibleEvent_unref (const AccessibleEvent *e)
{
  const InternalEvent *private = cspi_internal_event_check (e);
  /* decrement refcount and remove if appropriate */
  if (private)
    {
      InternalEvent *event = cspi_internal_event_lookup (private);
      if (event) 
	{
	  event->ref_count--;
	  if (event->ref_count < 1)
	    cspi_internal_event_remove (event);
	}
    }
}
