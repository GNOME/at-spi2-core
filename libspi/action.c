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
 * This pulls the definition of the SpiAction bonobo object
 */
#include "action.h"

/*
 * Static function declarations
 */

static void
spi_action_class_init (SpiActionClass *klass);
static void
spi_action_init (SpiAction *action);
static void
spi_action_finalize (GObject *obj);
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

BONOBO_TYPE_FUNC_FULL (SpiAction,
		       Accessibility_Action,
		       BONOBO_OBJECT_TYPE,
		       spi_action);

static void
spi_action_class_init (SpiActionClass *klass)
{
  GObjectClass * object_class = (GObjectClass *) klass;
  POA_Accessibility_Action__epv *epv = &klass->epv;
  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = spi_action_finalize;


  /* Initialize epv table */

  epv->_get_nActions = impl__get_nActions;
  epv->doAction = impl_doAction;
  epv->getDescription = impl_getDescription;
  epv->getName = impl_getName;
  epv->getKeyBinding = impl_getKeyBinding;
}

static void
spi_action_init (SpiAction *action)
{
}

static void
spi_action_finalize (GObject *obj)
{
  SpiAction *action = SPI_ACTION (obj);
  g_object_unref (action->atko);
  action->atko = NULL;
  parent_class->finalize (obj);
}

SpiAction *
spi_action_interface_new (AtkObject *obj)
{
  SpiAction *new_action = 
    SPI_ACTION(g_object_new (SPI_ACTION_TYPE, NULL));
  new_action->atko = obj;
  g_object_ref (obj);
  return new_action;
}

static CORBA_long
impl__get_nActions(PortableServer_Servant servant,
	    CORBA_Environment * ev)
{
  SpiAction *action = SPI_ACTION (bonobo_object_from_servant(servant));
  return (CORBA_long) atk_action_get_n_actions (ATK_ACTION(action->atko));
}

static CORBA_boolean
impl_doAction (PortableServer_Servant servant,
	       const CORBA_long index, CORBA_Environment * ev)
{
  SpiAction *action = SPI_ACTION (bonobo_object_from_servant (servant));
  return (CORBA_boolean) atk_action_do_action (ATK_ACTION(action->atko), (gint) index);
}


static CORBA_string
impl_getDescription (PortableServer_Servant servant,
		const CORBA_long index,
		CORBA_Environment * ev)
{
  SpiAction *action = SPI_ACTION (bonobo_object_from_servant(servant));
  const gchar *rv;
  
  rv = atk_action_get_description (ATK_ACTION(action->atko), (gint) index);
  if (rv)
    return CORBA_string_dup (rv);
  else
    return CORBA_string_dup ("");
}


static CORBA_string
impl_getName (PortableServer_Servant servant,
		const CORBA_long index,
		CORBA_Environment * ev)
{
  SpiAction *action = SPI_ACTION (bonobo_object_from_servant(servant));
  const gchar *rv;
  
  rv = atk_action_get_name (ATK_ACTION(action->atko), (gint) index);
  if (rv)
    return CORBA_string_dup (rv);
  else
    return CORBA_string_dup ("");
}

static CORBA_string
impl_getKeyBinding (PortableServer_Servant servant,
		    const CORBA_long index,
		    CORBA_Environment * ev)
{
  SpiAction *action = SPI_ACTION (bonobo_object_from_servant(servant));
  const gchar *rv;
  
  rv = atk_action_get_keybinding (ATK_ACTION(action->atko), (gint) index);
  if (rv)
    return CORBA_string_dup (rv);
  else
    return CORBA_string_dup ("");
}
