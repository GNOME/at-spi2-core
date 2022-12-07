/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef REENTRANT_LIST_H_
#define REENTRANT_LIST_H_

#include <glib.h>

G_BEGIN_DECLS

typedef enum
{
  SPI_RE_ENTRANT_CONTINUE = 0,
  SPI_RE_ENTRANT_TERMINATE
} SpiReEntrantContinue;

typedef SpiReEntrantContinue (*SpiReEntrantFn) (GList *const *list,
                                                gpointer user_data);

void spi_re_entrant_list_delete_link (GList *const *element_ptr);
void spi_re_entrant_list_foreach (GList **list,
                                  SpiReEntrantFn func,
                                  gpointer user_data);

G_END_DECLS

#endif /* REENTRANT_LIST_H_ */
