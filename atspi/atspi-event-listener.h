/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 *           
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

#ifndef _ATSPI_EVENT_LISTENER_H_
#define _ATSPI_EVENT_LISTENER_H_

#include "glib-object.h"

#include "atspi-event-types.h"

/**
 * AtspiEventListenerCB:
 * @event: The event for which notification is sent.
 * @user_data: User data which is passed to the callback each time a notification takes place.
 *
 * A function prototype for callbacks via which clients are notified of AT-SPI events.
 * 
 **/
typedef void       (*AtspiEventListenerCB)     (const AtspiEvent     *event,
						     void                      *user_data);

gboolean
atspi_event_listener_register (AtspiEventListenerCB callback,
				 void *user_data,
				 const gchar              *event_type);

gboolean
atspi_event_listener_deregister (AtspiEventListenerCB callback,
				   void *user_data,
				   const gchar              *event_type);
#endif	/* _ATSPI_EVENT_LISTENER_H_ */
