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
#include <libspi/keystrokelistener.h>

/* Our parent Gtk object type  */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

/* A pointer to our parent object class */
static GObjectClass *keystroke_listener_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
keystroke_listener_object_finalize (GObject *object)
{

#ifdef SPI_DEBUG
        fprintf(stderr, "keystroke_listener_object_finalize called\n");
#endif
        keystroke_listener_parent_class->finalize (object);
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

void   keystroke_listener_remove_callback (SpiKeystrokeListener *listener,
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
  while (callbacks)
  {
	  BooleanKeystrokeListenerCB cb = (BooleanKeystrokeListenerCB) callbacks->data;
	  was_consumed = (*cb) (key) || was_consumed;
	  callbacks = g_list_next (callbacks);
  }
  return was_consumed;
}

static void
spi_keystroke_listener_class_init (SpiKeystrokeListenerClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_KeystrokeListener__epv *epv = &klass->epv;
        keystroke_listener_parent_class = g_type_class_peek_parent (klass);

        object_class->finalize = keystroke_listener_object_finalize;

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
    SpiKeystrokeListener *retval =
               SPI_KEYSTROKE_LISTENER (g_object_new (spi_keystroke_listener_get_type (), NULL));
    return retval;
}
