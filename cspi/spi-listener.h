#ifndef _SPI_LISTENER_H_
#define _SPI_LISTENER_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "accessibleeventlistener.h"
#include "keystrokelistener.h"

#define SPI_KEYSET_ALL_KEYS ((void *)NULL)


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

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
