/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 * Copyright 2010, 2011 Novell, Inc.
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

#ifndef _ATSPI_EVENT_LISTENER_PRIVATE_H_
#define _ATSPI_EVENT_LISTENER_PRIVATE_H_

#include "atspi-event-listener.h"

#include "dbus/dbus.h"

G_BEGIN_DECLS

DBusHandlerResult _atspi_dbus_handle_Event (DBusConnection *bus, DBusMessage *message, void *data);

void _atspi_send_event (AtspiEvent *e);

DBusHandlerResult _atspi_dbus_handle_event (DBusConnection *bus, DBusMessage *message, void *data);

void
_atspi_reregister_event_listeners ();

G_END_DECLS

#endif	/* _ATSPI_EVENT_LISTENER_H_ */
