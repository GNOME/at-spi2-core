/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#ifndef SPI_APPLICATION_H_
#define SPI_APPLICATION_H_

#include <libspi/accessible.h>

G_BEGIN_DECLS

#define SPI_APPLICATION_TYPE        (spi_application_get_type ())
#define SPI_APPLICATION(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_APPLICATION_TYPE, SpiApplication))
#define SPI_APPLICATION_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST((k), SPI_APPLICATION_TYPE, SpiApplicationClass))
#define SPI_IS_APPLICATION(o)       (G_TYPE_CHECK__INSTANCE_TYPE ((o), SPI_APPLICATION_TYPE))
#define SPI_IS_APPLICATION_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), SPI_APPLICATION_TYPE))

typedef struct {
        SpiAccessible parent;
        long id;
	GList *toolkit_listeners;
} SpiApplication;

typedef struct {
        SpiAccessibleClass parent_class;
        POA_Accessibility_Application__epv epv;
	GHashTable *toolkit_event_names;
	GHashTable *generic_event_names;
} SpiApplicationClass;

GType           spi_application_get_type (void);
SpiApplication *spi_application_new      (AtkObject    *app_root);
void            spi_emit_eventv          (GObject      *gobject,
					  unsigned long detail1,
					  unsigned long detail2,
					  const char   *format, ...);

G_END_DECLS

#endif /* SPI_APPLICATION_H_ */
