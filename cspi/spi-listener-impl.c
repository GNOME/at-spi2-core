#include <cspi/spi-private.h>
#include <cspi/spi-listener-impl.h>

typedef struct
{
  union
    {
      AccessibleEventListenerCB     event;
      AccessibleKeystrokeListenerCB key_event;
      gpointer                      method;
    } cb;
  gpointer user_data;
} EventHandler;

GObjectClass *event_parent_class;
GObjectClass *keystroke_parent_class;

/*
 * Misc. helpers.
 */

static EventHandler *
event_handler_new (gpointer method, gpointer user_data)
{
  EventHandler *eh = g_new0 (EventHandler, 1);

  eh->cb.method = method;
  eh->user_data = user_data;

  return eh;
}

static void
event_handler_free (EventHandler *handler)
{
  g_free (handler);
}

static GList *
event_list_remove_by_callback (GList *list, gpointer callback)
{
  GList *l, *next;
	
  for (l = list; l; l = next)
    {
      EventHandler *eh = l->data;
      next = l->next;
      
      list = g_list_delete_link (list, l);
      
      event_handler_free (eh);
    }

  return list;
}

/*
 * Standard event dispatcher
 */

BONOBO_CLASS_BOILERPLATE (CSpiEventListener, cspi_event_listener,
			  GObject, spi_event_listener_get_type ())

static void
cspi_event (SpiEventListener    *listener,
	    Accessibility_Event *event)
{
  GList *l;
  CSpiEventListener *clistener = (CSpiEventListener *) listener;
  AccessibleEvent    aevent;
  Accessible        *source;

  source = cspi_object_add_check (bonobo_object_dup_ref (event->source, cspi_ev ()));

  aevent.type    = event->type;
  aevent.source  = source;
  aevent.detail1 = event->detail1;
  aevent.detail2 = event->detail2;

  /* FIXME: re-enterancy hazard on this list */
  for (l = clistener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;

      eh->cb.event (&aevent, eh->user_data);
    }

  cspi_object_unref (source);
}

static void
cspi_event_listener_instance_init (CSpiEventListener *listener)
{
}

static void
cspi_event_listener_finalize (GObject *object)
{
  CSpiEventListener *listener = (CSpiEventListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      event_handler_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  event_parent_class->finalize (object);
}

static void
cspi_event_listener_class_init (CSpiEventListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  event_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = cspi_event_listener_finalize;

  klass->event = cspi_event;
}

CSpiEventListener *
cspi_event_listener_new (void)
{
  return g_object_new (cspi_event_listener_get_type (), NULL);
}

void
cspi_event_listener_add_callback (CSpiEventListener        *listener,
				  AccessibleEventListenerCB callback,
				  void                     *user_data)
{
  g_return_if_fail (CSPI_IS_EVENT_LISTENER (listener));
  listener->callbacks = g_list_prepend (listener->callbacks,
					event_handler_new (callback, user_data));
}

void
cspi_event_listener_remove_callback (CSpiEventListener        *listener,
				     AccessibleEventListenerCB callback)
{
  g_return_if_fail (CSPI_IS_EVENT_LISTENER (listener));
  listener->callbacks = event_list_remove_by_callback (listener->callbacks, callback);
}

/*
 * Key event dispatcher
 */

static gboolean
cspi_key_event (SpiKeystrokeListener          *listener,
		const Accessibility_KeyStroke *keystroke)
{
  GList *l;
  CSpiKeystrokeListener *clistener = (CSpiKeystrokeListener *) listener;
  AccessibleKeystroke akeystroke;
  gboolean handled = FALSE;

#ifdef SPI_KEYEVENT_DEBUG
  fprintf (stderr, "%s%c",
	   (keystroke->modifiers & SPI_KEYMASK_ALT)?"Alt-":"",
	   ((keystroke->modifiers & SPI_KEYMASK_SHIFT)^(keystroke->modifiers & SPI_KEYMASK_SHIFTLOCK))?
	   (char) toupper((int) keystroke->keyID) : (char) tolower((int) keystroke->keyID));
  
  fprintf (stderr, "Key:\tsym %ld\n\tmods %x\n\tcode %d\n\ttime %ld\n",
	   (long) keystroke->keyID,
	   (unsigned int) keystroke->modifiers,
	   (int) keystroke->keycode,
	   (long int) keystroke->timestamp);
#endif

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

  /* FIXME: re-enterancy hazard on this list */
  for (l = clistener->callbacks; l; l = l->next)
    {
      EventHandler *eh = l->data;

      if ((handled = eh->cb.key_event (&akeystroke, eh->user_data)))
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
cspi_keystroke_listener_finalize (GObject *object)
{
  CSpiKeystrokeListener *listener = (CSpiKeystrokeListener *) object;
  GList *l;
  
  for (l = listener->callbacks; l; l = l->next)
    {
      event_handler_free (l->data);
    }
  
  g_list_free (listener->callbacks);

  keystroke_parent_class->finalize (object);
}

static void
cspi_keystroke_listener_class_init (CSpiKeystrokeListenerClass *klass)
{
  GObjectClass *object_class = (GObjectClass *) klass;

  keystroke_parent_class = g_type_class_peek_parent (klass);
  object_class->finalize = cspi_keystroke_listener_finalize;

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
				      AccessibleKeystrokeListenerCB callback,
				      void                         *user_data)
{
  g_return_if_fail (CSPI_IS_KEYSTROKE_LISTENER (listener));
  listener->callbacks = g_list_prepend (listener->callbacks,
					event_handler_new (callback, user_data));
}

void
cspi_keystroke_listener_remove_callback (CSpiKeystrokeListener        *listener,
					 AccessibleKeystrokeListenerCB callback)
{
  g_return_if_fail (CSPI_IS_KEYSTROKE_LISTENER (listener));
  listener->callbacks = event_list_remove_by_callback (listener->callbacks, callback);
}
