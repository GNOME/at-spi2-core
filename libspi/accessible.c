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

/*
 * accessible.c: test for accessibility implementation
 *
 */
#include <config.h>
#include <bonobo/Bonobo.h>

#include <stdio.h>

/*
 * This pulls the CORBA definitions for the "Accessibility::Accessible" server
 */
#include <libspi/Accessibility.h>

/*
 * This pulls the definition for the BonoboObject (Gtk Type)
 */
#include "accessible.h"

/*
 * Our parent Gtk object type
 */
#define PARENT_TYPE BONOBO_OBJECT_TYPE

/*
 * A pointer to our parent object class
 */
static GObjectClass *accessible_parent_class;

/*
 * Implemented GObject::finalize
 */
static void
accessible_object_finalize (GObject *object)
{
        Accessible *accessible = ACCESSIBLE (object);

        printf("accessible_object_finalize called\n");
        g_object_unref (accessible->atko);
        accessible->atko = NULL;

        printf("atko freed, calling parent finalize\n");
        accessible_parent_class->finalize (object);
}

/*
 * CORBA Accessibility::Accessible::get_name method implementation
 */
static CORBA_char *
impl_accessibility_accessible_get_name (PortableServer_Servant servant,
                                        CORBA_Environment     *ev)
{
  CORBA_char * retval;
  Accessible *accessible = ACCESSIBLE (bonobo_object_from_servant (servant));
  retval = atk_object_get_name (accessible->atko);
  if (retval )
    retval = CORBA_string_dup (retval);
  else
    retval = CORBA_string_dup ("");
  fprintf (stderr, "Accessible get_name called: %s\n", retval);
  return retval;
}

/*
 * CORBA Accessibility::Accessible::set_name method implementation
 */
static void
impl_accessibility_accessible_set_name (PortableServer_Servant servant,
                                        const CORBA_char      *name,
                                        CORBA_Environment     *ev)
{
  Accessible *accessible = ACCESSIBLE (bonobo_object_from_servant (servant));
  atk_object_set_name (accessible->atko, name);
  printf ("Accessible set_name called: %s\n", name);
}

/*
 * CORBA Accessibility::Accessible::get_description method implementation
 */
static CORBA_char *
impl_accessibility_accessible_get_description (PortableServer_Servant servant,
                                               CORBA_Environment     *ev)
{
  CORBA_char * retval;
  Accessible *accessible = ACCESSIBLE (bonobo_object_from_servant (servant));
  retval = CORBA_string_dup (atk_object_get_description (accessible->atko));
  fprintf (stderr, "Accessible get_description called: %s\n", retval);
  return retval;
}

/*
 * CORBA Accessibility::Accessible::set_description method implementation
 */
static void
impl_accessibility_accessible_set_description (PortableServer_Servant servant,
                                               const CORBA_char      *name,
                                               CORBA_Environment     *ev)
{
  Accessible *accessible = ACCESSIBLE (bonobo_object_from_servant (servant));
  atk_object_set_description (accessible->atko, name);
  printf ("Accessible set_description called: %s\n", name);
}

static void
accessible_class_init (AccessibleClass *klass)
{
        GObjectClass * object_class = (GObjectClass *) klass;
        POA_Accessibility_Accessible__epv *epv = &klass->epv;
        accessible_parent_class = g_type_class_peek_parent (klass);

        object_class->finalize = accessible_object_finalize;

        epv->_get_name = impl_accessibility_accessible_get_name;
        epv->_set_name = impl_accessibility_accessible_set_name;
        epv->_get_description = impl_accessibility_accessible_get_description;
        epv->_set_description = impl_accessibility_accessible_set_description;

        /* epv->_get_parent = impl_accessibility_accessible_get_parent;               */
        /* epv->_get_childCount = impl_accessibility_accessible_get_child_count;      */
        /* epv->getChildAtIndex = impl_accessibility_accessible_get_child_at_index;   */
        /* epv->getIndexInParent = impl_accessibility_accessible_get_index_in_parent; */
        /* epv->getRelationSet = impl_accessibility_accessible_get_relation_set;      */
        /* epv->getState = impl_accessibility_accessible_get_state;                   */
        /* epv->getRole = impl_accessibility_accessible_get_role;                     */
}

static void
accessible_init (Accessible *accessible)
{
}

GType
accessible_get_type (void)
{
        static GType type = 0;

        if (!type) {
                static const GTypeInfo tinfo = {
                        sizeof (AccessibleClass),
                        (GBaseInitFunc) NULL,
                        (GBaseFinalizeFunc) NULL,
                        (GClassInitFunc) accessible_class_init,
                        (GClassFinalizeFunc) NULL,
                        NULL, /* class data */
                        sizeof (Accessible),
                        0, /* n preallocs */
                        (GInstanceInitFunc) accessible_init,
                        NULL /* value table */
                };
                /*
                 * Bonobo_type_unique auto-generates a load of
                 * CORBA structures for us. All derived types must
                 * use bonobo_type_unique.
                 */
                type = bonobo_type_unique (
                        PARENT_TYPE,
                        POA_Accessibility_Accessible__init,
                        NULL,
                        G_STRUCT_OFFSET (AccessibleClass, epv),
                        &tinfo,
                        "Accessible");
        }

        return type;
}

Accessible *
accessible_new (AtkObject *o)
{
    Accessible *retval =
               ACCESSIBLE (g_object_new (accessible_get_type (), NULL));
    g_object_ref (o);
    retval->atko = ATK_OBJECT (o);
    return retval;
}
