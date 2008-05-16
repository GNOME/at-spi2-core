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

/* devicelistener.c: implement the DeviceListener interface */

#include <config.h>
#ifdef SPI_DEBUG
#  include <stdio.h>
#endif
#include <libspi/listener.h>
#include <libspi/devicelistener.h>

/* Our parent Gtk object type  */
#define PARENT_TYPE BONOBO_TYPE_OBJECT

enum {
	DEVICE_EVENT,
	LAST_SIGNAL
};
static guint signals [LAST_SIGNAL];

/*
 * CORBA Accessibility::DeviceListener::keyEvent method implementation
 */
static CORBA_boolean
impl_device_event (PortableServer_Servant           servant,
		   const Accessibility_DeviceEvent *key,
		   CORBA_Environment               *ev)
{
  gboolean was_consumed = FALSE;
  SpiDeviceListener *listener = SPI_DEVICE_LISTENER (
	  bonobo_object_from_servant (servant));

  g_signal_emit (G_OBJECT (listener), signals [DEVICE_EVENT], 0, key, &was_consumed);

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
spi_device_listener_class_init (SpiDeviceListenerClass *klass)
{
  POA_Accessibility_DeviceEventListener__epv *epv = &klass->epv;
  
  signals [DEVICE_EVENT] = g_signal_new (
    "device_event",
    G_TYPE_FROM_CLASS (klass),
    G_SIGNAL_RUN_LAST,
    G_STRUCT_OFFSET (SpiDeviceListenerClass, device_event),
    boolean_handled_accumulator, NULL,
    marshal_BOOLEAN__POINTER,
    G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
  
  epv->notifyEvent = impl_device_event;
}

static void
spi_device_listener_init (SpiDeviceListener *device_listener)
{
}

BONOBO_TYPE_FUNC_FULL (SpiDeviceListener,
		       Accessibility_DeviceEventListener,
		       BONOBO_TYPE_OBJECT,
		       spi_device_listener)

SpiDeviceListener *
spi_device_listener_new (void)
{
    SpiDeviceListener *retval = g_object_new (
	    SPI_DEVICE_LISTENER_TYPE, NULL);
    return retval;
}
