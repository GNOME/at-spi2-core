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

/* keystrokelistener.c: implement the KeystrokeListener interface */

#include <config.h>
#ifdef SPI_DEBUG
#  include <stdio.h>
#endif
#include <libspi/listener.h>
#include <libspi/keystrokelistener.h>

/* Our parent Gtk object type  */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

/* A pointer to our parent object class */
static BonoboObjectClass *keystroke_listener_parent_class;

enum {
	KEY_EVENT,
	LAST_SIGNAL
};
static guint signals [LAST_SIGNAL];

/* GObject::finalize */
static void
spi_keystroke_listener_object_finalize (GObject *object)
{
  SpiKeystrokeListener *listener = SPI_KEYSTROKE_LISTENER (object);

  g_list_free (listener->callbacks);
#ifdef SPI_DEBUG
  fprintf(stderr, "keystroke_listener_object_finalize called\n");
#endif
  ((GObjectClass *) keystroke_listener_parent_class)->finalize (object);
}

void   spi_keystroke_listener_add_callback (SpiKeystrokeListener *listener,
					    BooleanKeystrokeListenerCB callback)
{
  listener->callbacks = g_list_append (listener->callbacks, callback);
#ifdef SPI_DEBUG
        fprintf(stderr, "keystroke_listener_add_callback (%p) called\n",
		(gpointer) callback);
#endif
}

void
spi_keystroke_listener_remove_callback (SpiKeystrokeListener *listener,
					BooleanKeystrokeListenerCB callback)
{
  listener->callbacks = g_list_remove (listener->callbacks, callback);
}

/*
 * CORBA Accessibility::KeystrokeListener::keyEvent method implementation
 */
static CORBA_boolean
impl_key_event (PortableServer_Servant     servant,
		const Accessibility_KeyStroke *key,
		CORBA_Environment         *ev)
{
  SpiKeystrokeListener *listener = SPI_KEYSTROKE_LISTENER (bonobo_object_from_servant (servant));
  GList *callbacks = listener->callbacks;
  gboolean was_consumed = FALSE;

  g_signal_emit (G_OBJECT (listener), signals [KEY_EVENT], 0, key, &was_consumed);
  if (was_consumed)
    {
      return TRUE;
    }

#ifdef SPI_KEYEVENT_DEBUG
  if (ev->_major != CORBA_NO_EXCEPTION) {
    fprintf(stderr,
            ("Accessibility app error: exception during keystroke notification: %s\n"),
            CORBA_exception_id(ev));
    exit(-1);
  }
  else {
    fprintf(stderr, "%s%c",
	    (key->modifiers & SPI_KEYMASK_ALT)?"Alt-":"",
	    ((key->modifiers & SPI_KEYMASK_SHIFT)^(key->modifiers & SPI_KEYMASK_SHIFTLOCK))?
	    (char) toupper((int) key->keyID) : (char) tolower((int) key->keyID));
  }
#endif
  /* TODO: convert from the CORBA-based struct to a c-type-based one ? */
#ifdef SPI_KEYSTROKE_DEBUG  
    fprintf (stderr, "Key:\tsym %ld\n\tmods %x\n\tcode %d\n\ttime %ld\n",
	   (long) key->keyID,
	   (unsigned int) key->modifiers,
	   (int) key->keycode,
	   (long int) key->timestamp);
#endif
  while (callbacks)
  {
	  BooleanKeystrokeListenerCB cb = (BooleanKeystrokeListenerCB) callbacks->data;
	  was_consumed = (*cb) (key) || was_consumed;
	  callbacks = g_list_next (callbacks);
  }
  return was_consumed;
}


static gboolean
boolean_handled_accumulator (GSignalInvocationHint *ihint,
			     GValue                *return_accu,
			     const GValue          *handler_return,
			     gpointer               dummy)
{
  gboolean continue_emission;
  gboolean signal_handled;
  
  signal_handled = g_value_get_boolean (handler_return);
  g_value_set_boolean (return_accu, signal_handled);
  continue_emission = !signal_handled;
  
  return continue_emission;
}

void
marshal_BOOLEAN__POINTER (GClosure     *closure,
			  GValue       *return_value,
			  guint         n_param_values,
			  const GValue *param_values,
			  gpointer      invocation_hint,
			  gpointer      marshal_data)
{
  typedef gboolean (*GMarshalFunc_BOOLEAN__POINTER) (gpointer     data1,
                                                     gpointer     arg_1,
                                                     gpointer     data2);
  register GMarshalFunc_BOOLEAN__POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;
  gboolean v_return;

  g_return_if_fail (return_value != NULL);
  g_return_if_fail (n_param_values == 2);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_BOOLEAN__POINTER) (marshal_data ? marshal_data : cc->callback);

  v_return = callback (data1,
                       g_value_get_pointer (param_values + 1),
                       data2);

  g_value_set_boolean (return_value, v_return);
}

static void
spi_keystroke_listener_class_init (SpiKeystrokeListenerClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_KeystrokeListener__epv *epv = &klass->epv;
  keystroke_listener_parent_class = g_type_class_peek_parent (klass);
  
  signals [KEY_EVENT] = g_signal_new (
    "key_event",
    G_TYPE_FROM_CLASS (klass),
    G_SIGNAL_RUN_LAST,
    G_STRUCT_OFFSET (SpiKeystrokeListenerClass, key_event),
    boolean_handled_accumulator, NULL,
    marshal_BOOLEAN__POINTER,
    G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
  
  object_class->finalize = spi_keystroke_listener_object_finalize;
  
  epv->keyEvent = impl_key_event;
}

static void
spi_keystroke_listener_init (SpiKeystrokeListener *keystroke_listener)
{
  keystroke_listener->callbacks = NULL;
}

BONOBO_TYPE_FUNC_FULL (SpiKeystrokeListener,
		       Accessibility_KeystrokeListener,
		       BONOBO_TYPE_OBJECT,
		       spi_keystroke_listener);

SpiKeystrokeListener *
spi_keystroke_listener_new (void)
{
    SpiKeystrokeListener *retval = g_object_new (
	    SPI_KEYSTROKE_LISTENER_TYPE, NULL);
    return retval;
}
