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
 * component.c : bonobo wrapper for accessible component implementation
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
 * This pulls the definition of the Action bonobo object
 */
#include "action.h"

/*
 * Static function declarations
 */

static void
action_class_init (ActionClass *klass);
static void
action_init (Action *action);
static void
action_finalize (GObject *obj);
static CORBA_long
impl__get_nActions(PortableServer_Servant servant,
		 CORBA_Environment * ev);
static CORBA_string
impl_getDescription (PortableServer_Servant servant,
		     const CORBA_long index,
		     CORBA_Environment * ev);
static CORBA_boolean 
impl_doAction (PortableServer_Servant servant,
	       const CORBA_long index, CORBA_Environment * ev);
static CORBA_string
impl_getName (PortableServer_Servant servant,
	      const CORBA_long index,
	      CORBA_Environment * ev);
static CORBA_string
impl_getKeyBinding (PortableServer_Servant servant,
		    const CORBA_long index,
		    CORBA_Environment * ev);

static GObjectClass *parent_class;

GType
action_get_type (void)
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo = {
      sizeof (ActionClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) action_class_init,
      (GClassFinalizeFunc) NULL,
      NULL, /* class data */
      sizeof (Action),
      0, /* n preallocs */
      (GInstanceInitFunc) action_init,
                        NULL /* value table */
    };

    /*
     * Bonobo_type_unique auto-generates a load of
     * CORBA structures for us. All derived types must
     * use bonobo_type_unique.
     */
    type = bonobo_type_unique (
			       BONOBO_OBJECT_TYPE,
			       POA_Accessibility_Action__init,
			       NULL,
			       G_STRUCT_OFFSET (ActionClass, epv),
			       &tinfo,
			       "AccessibleAction");
  }

  return type;
}

static void
action_class_init (ActionClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Action__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = action_finalize;


  /* Initialize epv table */

  epv->_get_nActions = impl__get_nActions;
  epv->doAction = impl_doAction;
  epv->getDescription = impl_getDescription;
  epv->getName = impl_getName;
  epv->getKeyBinding = impl_getKeyBinding;
}

static void
action_init (Action *action)
{
}

static void
action_finalize (GObject *obj)
{
  Action *action = ACTION (obj);
  g_object_unref (action->atko);
  action->atko = NULL;
  parent_class->finalize (obj);
}

Action *
action_interface_new (AtkObject *obj)
{
  Action *new_action = 
    ACTION(g_object_new (ACTION_TYPE, NULL));
  new_action->atko = obj;
  g_object_ref (obj);
  return new_action;
}

static CORBA_long
impl__get_nActions(PortableServer_Servant servant,
	    CORBA_Environment * ev)
{
  Action *action = ACTION (bonobo_object_from_servant(servant));
  return (CORBA_long) atk_action_get_n_actions (ATK_ACTION(action->atko));
}

static CORBA_boolean
impl_doAction (PortableServer_Servant servant,
	       const CORBA_long index, CORBA_Environment * ev)
{
  Action *action = ACTION (bonobo_object_from_servant (servant));
  return (CORBA_boolean) atk_action_do_action (ATK_ACTION(action->atko), (gint) index);
}


static CORBA_string
impl_getDescription (PortableServer_Servant servant,
		const CORBA_long index,
		CORBA_Environment * ev)
{
  Action *action = ACTION (bonobo_object_from_servant(servant));
  return CORBA_string_dup (atk_action_get_description (ATK_ACTION(action->atko), (gint) index));
}


static CORBA_string
impl_getName (PortableServer_Servant servant,
		const CORBA_long index,
		CORBA_Environment * ev)
{
  Action *action = ACTION (bonobo_object_from_servant(servant));
  return CORBA_string_dup (atk_action_get_name (ATK_ACTION(action->atko), (gint) index));
}

static CORBA_string
impl_getKeyBinding (PortableServer_Servant servant,
		    const CORBA_long index,
		    CORBA_Environment * ev)
{
  Action *action = ACTION (bonobo_object_from_servant(servant));
  return CORBA_string_dup (atk_action_get_keybinding (ATK_ACTION(action->atko), (gint) index));
}
