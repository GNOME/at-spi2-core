/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc.
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

#ifndef __SPI_LISTENER_IMPL_H__
#define __SPI_LISTENER_IMP_H__

#include <libspi/eventlistener.h>
#include <libspi/keystrokelistener.h>
#include <cspi/spi-impl.h>
#include <cspi/spi-listener.h>

G_BEGIN_DECLS

#define CSPI_EVENT_LISTENER_TYPE        (cspi_event_listener_get_type ())
#define CSPI_EVENT_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_EVENT_LISTENER_TYPE, CSpiEventListener))
#define CSPI_EVENT_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_EVENT_LISTENER_TYPE, CSpiEventListenerClass))
#define CSPI_IS_EVENT_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_EVENT_LISTENER_TYPE))
#define CSPI_IS_EVENT_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_EVENT_LISTENER_TYPE))

typedef struct {
	SpiEventListener parent;
	GList           *callbacks;
} CSpiEventListener;
typedef SpiEventListenerClass CSpiEventListenerClass;

GType cspi_event_listener_get_type (void);

#define CSPI_KEYSTROKE_LISTENER_TYPE        (cspi_keystroke_listener_get_type ())
#define CSPI_KEYSTROKE_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), CSPI_KEYSTROKE_LISTENER_TYPE, CSpiKeystrokeListener))
#define CSPI_KEYSTROKE_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), CSPI_KEYSTROKE_LISTENER_TYPE, CSpiKeystrokeListenerClass))
#define CSPI_IS_KEYSTROKE_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), CSPI_KEYSTROKE_LISTENER_TYPE))
#define CSPI_IS_KEYSTROKE_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), CSPI_KEYSTROKE_LISTENER_TYPE))

typedef struct {
	SpiKeystrokeListener parent;
	GList               *callbacks;
} CSpiKeystrokeListener;
typedef SpiKeystrokeListenerClass CSpiKeystrokeListenerClass;

GType cspi_keystroke_listener_get_type (void);

G_END_DECLS

#endif /* __SPI_LISTENER_IMPL_H__ */
