
/**
 * createEventListener:
 * @callback : an #SpiAccessibleEventListenerCB callback function, or NULL.
 *
 * Create a new #SpiAccessibleEventListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #SpiAccessibleEventListener.
 *
 **/
SpiAccessibleEventListener *
createEventListener (SpiAccessibleEventListenerCB callback)
{
  SpiAccessibleEventListener *listener = spi_accessible_event_spi_listener_new ();
  if (callback)
    {
      spi_accessible_event_spi_listener_add_callback (listener, callback);
    }
  return listener;
}

/**
 * EventListener_addCallback:
 * @listener: the #SpiAccessibleEventListener instance to modify.
 * @callback: an #SpiAccessibleEventListenerCB function pointer.
 *
 * Add an in-process callback function to an existing SpiAccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_addCallback (SpiAccessibleEventListener *listener,
                           SpiAccessibleEventListenerCB callback)
{
  spi_accessible_event_spi_listener_add_callback (listener, callback);
  return TRUE;
}

/**
 * EventListener_removeCallback:
 * @listener: the #SpiAccessibleEventListener instance to modify.
 * @callback: an #SpiAccessibleEventListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing SpiAccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_removeCallback (SpiAccessibleEventListener *listener,
                           SpiAccessibleEventListenerCB callback)
{
  spi_accessible_event_spi_listener_remove_callback (listener, callback);
  return TRUE;
}

/**
 * createKeystrokeListener:
 * @callback : an #KeystrokeListenerCB callback function, or NULL.
 *
 * Create a new #KeystrokeListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #KeystrokeListener.
 *
 **/
KeystrokeListener *
createKeystrokeListener (KeystrokeListenerCB callback)
{
  KeystrokeListener *listener = keystroke_spi_listener_new ();
  if (callback)
    {
      keystroke_spi_listener_add_callback (listener, callback);
    }
  return listener;
}

/**
 * KeystrokeListener_addCallback:
 * @listener: the #KeystrokeListener instance to modify.
 * @callback: an #KeystrokeListenerCB function pointer.
 *
 * Add an in-process callback function to an existing #KeystrokeListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
KeystrokeListener_addCallback (KeystrokeListener *listener,
                           KeystrokeListenerCB callback)
{
  keystroke_spi_listener_add_callback (listener, callback);
  return TRUE;
}

/**
 * KeystrokeListener_removeCallback:
 * @listener: the #KeystrokeListener instance to modify.
 * @callback: an #KeystrokeListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing KeystrokeListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
KeystrokeListener_removeCallback (KeystrokeListener *listener,
				  KeystrokeListenerCB callback)
{
  keystroke_spi_listener_remove_callback (listener, callback);
  return TRUE;
}

