/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
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
CORBA_Object cspi_dup_ref            (CORBA_Object object);
void         cspi_release_unref      (CORBA_Object object);
char        *cspi_exception_get_text (void);
CORBA_Object cspi_init               (void);
SPIBoolean   cspi_ping               (CORBA_Object object);
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
