
/**
 * createEventListener:
 * @callback : an #AccessibleEventListenerCB callback function, or NULL.
 *
 * Create a new #AccessibleEventListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #AccessibleEventListener.
 *
 **/
AccessibleEventListener *
createEventListener (AccessibleEventListenerCB callback)
{
  AccessibleEventListener *listener = accessible_event_listener_new ();
  if (callback)
    {
      accessible_event_listener_add_callback (listener, callback);
    }
  return listener;
}

/**
 * EventListener_addCallback:
 * @listener: the #AccessibleEventListener instance to modify.
 * @callback: an #AccessibleEventListenerCB function pointer.
 *
 * Add an in-process callback function to an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_addCallback (AccessibleEventListener *listener,
                           AccessibleEventListenerCB callback)
{
  accessible_event_listener_add_callback (listener, callback);
  return TRUE;
}

/**
 * EventListener_removeCallback:
 * @listener: the #AccessibleEventListener instance to modify.
 * @callback: an #AccessibleEventListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_removeCallback (AccessibleEventListener *listener,
                           AccessibleEventListenerCB callback)
{
  accessible_event_listener_remove_callback (listener, callback);
  return TRUE;
}
