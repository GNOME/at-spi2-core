#include <cspi/spi-private.h>
#include <cspi/spi-listener-impl.h>

/*
 * Standard event dispatcher
 */

BONOBO_CLASS_BOILERPLATE (CSpiEventListener, cspi_event_listener,
			  GObject, spi_event_listener_get_type ())

static void
cspi_event (SpiEventListener    *listener,
	    Accessibility_Event *event)
{
  GSList *l;
  CSpiEventListener *clistener = (CSpiEventListener *) listener;
  AccessibleEvent aevent;

  aevent.type    = event->type;
  aevent.source  = cspi_object_add (event->source);
  aevent.detail1 = event->detail1;
  aevent.detail2 = event->detail2;

  for (l = clistener->callbacks; l; l = l->next)
    {
      AccessibleEventListenerCB cb = l->data;
      cb (&aevent);
    }
  
  cspi_object_unref (aevent.source);
}

static void
cspi_event_listener_instance_init (CSpiEventListener *listener)
{
}

static void
cspi_event_listener_class_init (CSpiEventListenerClass *klass)
{
  klass->event = cspi_event;
}

CSpiEventListener *
cspi_event_listener_new (void)
{
  return g_object_new (cspi_event_listener_get_type (), NULL);
}

void
cspi_event_listener_add_callback (CSpiEventListener        *listener,
				  AccessibleEventListenerCB callback)
{
  g_return_if_fail (IS_SPI_ACCESSIBLE_EVENT_SPI_LISTENER (listener));
  listener->callbacks = g_slist_prepend (listener->callbacks, callback);
}

void
cspi_event_listener_remove_callback (CSpiEventListener        *listener,
				     AccessibleEventListenerCB callback)
{
  g_return_if_fail (IS_CSPI_KEYSTROKE_LISTENER (listener));
  listener->callbacks = g_slist_remove (listener->callbacks, callback);
}

/*
 * Key event dispatcher
 */

static gboolean
cspi_key_event (SpiKeystrokeListener          *listener,
		const Accessibility_KeyStroke *keystroke)
{
  GSList *l;
  CSpiKeystrokeListener *clistener = (CSpiKeystrokeListener *) listener;
  AccessibleKeystroke akeystroke;
  gboolean handled = FALSE;

  switch (keystroke->type)
    {
      case Accessibility_KEY_PRESSED:
	akeystroke.type = SPI_KEY_PRESSED;
	break;
      case Accessibility_KEY_RELEASED:
	akeystroke.type = SPI_KEY_RELEASED;
	break;
      default:
	akeystroke.type = 0;
	break;
    }
  akeystroke.keyID     = keystroke->keyID;
  akeystroke.keycode   = keystroke->keycode;
  akeystroke.timestamp = keystroke->timestamp;
  akeystroke.modifiers = keystroke->modifiers;

  for (l = clistener->callbacks; l; l = l->next)
    {
      AccessibleKeystrokeListenerCB cb = l->data;
      if ((handled = cb (&akeystroke)))
        {
	  break;
	}
    }
  
  return handled;
}

static void
cspi_keystroke_listener_init (CSpiKeystrokeListener *listener)
{
}

static void
cspi_keystroke_listener_class_init (CSpiKeystrokeListenerClass *klass)
{
  klass->key_event = cspi_key_event;
}

BONOBO_TYPE_FUNC (CSpiKeystrokeListener, 
		  spi_keystroke_listener_get_type (),
		  cspi_keystroke_listener);

CSpiKeystrokeListener *
cspi_keystroke_listener_new (void)
{
  return g_object_new (cspi_keystroke_listener_get_type (), NULL);
}

void
cspi_keystroke_listener_add_callback (CSpiKeystrokeListener        *listener,
				      AccessibleKeystrokeListenerCB callback)
{
  g_return_if_fail (IS_CSPI_KEYSTROKE_LISTENER (listener));
  listener->callbacks = g_slist_prepend (listener->callbacks, callback);
}

void
cspi_keystroke_listener_remove_callback (CSpiKeystrokeListener        *listener,
					 AccessibleKeystrokeListenerCB callback)
{
  g_return_if_fail (IS_CSPI_KEYSTROKE_LISTENER (listener));
  listener->callbacks = g_slist_remove (listener->callbacks, callback);
}
