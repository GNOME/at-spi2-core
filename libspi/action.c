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

/* component.c : bonobo wrapper for accessible component implementation */

#include <config.h>
#include <stdio.h>
#include <libspi/action.h>
#include <atk/atkaction.h>

/*
 * Static function declarations
 */

static void
spi_action_class_init (SpiActionClass *klass);
static void
spi_action_init (SpiAction *action);
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

BONOBO_TYPE_FUNC_FULL (SpiAction,
		       Accessibility_Action,
		       SPI_TYPE_BASE,
		       spi_action)

static void
spi_action_class_init (SpiActionClass *klass)
{
  POA_Accessibility_Action__epv *epv = &klass->epv;

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

SpiAction *
spi_action_interface_new (AtkObject *obj)
{
  SpiAction *new_action = g_object_new (SPI_ACTION_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_action), G_OBJECT(obj));

  return new_action;
}

static AtkAction *
get_action_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));
  g_return_val_if_fail (object != NULL, NULL);
  /* the convention of making hyperlinks actionable breaks the assertion below */
  /* g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL); */
  return ATK_ACTION (object->gobj);
}

static CORBA_long
impl__get_nActions (PortableServer_Servant servant,
		    CORBA_Environment     *ev)
{
  AtkAction *action = get_action_from_servant (servant);
  return atk_action_get_n_actions (action);
}

static CORBA_boolean
impl_doAction (PortableServer_Servant servant,
	       const CORBA_long index, CORBA_Environment * ev)
{
  AtkAction *action = get_action_from_servant (servant);
  return atk_action_do_action (action, (gint) index);
}

static CORBA_string
impl_getDescription (PortableServer_Servant servant,
		const CORBA_long index,
		CORBA_Environment * ev)
{
  AtkAction *action = get_action_from_servant (servant);
  const gchar *rv;
  
  rv = atk_action_get_description (action, index);
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
  AtkAction *action = get_action_from_servant (servant);
  const gchar *rv;
  
  rv = atk_action_get_name (action, index);
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
  AtkAction *action = get_action_from_servant (servant);
  const gchar *rv;
  
  rv = atk_action_get_keybinding (action, index);
  if (rv)
    return CORBA_string_dup (rv);
  else
    return CORBA_string_dup ("");
}
