/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001 Sun Microsystems Inc., Ximian Inc.
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
#include <libbonobo.h>
#include <libspi/desktop.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_ACCESSIBLE_TYPE

typedef struct {
	SpiDesktop *desktop;
	Accessibility_Application ref;
} Application;

/* A pointer to our parent object class */
static SpiAccessibleClass *parent_class;

static void
spi_desktop_init (SpiDesktop *desktop)
{
  spi_base_construct_default (SPI_BASE (desktop));

  desktop->applications = NULL;

  atk_object_set_name (SPI_BASE (desktop)->atko, "main");
}

static void
spi_desktop_dispose (GObject *object)
{
  SpiDesktop *desktop = (SpiDesktop *) object;

  while (desktop->applications)
    {
      Application *app = (Application *) desktop->applications;
      spi_desktop_remove_application (desktop, app->ref);
    }

  G_OBJECT_CLASS (parent_class)->dispose (object); 
}

static CORBA_long
impl_desktop_get_child_count (PortableServer_Servant servant,
                              CORBA_Environment     *ev)
{
  SpiDesktop *desktop = SPI_DESKTOP (bonobo_object_from_servant (servant));

  if (desktop->applications)
    {
      return g_list_length (desktop->applications);
    }
  else
    {
      return 0;
    }
}

static Accessibility_Accessible
impl_desktop_get_child_at_index (PortableServer_Servant servant,
                                 const CORBA_long       index,
                                 CORBA_Environment     *ev)
{
  SpiDesktop  *desktop = SPI_DESKTOP (bonobo_object_from_servant (servant));
  CORBA_Object retval;
  Application *app;

  app = g_list_nth_data (desktop->applications, index);

  if (app)
    {
      retval = bonobo_object_dup_ref (app->ref, ev);
      if (BONOBO_EX (ev))
        {
          CORBA_exception_free (ev);
	  CORBA_exception_set (ev, CORBA_USER_EXCEPTION,
			       ex_Accessibility_ChildGone, NULL);
	  retval = CORBA_OBJECT_NIL;
	}
    }
  else
    {
      retval = CORBA_OBJECT_NIL;
    }

  return (Accessibility_Accessible) retval;
}

static void
spi_desktop_class_init (SpiDesktopClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  SpiAccessibleClass * spi_accessible_class = (SpiAccessibleClass *) klass;
  POA_Accessibility_Accessible__epv *epv = &spi_accessible_class->epv;

  object_class->dispose = spi_desktop_dispose;
  
  parent_class = g_type_class_ref (SPI_ACCESSIBLE_TYPE);

  epv->_get_childCount = impl_desktop_get_child_count;
  epv->getChildAtIndex = impl_desktop_get_child_at_index;
}

BONOBO_TYPE_FUNC_FULL (SpiDesktop,
		       Accessibility_Desktop,
		       PARENT_TYPE,
		       spi_desktop);

SpiDesktop *
spi_desktop_new (void)
{
  SpiDesktop *retval = g_object_new (SPI_DESKTOP_TYPE, NULL);

  return retval;
}

static void
abnormal_application_termination (gpointer object, Application *app)
{
  g_return_if_fail (SPI_IS_DESKTOP (app->desktop));

  spi_desktop_remove_application (app->desktop, app->ref);
}

void
spi_desktop_add_application (SpiDesktop *desktop,
			     const Accessibility_Application application)
{
  CORBA_Environment ev;
  Application       *app;
  Accessibility_Application ref;

  g_return_if_fail (SPI_IS_DESKTOP (desktop));

  spi_desktop_remove_application (desktop, application);

  CORBA_exception_init (&ev);

  ref = bonobo_object_dup_ref (application, &ev);

  if (!BONOBO_EX (&ev))
    {
      app = g_new (Application, 1);
      app->desktop = desktop;
      app->ref = ref;

      desktop->applications = g_list_append (desktop->applications, app);

      ORBit_small_listen_for_broken (app->ref, G_CALLBACK (abnormal_application_termination), app);
    }

  CORBA_exception_free (&ev);
}

void
spi_desktop_remove_application (SpiDesktop *desktop,
				const Accessibility_Application app_ref)
{
  GList *l;
  CORBA_Environment ev;

  g_return_if_fail (SPI_IS_DESKTOP (desktop));

  CORBA_exception_init (&ev);

  for (l = desktop->applications; l; l = l->next)
    {
      Application *app = (Application *) l->data;

      if (CORBA_Object_is_equivalent (app->ref, app_ref, &ev))
        {
	  break;
	}
    }

  CORBA_exception_free (&ev);

  if (l)
    {
      Application *app = (Application *) l->data;

      desktop->applications = g_list_delete_link (desktop->applications, l);

      ORBit_small_unlisten_for_broken (app->ref, G_CALLBACK (abnormal_application_termination));
      bonobo_object_release_unref (app->ref, NULL);
      g_free (app);
    }
}
