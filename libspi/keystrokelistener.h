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

#ifndef SPI_KEYSTROKE_LISTENER_H_
#define SPI_KEYSTROKE_LISTENER_H_

#include <bonobo/bonobo-object.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>
#include <libspi/keymasks.h>

G_BEGIN_DECLS

#define SPI_KEYSTROKE_LISTENER_TYPE        (spi_keystroke_listener_get_type ())
#define SPI_KEYSTROKE_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_KEYSTROKE_LISTENER_TYPE, SpiKeystrokeListener))
#define SPI_KEYSTROKE_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_KEYSTROKE_LISTENER_TYPE, SpiKeystrokeListenerClass))
#define SPI_IS_KEYSTROKE_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_KEYSTROKE_LISTENER_TYPE))
#define SPI_IS_KEYSTROKE_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_KEYSTROKE_LISTENER_TYPE))

typedef struct _SpiKeystrokeListener SpiKeystrokeListener;

struct _SpiKeystrokeListener {
        BonoboObject parent;
};

typedef struct {
        BonoboObjectClass parent_class;
        POA_Accessibility_DeviceEventListener__epv epv;

	gboolean (*key_event) (SpiKeystrokeListener *listener,
			       const Accessibility_DeviceEvent *key);
} SpiKeystrokeListenerClass;

GType                  spi_keystroke_listener_get_type        (void);
SpiKeystrokeListener  *spi_keystroke_listener_new             (void);

G_END_DECLS

#endif /* KEYSTROKE_SPI_LISTENER_H_ */
