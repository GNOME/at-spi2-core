#ifndef _SPI_SPI_LISTENER_H_
#define _SPI_SPI_LISTENER_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "accessibleeventlistener.h"
#include "keystrokelistener.h"


/*
 *
 * Structure used to encapsulate event information
 *
 */

typedef Accessibility_Event SpiAccessibleEvent;

/*
 *
 * Function prototype typedefs for Event SpiListener Callbacks.
 * (see libspi/accessibleeventlistener.h for definition of VoidEventListenerCB).
 *
 * usage: signatures should be
 * void (*SpiAccessibleEventListenerCB) (SpiAccessibleEvent *event);
 *
 * boolean (*KeystrokeListenerCB) (KeystrokeEvent *Event);
 */

typedef VoidEventListenerCB SpiAccessibleEventListenerCB;
typedef BooleanKeystrokeListenerCB KeystrokeListenerCB;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
