/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002, 2003 Sun Microsystems Inc.,
 * Copyright 2001, 2002, 2003 Ximian, Inc.
 * Copyright 2008, 2009, 2010 Codethink Ltd.
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

#ifndef BRIDGE_H
#define BRIDGE_H

#include <atk/atk.h>
#include <droute/droute.h>

typedef struct _SpiBridge SpiBridge;
typedef struct _SpiBridgeClass SpiBridgeClass;

G_BEGIN_DECLS

typedef struct _event_data event_data;
struct _event_data
{
  gchar *bus_name;
  gchar **data;
};

struct _SpiBridge
{
  GObject parent;

  AtkObject *root;

  DBusConnection *bus;
  DRouteContext  *droute;
  GMainContext *main_context;
  DBusServer *server;
  GList *direct_connections;

/*
  SpiRegister *reg;
  SpiCache    *cache;
  SpiLeasing  *leasing;
*/
  gchar *desktop_name;
  gchar *desktop_path;
gchar *app_tmp_dir;
gchar *app_bus_addr;
  GList *events;
  gboolean events_initialized;
};

extern SpiBridge *spi_global_app_data;

void spi_atk_add_client (const char *bus_name);
void spi_atk_remove_client (const char *bus_name);

int spi_atk_create_socket (SpiBridge *app);

G_END_DECLS

#endif /* BRIDGE_H */
