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

SPIBoolean   cspi_check_ev           (const char *error_string);
void         cspi_dup_ref            (CORBA_Object object);
void         cspi_release_unref      (CORBA_Object object);
char        *cspi_exception_get_text (void);
CORBA_Object cspi_init               (void);
void         cspi_main               (void);
void         cspi_main_quit          (void);

/* Listener bits */

gpointer     cspi_event_listener_new           (void);
void         cspi_event_listener_unref         (AccessibleEventListener      *listener);
CORBA_Object cspi_event_listener_get_corba     (AccessibleEventListener      *listener);
void         cspi_event_listener_add_cb        (AccessibleEventListener      *listener,
						AccessibleEventListenerCB     callback,
						void                         *user_data);
void         cspi_event_listener_remove_cb     (AccessibleEventListener      *listener,
						AccessibleEventListenerCB     callback);

gpointer     cspi_keystroke_listener_new       (void);
void         cspi_keystroke_listener_unref     (AccessibleKeystrokeListener  *listener);
CORBA_Object cspi_keystroke_listener_get_corba (AccessibleKeystrokeListener  *listener);
void         cspi_keystroke_listener_add_cb    (AccessibleKeystrokeListener  *listener,
						AccessibleKeystrokeListenerCB callback,
						void                         *user_data);
void         cspi_keystroke_listener_remove_cb (AccessibleKeystrokeListener  *listener,
						AccessibleKeystrokeListenerCB callback);

#endif /* _SPI_LOWLEVEL_H_ */
