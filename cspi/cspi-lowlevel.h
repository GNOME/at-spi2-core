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
 */

#include "cspi/spi-private.h"

SPIBoolean   cspi_check_ev           (const char *error_string);
char        *cspi_exception_get_text (void);
void         cspi_main               (void);
void         cspi_main_quit          (void);

/* Listener bits */

gpointer     cspi_event_listener_new           (void);
void         cspi_event_listener_unref         (AccessibleEventListener      *listener);
char *cspi_device_listener_get_path     (CSpiDeviceListener *listener);
void         cspi_event_listener_add_cb        (AccessibleEventListener      *listener,
						AccessibleEventListenerCB     callback,
						void                         *user_data);
void         cspi_event_listener_remove_cb     (AccessibleEventListener      *listener,
						AccessibleEventListenerCB     callback); 
gpointer     cspi_keystroke_listener_new       (void);
void         cspi_keystroke_listener_unref     (AccessibleKeystrokeListener  *listener);
char *cspi_keystroke_listener_get_path (AccessibleKeystrokeListener  *listener);
void         cspi_keystroke_listener_add_cb    (AccessibleKeystrokeListener  *listener,
						AccessibleKeystrokeListenerCB callback,
						void                         *user_data);
void         cspi_keystroke_listener_remove_cb (AccessibleKeystrokeListener  *listener,
						AccessibleKeystrokeListenerCB callback);

void cspi_dispatch_event (AccessibleEvent *e);
DBusHandlerResult cspi_dbus_handle_event (DBusConnection *bus, DBusMessage *message, void *user_data);
DBusHandlerResult cspi_dbus_handle_deviceEvent (DBusConnection *bus, DBusMessage *message, void *user_data);

DBusConnection *cspi_bus();
Accessible *cspi_ref_accessible (const char *app, const char *path);
Accessible *cspi_ref_related_accessible (Accessible *obj, const char *path);

dbus_bool_t cspi_dbus_call (Accessible *obj, const char *interface, const char *method, DBusError *error, const char *type, ...);
dbus_bool_t cspi_dbus_get_property (Accessible *obj, const char *interface, const char *name, DBusError *error, const char *type, void *data);

extern const char *spi_bus_registry;
extern const char *spi_interface_accessible;
extern const char *spi_interface_action;
extern const char *spi_interface_application;
extern const char *spi_interface_component;
extern const char *spi_interface_dec;
extern const char *spi_interface_desktop;
extern const char *spi_interface_device_event_listener;
extern const char *spi_interface_document;
extern const char *spi_interface_editable_text;
extern const char *spi_interface_hyperlink;
extern const char *spi_interface_hypertext;
extern const char *spi_interface_image;
extern const char *spi_interface_registry;
extern const char *spi_interface_selection;
extern const char *spi_interface_table;
extern const char *spi_interface_text;
extern const char *spi_interface_tree;
extern const char *spi_interface_value;
extern const char *spi_path_dec;
extern const char *spi_path_registry;

#endif /* _SPI_LOWLEVEL_H_ */
