#ifndef _SPI_LISTENER_H_
#define _SPI_LISTENER_H_

#include <libspi/accessibleeventlistener.h>
#include <libspi/keystrokelistener.h>

G_BEGIN_DECLS

/*
 *
 * Structure used to encapsulate event information
 *
 */

typedef Accessibility_Event AccessibleEvent;

/*
 *
 * Function prototype typedefs for Event Listener Callbacks.
 * (see libspi/accessibleeventlistener.h for definition of SpiVoidEventListenerCB).
 *
 * usage: signatures should be
 * void (*AccessibleEventListenerCB) (AccessibleEvent *event);
 *
 * boolean (*AccessibleKeystrokeListenerCB) (AccessibleKeystrokeEvent *Event);
 */

typedef VoidSpiEventListenerCB AccessibleEventListenerCB;
typedef BooleanKeystrokeListenerCB    AccessibleKeystrokeListenerCB;

G_END_DECLS

#endif
