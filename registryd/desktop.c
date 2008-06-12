/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc., Ximian Inc.
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

/* desktop.c: implements SpiDesktop.idl */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <atk/atkcomponent.h>
#include <gdk/gdkscreen.h>
#include <gdk/gdkx.h>

#include <spi-common/spi-dbus.h>

#include "desktop.h"
#include "registry.h"

G_DEFINE_TYPE(SpiDesktop, spi_desktop, G_TYPE_OBJECT)

/* SpiDesktop signals */
enum {
  APPLICATION_ADDED,
  APPLICATION_REMOVED,  
LAST_SIGNAL
};
static guint spi_desktop_signals[LAST_SIGNAL];


/* Our parent Gtk object type */
#define PARENT_TYPE SPI_ACCESSIBLE_TYPE

static gboolean exiting = FALSE;

/* A pointer to our parent object class */
static GObjectClass *parent_class;

#define SPI_TYPE_ATK_DESKTOP		(spi_atk_desktop_get_type ())
#define SPI_ATK_DESKTOP(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), SPI_TYPE_ATK_DESKTOP, SpiAtkDesktop))
#define SPI_IS_ATK_DESKTOP(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), SPI_TYPE_ATK_DESKTOP))

typedef struct {
	AtkObject parent;

	GdkScreen *screen;
} SpiAtkDesktop;

typedef struct {
	AtkObjectClass parent;
} SpiAtkDesktopClass;

static void spi_atk_desktop_init (SpiAtkDesktop *desktop);
static void atk_component_interface_init (AtkComponentIface *iface);
static void spi_atk_desktop_get_extents	 (AtkComponent    *component,
                                          gint            *x,
                                          gint            *y,
                                          gint            *width,
                                          gint            *height,
                                          AtkCoordType    coord_type);

static GType 
spi_atk_desktop_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
  	sizeof (SpiAtkDesktopClass),
  	(GBaseInitFunc) NULL,
  	(GBaseFinalizeFunc) NULL,
  	(GClassInitFunc) NULL,
  	(GClassFinalizeFunc) NULL,
  	NULL,
  	sizeof (SpiAtkDesktop),
  	0,
  	(GInstanceInitFunc) spi_atk_desktop_init,
      } ;
      static const GInterfaceInfo atk_component_info =
	{
        (GInterfaceInitFunc) atk_component_interface_init,
	(GInterfaceFinalizeFunc) NULL,
	NULL
      };

      type = g_type_register_static (ATK_TYPE_OBJECT,
                                     "SpiAtkDesktop", &typeInfo, 0);
      g_type_add_interface_static (type, ATK_TYPE_COMPONENT,
                                   &atk_component_info);
    }
  return type;
}

static void 
spi_atk_desktop_init (SpiAtkDesktop *desktop)
{
  GdkDisplay *display;

  atk_object_set_name (ATK_OBJECT (desktop), "main");
  display = gdk_x11_lookup_xdisplay (GDK_DISPLAY ());
  desktop->screen = gdk_display_get_default_screen (display);
}

static void
atk_component_interface_init (AtkComponentIface *iface)
{
  g_return_if_fail (iface != NULL);

  iface->get_extents = spi_atk_desktop_get_extents;
}

static void 
spi_atk_desktop_get_extents (AtkComponent *component,
                             gint         *x,
                             gint	  *y,
                             gint	  *width,
                             gint	  *height,
                             AtkCoordType coord_type)
{
  SpiAtkDesktop *desktop;

  g_return_if_fail (SPI_IS_ATK_DESKTOP (component));
  desktop = SPI_ATK_DESKTOP (component);
  *x = 0;
  *y = 0;
  *width = gdk_screen_get_width (desktop->screen);
  *height = gdk_screen_get_height (desktop->screen);
}

static void
spi_desktop_init (SpiDesktop *desktop)
{
  desktop->applications = NULL;
}

static void
spi_desktop_dispose (GObject *object)
{
  SpiDesktop *desktop = (SpiDesktop *) object;

  while (desktop->applications)
    {
      SpiDesktopApplication *app = desktop->applications->data;
      g_assert (app != NULL);
      spi_desktop_remove_application (desktop, app->bus_name);
    }

  G_OBJECT_CLASS (parent_class)->dispose (object); 
}

static dbus_bool_t
impl_desktop_get_child_count (const char *path, DBusMessageIter * iter,
		     void *user_data)
{
  SpiDesktop *desktop = SPI_REGISTRY(user_data)->desktop;

  if (desktop->applications)
    {
      return droute_return_v_int32(iter, g_list_length (desktop->applications));
    }
  else
    {
      return droute_return_v_int32(iter, 0);
    }
}

static DBusMessage *
impl_desktop_get_child_at_index (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiDesktop *desktop = SPI_REGISTRY(user_data)->desktop;
  DBusError error;
  dbus_int32_t index;
  SpiDesktopApplication *app;
  const char *bus_name;
  DBusMessage *reply;

  dbus_error_init (&error);
  if (!dbus_message_get_args (message, &error, DBUS_TYPE_INT32, &index, DBUS_TYPE_INVALID))
  {
    return spi_dbus_general_error (message);
  }
  app = g_list_nth_data (desktop->applications, index);
  bus_name = (app? app->bus_name: "");

  reply = dbus_message_new_method_return (message);
  if (reply)
    {
      dbus_message_append_args (reply, DBUS_TYPE_STRING, &bus_name, DBUS_TYPE_INVALID);
    }

  return reply;
}

static DBusMessage *
impl_desktop_get_children (DBusConnection *bus, DBusMessage *message, void *user_data)
{
  SpiDesktop *desktop = SPI_REGISTRY(user_data)->desktop;
  DBusError error;
  gint count;
  gint i;
  SpiDesktopApplication *app;
  const char *bus_name;
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;

  reply = dbus_message_new_method_return (message);
  if (!reply) return NULL;
  dbus_message_iter_init_append (reply, &iter);
  if (!dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "s", &iter_array))
  {
    goto oom;
  }
  count = g_list_length (desktop->applications);
  for (i = 0; i < count; i++)
  {
    app = g_list_nth_data (desktop->applications, i);
    bus_name = (app? app->bus_name: "");
    dbus_message_iter_append_basic (&iter_array, DBUS_TYPE_STRING, &bus_name);
  }
  if (!dbus_message_iter_close_container (&iter, &iter_array))
  {
    goto oom;
  }
  return reply;
oom:
  // TODO: Handle out of memory
  return reply;
}

static void
spi_desktop_exiting (void)
{
  exiting = TRUE;
}

static void
spi_desktop_class_init (SpiDesktopClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;

  object_class->dispose = spi_desktop_dispose;
  
  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  spi_desktop_signals[APPLICATION_ADDED] =
    g_signal_new ("application_added",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (SpiDesktopClass, application_added),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__UINT,
		  G_TYPE_NONE,
		  1, G_TYPE_UINT);
  spi_desktop_signals[APPLICATION_REMOVED] =
    g_signal_new ("application_removed",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (SpiDesktopClass, application_removed),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__UINT,
		  G_TYPE_NONE,
		  1, G_TYPE_UINT);
  g_atexit (spi_desktop_exiting);
}

SpiDesktop *
spi_desktop_new (void)
{
  return g_object_new (SPI_DESKTOP_TYPE, NULL);
}

static void
abnormal_application_termination (gpointer object, SpiDesktopApplication *app)
{
  g_return_if_fail (SPI_IS_DESKTOP (app->desktop));

  if (!exiting)
    spi_desktop_remove_application (app->desktop, app->bus_name);
}

void
spi_desktop_add_application (SpiDesktop *desktop,
			     const char *application)
{
  SpiDesktopApplication       *app;

  g_return_if_fail (SPI_IS_DESKTOP (desktop));

  app = g_new (SpiDesktopApplication, 1);
  app->desktop = desktop;
  app->bus_name = application;

      desktop->applications = g_list_append (desktop->applications, app);

  // TODO: Listen for termination, and call abnormal_application_termination

  g_signal_emit (G_OBJECT (desktop),
		 spi_desktop_signals[APPLICATION_ADDED], 0,
		 g_list_index (desktop->applications, app));
}

void
spi_desktop_remove_application (SpiDesktop *desktop,
				const char *bus_name)
{
  guint idx;
  GList *l;

  g_return_if_fail (  bus_name != NULL);
  g_return_if_fail (SPI_IS_DESKTOP (desktop));

  idx = 0;
  for (l = desktop->applications; l; l = l->next)
    {
      SpiDesktopApplication *app = (SpiDesktopApplication *) l->data;

      if (!strcmp(app->bus_name, bus_name))
        {
	  break;
	}
      idx++;
    }

  if (!l) return;

  g_signal_emit (G_OBJECT (desktop), spi_desktop_signals[APPLICATION_REMOVED], 0, idx);

  SpiDesktopApplication *app = (SpiDesktopApplication *) l->data;

  desktop->applications = g_list_delete_link (desktop->applications, l);

  g_free (app);
}

static DRouteMethod methods[] =
{
  { impl_desktop_get_child_at_index, "getChildAtIndex" },
  { impl_desktop_get_children, "getChildren" },
  { NULL, NULL }
};

static DRouteProperty properties[] =
{
  { impl_desktop_get_child_count, NULL, "getChildCount" },
  { NULL, NULL, NULL }
};

void
spi_registry_initialize_desktop_interface (DRouteData * data)
{
  droute_add_interface (data, SPI_DBUS_INTERFACE_DESKTOP, methods,
			properties, NULL, NULL);
};
