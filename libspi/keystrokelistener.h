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

#ifndef KEYSTROKE_SPI_LISTENER_H_
#define KEYSTROKE_SPI_LISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <bonobo/bonobo-object.h>
#include <atk/atkobject.h>
#include <libspi/Accessibility.h>
#include "keymasks.h"

#define KEYSTROKE_SPI_LISTENER_TYPE        (keystroke_spi_listener_get_type ())
#define KEYSTROKE_SPI_LISTENER(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), KEYSTROKE_SPI_LISTENER_TYPE, KeystrokeListener))
#define KEYSTROKE_SPI_LISTENER_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), KEYSTROKE_SPI_LISTENER_TYPE, KeystrokeListenerClass))
#define IS_KEYSTROKE_SPI_LISTENER(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), KEYSTROKE_SPI_LISTENER_TYPE))
#define IS_KEYSTROKE_SPI_LISTENER_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), KEYSTROKE_SPI_LISTENER_TYPE))

typedef gboolean (*BooleanKeystrokeListenerCB) (void *keystroke_ptr);

typedef struct {
        BonoboObject parent;
	GList *callbacks;
} KeystrokeListener;

typedef struct {
        BonoboObjectClass parent_class;
        POA_Accessibility_KeystrokeListener__epv epv;
} KeystrokeListenerClass;

GType               keystroke_spi_listener_get_type   (void);
KeystrokeListener  *keystroke_spi_listener_new       (void);
void   keystroke_spi_listener_add_callback (KeystrokeListener *listener,
                                        BooleanKeystrokeListenerCB callback);
void   keystroke_spi_listener_remove_callback (KeystrokeListener *listener,
                                           BooleanKeystrokeListenerCB callback);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KEYSTROKE_SPI_LISTENER_H_ */
