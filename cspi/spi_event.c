/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
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
  if (CORBA_TypeCode_equivalent (any->_type, TC_Accessibility_EventDetails, NULL)) 
    {
      Accessibility_EventDetails *details = (Accessibility_EventDetails *)any->_value;
      if (CORBA_TypeCode_equal (details->any_data._type, TC_CORBA_string, cspi_ev()))
	  return CORBA_string_dup (* (char **) (details->any_data._value));
      else
	  return CORBA_string_dup ("");
      return CORBA_string_dup (* (char **) (details->any_data._value));
    }
  else if (CORBA_TypeCode_equivalent (any->_type, TC_CORBA_string, NULL)) 
    {
      return CORBA_string_dup (* (char **) any->_value);
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

  g_return_val_if_fail (e, NULL);
  g_return_val_if_fail (e->data, NULL);

  any = (CORBA_any *) e->data;
  if (CORBA_TypeCode_equivalent (any->_type, TC_Accessibility_EventDetails, NULL)) 
    {
      Accessibility_EventDetails *details = (Accessibility_EventDetails *)any->_value;
      if (CORBA_TypeCode_equal (details->any_data._type, TC_CORBA_Object, cspi_ev()))
	  return cspi_object_take (* (CORBA_Object *) (details->any_data._value));
      else
	  return NULL;
    }
  else if (CORBA_TypeCode_equal (any->_type, TC_CORBA_Object, cspi_ev()))
    return cspi_object_take (* (CORBA_Object *) any->_value);
  else 
    return NULL;
}

static SPIRect *
cspi_internal_event_get_rect (const InternalEvent *e)
{
  CORBA_any *any;
  g_return_val_if_fail (e, NULL);
  g_return_val_if_fail (e->data, NULL);
  any = (CORBA_any *) e->data;
  if (CORBA_TypeCode_equivalent (any->_type, TC_Accessibility_EventDetails, NULL)) 
    {
      Accessibility_EventDetails *details = (Accessibility_EventDetails *)any->_value;
      SPIRect *rect = g_new (SPIRect, 1);
      if (CORBA_TypeCode_equal (details->any_data._type, TC_Accessibility_BoundingBox, cspi_ev()))
      {
	  Accessibility_BoundingBox *bounds = (Accessibility_BoundingBox *) details->any_data._value;
	  rect->x = bounds->x;
	  rect->y = bounds->y;
	  rect->width = bounds->width;
	  rect->height = bounds->height;
	  return rect;
      }
      else
	  return NULL;
    }
  if (CORBA_TypeCode_equivalent (any->_type, TC_Accessibility_BoundingBox, NULL)) 
    {
      SPIRect *rect = g_new (SPIRect, 1);
      Accessibility_BoundingBox *bounds = (Accessibility_BoundingBox *) any->_value;
      rect->x = bounds->x;
      rect->y = bounds->y;
      rect->width = bounds->width;
      rect->height = bounds->height;
      return rect;
    } 
  else
    {
#ifdef EVENT_CONTEXT_DEBUG
      fprintf (stderr, "requested string, TC is not TC_Accessible_RectBounds! (%u)\n",
	       (unsigned) any->_type);
#endif
      return NULL;
    }
}

/**
 * AccessibleEvent_getSourceName:
 * @e: an #AccessibleEvent to be queried. 
 *
 * Get the 'accessible-name' of the object emitting the event.
 *
 * Returns: The name of the event source, or NULL if the event source cannot be identified
 *          or does not report a name.
 */
char*        AccessibleEvent_getSourceName (const AccessibleEvent *e)
{
    InternalEvent *ie = (InternalEvent *)e;
    CORBA_any *any = ((ie && ie->data) ? (CORBA_any *)ie->data : NULL);
    if (any &&
	CORBA_TypeCode_equivalent (any->_type, 
				   TC_Accessibility_EventDetails, NULL))
      {
	  Accessibility_EventDetails *details = (Accessibility_EventDetails *) any->_value;
	  return CORBA_string_dup (details->source_name);
      }
    else
	return NULL;
}

/**
 * AccessibleEvent_getSourceRole:
 * @e: an #AccessibleEvent to be queried. 
 *
 * Get the #AccessibleRole of the object emitting the event.
 *
 * Returns: #AccessibleRole of the event source, or SPI_ROLE_UNKNOWN
 *          if the event source's role is unknown or unspecified.
 *          (Some kinds of events, such as 'mouse:' events or
 *          toolkit events, don't have associated object roles.)
 */
AccessibleRole AccessibleEvent_getSourceRole (const AccessibleEvent *e)
{
    InternalEvent *ie = (InternalEvent *)e;
    CORBA_any *any = ((ie && ie->data) ? (CORBA_any *)ie->data : NULL);
    if (any &&
	CORBA_TypeCode_equivalent (any->_type, 
				   TC_Accessibility_EventDetails, NULL))
      {
	  Accessibility_EventDetails *details = (Accessibility_EventDetails *) any->_value;
	  return cspi_role_from_spi_role (details->source_role);
      }
    else
	return SPI_ROLE_UNKNOWN;
}

/**
 * AccessibleEvent_getSourceApplication:
 * @e: an #AccessibleEvent to be queried. 
 *
 * Get the #Application hosting the object which emitted the event.
 *
 * Returns: A pointer to the host #Application contining the event source
 *          component.
 */
AccessibleApplication* AccessibleEvent_getSourceApplication (const AccessibleEvent *e)
{
    InternalEvent *ie = (InternalEvent *)e;
    CORBA_any *any = ((ie && ie->data) ? (CORBA_any *)ie->data : NULL);
    if (any &&
	CORBA_TypeCode_equivalent (any->_type, 
				   TC_Accessibility_EventDetails, NULL))
      {
	  Accessibility_EventDetails *details = (Accessibility_EventDetails *) any->_value;
	  return  cspi_object_take (details->host_application);
      }
    else
	return NULL;
}

/**
 * AccessibleEvent_getSourceDetails:
 * @e: an #AccessibleEvent to be queried. 
 * @name: a pointer to a character string which will point to the name of the event source 
 * on successful completion of the call.
 * @role: a pointer to an #AccessibleRole which will point to the role of the event source
 * on successful completion of the call.
 * @app: A pointer to an #AccessibleApplication which points to the host application for this event
 * on successful completion of the call.
 *
 * Get the host #Application, "accessible name", and #AccessibleRole 
 * of the object which emitted the event.
 *
 * Returns: TRUE if the source details were successfully retrieved, 
 *          FALSE if they were not, either due to error, incomplete data,
 *          or the fact that the event did not encapsulate the required data.
 */
SPIBoolean   AccessibleEvent_getSourceDetails (const AccessibleEvent *e, 
					       char **name, AccessibleRole *role, 
					       AccessibleApplication **app)
{
    InternalEvent *ie = (InternalEvent *)e;
    CORBA_any *any = ((ie && ie->data) ? (CORBA_any *)ie->data : NULL);
    if (any &&
	CORBA_TypeCode_equivalent (any->_type, 
				   TC_Accessibility_EventDetails, NULL))
      {
	  Accessibility_EventDetails *details = (Accessibility_EventDetails *) any->_value;
	  *name = CORBA_string_dup (details->source_name);
	  *role = cspi_role_from_spi_role (details->source_role);
	  *app = cspi_object_take (details->host_application);
	  return TRUE;
      }
    else
      {
        *name = NULL;
	*role = SPI_ROLE_UNKNOWN;
	*app = NULL;
	return FALSE;
      }
}

/**
 * AccessibleTextChangedEvent_getChangeString:
 * @e: a pointer to the #AccessibleEvent being queried.
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
  /* TODO: check the event type. */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleTextSelectionChangedEvent_getSelectionString:
 * @e: a pointer to the #AccessibleEvent being queried.
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
  /* TODO: check the event type. */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleWindowEvent_getTitleString:
 * @e: a pointer to the #AccessibleEvent being queried.
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
  /* TODO: check the event type. */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleChildChangedEvent_getChildAccessible:
 * @e: a pointer to the #AccessibleEvent being queried.
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
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:property-change:accessible-parent"
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

/**
 * AccessibleActiveDescendantChangedEvent_getActiveDescendant:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:active-descendant-changed"
 *         to get a reference to the changed #Accessible.
 *         Note that context #Accessibles are not guaranteed to outlive
 *         event delivery, in which case this call may return %NULL
 *         even if the object existed at the time of dispatch.
 *
 * Returns: an #Accessible pointer representing the new active descendant.
 **/
Accessible *
AccessibleActiveDescendantChangedEvent_getActiveDescendant (const AccessibleEvent *e) 
{
  const InternalEvent *foo = (InternalEvent *) e;
  return (Accessible *) cspi_internal_event_get_object (foo);
}

/**
 * AccessibleTableSummaryChangedEvent_getSummaryAccessible:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:property-changed:accessible-table-summary"
 *         to get a reference to the changed #Accessible.
 *         Note that context #Accessibles are not guaranteed to outlive
 *         event delivery, in which case this call may return %NULL
 *         even if the object existed at the time of dispatch.
 *
 * Returns: an #Accessible pointer representing the new table summary.
 **/
Accessible *
AccessibleTableSummaryChangedEvent_getSummaryAccessible (const AccessibleEvent *e) 
{
  const InternalEvent *foo = (InternalEvent *) e;
  return (Accessible *) cspi_internal_event_get_object (foo);
}

/**
 * AccessibleTableHeaderChangedEvent_getHeaderAccessible:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type 
 *         "object:property-changed:accessible-table-row-header" or
 *         "object:property-changed:accessible-table-column-header"
 *         to get a reference to the changed #Accessible.
 *         Note that context #Accessibles are not guaranteed to outlive
 *         event delivery, in which case this call may return %NULL
 *         even if the object existed at the time of dispatch.
 *
 * Returns: an #Accessible pointer representing the new table header.
 **/
Accessible *
AccessibleTableHeaderChangedEvent_getHeaderAccessible (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  return (Accessible *) cspi_internal_event_get_object (foo);
}


/**
 * AccessibleTableCaptionChangedEvent_getCaptionString:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type 
 *         "object:property-changed:accessible-table-caption-object" 
 *         returning the text in the caption, if present.
 *
 * Returns: a UTF-8 text string indicating the text in the caption.
 **/
char *
AccessibleTableCaptionChangedEvent_getCaptionString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type. */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleTableRowDescriptionChangedEvent_getDescriptionString:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type 
 *         "object:property-changed:accessible-table-row-description" 
 *         returning the new table row description.
 *
 * Returns: a UTF-8 text string representing the recently changed
 *         table row description 
 **/
char *
AccessibleTableRowDescriptionChangedEvent_getDescriptionString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type. */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleTableColumnDescriptionChangedEvent_getDescriptionString:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type 
 *         "object:property-changed:accessible-table-column-description" 
 *         returning the new table column description.
 *
 * Returns: a UTF-8 text string representing the recently changed
 *         table column description 
 **/
char *
AccessibleTableColumnDescriptionChangedEvent_getDescriptionString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type. */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleDescriptionChangedEvent_getDescriptionString:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type 
 *         "object:property-changed:accessible-description" 
 *         returning the new description.
 *
 * Returns: a UTF-8 text string representing the recently changed
 *         description 
 **/
char *
AccessibleDescriptionChangedEvent_getDescriptionString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type. */
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleBoundsChangedEvent_getNewBounds:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:bounds-changed", 
 *         returning a pointer to an SPIRect structure containing the
 *         new bounds, or NULL on error.
 *         The returned structure should be freed with SPI_freeRect when 
 *         the caller has finished referencing it.
 *
 * @Since: AT-SPI 1.6
 *
 * Returns: a pointer to an SPIRect defining the new object bounds.
 **/
SPIRect *
AccessibleBoundsChangedEvent_getNewBounds (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  /* TODO: check the event type. */
  return cspi_internal_event_get_rect (foo);
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

/**
 * AccessibleNameChangedEvent_getNameString:
 * @e: a pointer to the #AccessibleEvent being queried.
 *
 * Queries an #AccessibleEvent of type "object:property-change:accessible_name:", 
 *         returning the name.
 *
 * Returns: a UTF-8 text string representing the name of the 
 *         object which recently changed.
 **/
char *
AccessibleNameChangedEvent_getNameString (const AccessibleEvent *e)
{
  const InternalEvent *foo = (InternalEvent *) e;
  return cspi_internal_event_get_text (foo);
}

/**
 * AccessibleEvent_ref:
 * @e: a pointer to the #AccessibleEvent being referenced.
 *
 * Increments by 1 the reference count of the event
 *
 * Returns: TRUE if the function succeeded; FALSE if the pointer is not a
 *         valid event.
 **/
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

/**
 * AccessibleEvent_unref:
 * @e: a pointer to the #AccessibleEvent being referenced.
 *
 * Decrements by 1 the reference count of the event. The event is destroyed
 * when the reference count recahes zero.
 *
 **/
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
            {
	      cspi_internal_event_remove (event);
              g_free ((gpointer)e->type);
              Accessible_unref (e->source);
              CORBA_free (event->data);
              g_free ((gpointer)e);
            }
	}
    }
}
