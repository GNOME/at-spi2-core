#ifndef _SPI_LISTENER_H_
#define _SPI_LISTENER_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "keystrokelistener.h"
#include "accessibleeventlistener.h"

/*
 *
 * Structure used to encapsulate event information
 *
 */

typedef Accessibility_Event AccessibleEvent;

/*
 *
 * Function prototype typedefs for Event Listener Callbacks.
 * (see libspi/accessibleeventlistener.h for definition of VoidEventListenerCB).
 *
 * usage: signatures should be
 * void (*AccessibleEventListenerCB) (AccessibleEvent *event);
 *
 * boolean (*KeystrokeListenerCB) (KeystrokeEvent *Event);
 */

typedef VoidEventListenerCB AccessibleEventListenerCB;
typedef BooleanKeystrokeListenerCB KeystrokeListenerCB;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
