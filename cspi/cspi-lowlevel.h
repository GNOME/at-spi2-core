#ifndef _SPI_LOWLEVEL_H_
#define _SPI_LOWLEVEL_H_

/*
 * Private internal - details of the lowlevel at-spi
 * implementation abstraction
 *
 * These methods are implemented in cspi/bonobo/
 */

#include "cspi/spi-private.h"

/* Misc CORBA / bonobo bits */

SPIBoolean   cspi_check_ev       (const char *error_string);
void         cspi_dup_ref        (CORBA_Object object);
void         cspi_release_unref  (CORBA_Object object);

/* Listener bits */

CORBA_Object cspi_event_listener_new           (void);
void         cspi_event_listener_add_cb        (AccessibleEventListener      *listener,
						AccessibleEventListenerCB     callback,
						void                         *user_data);
void         cspi_event_listener_remove_cb     (AccessibleEventListener      *listener,
						AccessibleEventListenerCB     callback);

CORBA_Object cspi_keystroke_listener_new       (void);
void         cspi_keystroke_listener_add_cb    (AccessibleKeystrokeListener  *listener,
						AccessibleKeystrokeListenerCB callback,
						void                         *user_data);
void         cspi_keystroke_listener_remove_cb (AccessibleKeystrokeListener  *listener,
						AccessibleKeystrokeListenerCB callback);

#endif /* _SPI_LOWLEVEL_H_ */
