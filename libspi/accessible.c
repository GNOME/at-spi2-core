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

/* accessible.c: the core of the accessibility implementation */

#include <config.h>
#include <stdio.h>
#include <bonobo/bonobo-exception.h>
#include <atk/atk.h>
#include <libspi/libspi.h>

/* Our parent Gtk object type  */
#define PARENT_TYPE SPI_TYPE_BASE

static Accessibility_Role
spi_role_from_atk_role (AtkRole role)
{
  Accessibility_Role spi_role;

  /* TODO: finish and/or make efficient! */
  switch (role)
  {
    case ATK_ROLE_INVALID:
      spi_role = Accessibility_ROLE_INVALID;
      break;
    case ATK_ROLE_ACCEL_LABEL:
    case ATK_ROLE_ALERT:
    case ATK_ROLE_ANIMATION: 
    case ATK_ROLE_ARROW: 
    case ATK_ROLE_CALENDAR: 
    case ATK_ROLE_CANVAS:
    case ATK_ROLE_CHECK_BOX:
    case ATK_ROLE_CHECK_MENU_ITEM:
    case ATK_ROLE_COLOR_CHOOSER:
    case ATK_ROLE_COLUMN_HEADER:
    case ATK_ROLE_COMBO_BOX:
    case ATK_ROLE_DATE_EDITOR:
    case ATK_ROLE_DESKTOP_ICON:
    case ATK_ROLE_DESKTOP_FRAME:
    case ATK_ROLE_DIAL:
    case ATK_ROLE_DIALOG:
    case ATK_ROLE_DIRECTORY_PANE:
    case ATK_ROLE_DRAWING_AREA:
    case ATK_ROLE_FILE_CHOOSER:
    case ATK_ROLE_FILLER:
    case ATK_ROLE_FONT_CHOOSER:
    case ATK_ROLE_FRAME:
    case ATK_ROLE_GLASS_PANE: 
    case ATK_ROLE_HTML_CONTAINER: 
    case ATK_ROLE_ICON: 
    case ATK_ROLE_IMAGE:
    case ATK_ROLE_INTERNAL_FRAME:
    case ATK_ROLE_LABEL:
    case ATK_ROLE_LAYERED_PANE:
    case ATK_ROLE_LIST:
    case ATK_ROLE_LIST_ITEM:
    case ATK_ROLE_MENU:
    case ATK_ROLE_MENU_BAR:
    case ATK_ROLE_MENU_ITEM:
    case ATK_ROLE_OPTION_PANE:
    case ATK_ROLE_PAGE_TAB:
    case ATK_ROLE_PAGE_TAB_LIST:
    case ATK_ROLE_PANEL:
    case ATK_ROLE_PASSWORD_TEXT:
    case ATK_ROLE_POPUP_MENU:
    case ATK_ROLE_PROGRESS_BAR:
    case ATK_ROLE_PUSH_BUTTON:
    case ATK_ROLE_RADIO_BUTTON:
    case ATK_ROLE_RADIO_MENU_ITEM:
    case ATK_ROLE_ROOT_PANE:
    case ATK_ROLE_ROW_HEADER:
    case ATK_ROLE_SCROLL_BAR:
    case ATK_ROLE_SCROLL_PANE:
    case ATK_ROLE_SEPARATOR:
    case ATK_ROLE_SLIDER:
    case ATK_ROLE_SPLIT_PANE:
    case ATK_ROLE_SPIN_BUTTON:
    case ATK_ROLE_STATUSBAR:
    case ATK_ROLE_TABLE:
    case ATK_ROLE_TABLE_CELL:
    case ATK_ROLE_TABLE_COLUMN_HEADER:
    case ATK_ROLE_TABLE_ROW_HEADER:
    case ATK_ROLE_TEAR_OFF_MENU_ITEM:
    case ATK_ROLE_TERMINAL:
    case ATK_ROLE_TEXT:
    case ATK_ROLE_TOGGLE_BUTTON:
    case ATK_ROLE_TOOL_BAR:
    case ATK_ROLE_TOOL_TIP:
    case ATK_ROLE_TREE:
    case ATK_ROLE_TREE_TABLE:
    case ATK_ROLE_UNKNOWN:
    case ATK_ROLE_VIEWPORT:
    case ATK_ROLE_WINDOW:
    case ATK_ROLE_LAST_DEFINED:
    default:
      spi_role = Accessibility_ROLE_EXTENDED;	    
  }
  return spi_role;
}

static AtkObject *
get_accessible_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  if (!object)
    {
      return NULL;
    }

  return object->atko;
}

/*
 * CORBA Accessibility::Accessible::get_name method implementation
 */
static CORBA_char *
impl_accessibility_accessible_get_name (PortableServer_Servant servant,
                                        CORBA_Environment     *ev)
{
  const gchar *name;
  CORBA_char  *retval;
  AtkObject   *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, CORBA_string_dup (""));

  name = atk_object_get_name (object);

  if (name)
    {
      retval = CORBA_string_dup (name);
    }
  else
    {
      retval = CORBA_string_dup ("");
    }

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
  AtkObject *object = get_accessible_from_servant (servant);

  g_return_if_fail (object != NULL);

  atk_object_set_name (object, name);
}

/*
 * CORBA Accessibility::Accessible::get_description method implementation
 */
static CORBA_char *
impl_accessibility_accessible_get_description (PortableServer_Servant servant,
                                               CORBA_Environment     *ev)
{
  const gchar *descr;
  CORBA_char  *retval;
  AtkObject   *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, CORBA_string_dup (""));

  descr = atk_object_get_description (object);

  if (descr)
    {
      retval = CORBA_string_dup (descr);
    }
  else
    {
      retval = CORBA_string_dup ("");
    }

  return retval;
}

/*
 * CORBA Accessibility::Accessible::set_description method implementation
 */
static void
impl_accessibility_accessible_set_description (PortableServer_Servant servant,
                                               const CORBA_char      *descr,
                                               CORBA_Environment     *ev)
{
  AtkObject *object = get_accessible_from_servant (servant);

  g_return_if_fail (object != NULL);

  atk_object_set_description (object, descr);
}

/*
 * CORBA Accessibility::Accessible::get_parent method implementation
 */
static Accessibility_Accessible
impl_accessibility_accessible_get_parent (PortableServer_Servant servant,
                                          CORBA_Environment     *ev)
{
  AtkObject *parent;
  AtkObject *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, CORBA_OBJECT_NIL);

  parent = atk_object_get_parent (object);

  return spi_accessible_new_return (parent, FALSE, ev);
}

/*
 * CORBA Accessibility::Accessible::get_IndexInParent method implementation
 */
static CORBA_long
impl_accessibility_accessible_get_index_in_parent (PortableServer_Servant servant,
                                                   CORBA_Environment     *ev)
{
  AtkObject *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, -1);

  return atk_object_get_index_in_parent (object);
}

/*
 * CORBA Accessibility::Accessible::get_childCount method implementation
 */
static CORBA_long
impl_accessibility_accessible_get_child_count (PortableServer_Servant servant,
                                               CORBA_Environment     *ev)
{
  AtkObject *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, 0);

  return atk_object_get_n_accessible_children (object);
}

/*
 * CORBA Accessibility::Accessible::getChildAtIndex method implementation
 */
static Accessibility_Accessible
impl_accessibility_accessible_get_child_at_index (PortableServer_Servant servant,
                                                  const CORBA_long      index,
                                                  CORBA_Environment     *ev)
{
  AtkObject *child;
  AtkObject *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, 0);

  child = atk_object_ref_accessible_child (object, index);

  return spi_accessible_new_return (child, TRUE, ev);
}

/*
 * CORBA Accessibility::Accessible::getState method implementation
 */
static Accessibility_StateSet
impl_accessibility_accessible_get_state (PortableServer_Servant servant,
					 CORBA_Environment     *ev)
{
  AtkObject *object = get_accessible_from_servant (servant);

  bonobo_return_val_if_fail (object != NULL, NULL, ev);

  printf ("SpiAccessible get_state.\n");

  /* TODO: implement the bonobo stateset class */
  return (Accessibility_StateSet) NULL;
}

/*
 * CORBA Accessibility::Accessible::getRelationSet method implementation
 */
static Accessibility_RelationSet *
impl_accessibility_accessible_get_relation_set (PortableServer_Servant servant,
						CORBA_Environment     *ev)
{
  Accessibility_RelationSet *retval;
  gint n_relations;
  gint i;
  AtkRelationSet *relation_set;
  AtkObject      *object = get_accessible_from_servant (servant);

  bonobo_return_val_if_fail (object != NULL, NULL, ev);

  relation_set = atk_object_ref_relation_set (object);

  n_relations = atk_relation_set_get_n_relations (relation_set);
  retval = CORBA_sequence_Accessibility_Relation__alloc ();
  CORBA_sequence_Accessibility_Relation_allocbuf (n_relations);
	  
  for (i = 0; i < n_relations; ++i)
    {
      retval->_buffer[i] =
        bonobo_object_dup_ref (
          BONOBO_OBJREF (
            spi_relation_new (atk_relation_set_get_relation (relation_set, i))),
	  ev);
    }
  
  printf ("SpiAccessible get_relation_set.\n");
  return retval;
}

/*
 * CORBA Accessibility::Accessible::getRole method implementation
 */
static Accessibility_Role
impl_accessibility_accessible_get_role (PortableServer_Servant servant,
					CORBA_Environment     *ev)
{
  AtkRole            role;
  Accessibility_Role retval;
  AtkObject         *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, 0);

  role = atk_object_get_role (object);
  retval = spi_role_from_atk_role (role);

  return retval;
}

/*
 * CORBA Accessibility::Accessible::getRole method implementation
 */
static CORBA_char *
impl_accessibility_accessible_get_role_name (PortableServer_Servant servant,
					     CORBA_Environment     *ev)
{
  AtkRole            role;
  Accessibility_Role retval;
  AtkObject         *object = get_accessible_from_servant (servant);

  g_return_val_if_fail (object != NULL, 0);

  role = atk_object_get_role (object);

  return CORBA_string_dup (atk_role_get_name (role));
}

static void
spi_accessible_class_init (SpiAccessibleClass *klass)
{
        POA_Accessibility_Accessible__epv *epv = &klass->epv;

        epv->_get_name = impl_accessibility_accessible_get_name;
        epv->_set_name = impl_accessibility_accessible_set_name;
        epv->_get_description = impl_accessibility_accessible_get_description;
        epv->_set_description = impl_accessibility_accessible_set_description;

        epv->_get_parent = impl_accessibility_accessible_get_parent;
        epv->_get_childCount = impl_accessibility_accessible_get_child_count;
        epv->getChildAtIndex = impl_accessibility_accessible_get_child_at_index;
        epv->getIndexInParent = impl_accessibility_accessible_get_index_in_parent;

        epv->getRelationSet = impl_accessibility_accessible_get_relation_set;
        epv->getState = impl_accessibility_accessible_get_state;
        epv->getRole = impl_accessibility_accessible_get_role;
        epv->getRoleName = impl_accessibility_accessible_get_role_name;
}

static void
spi_accessible_init (SpiAccessible *accessible)
{
}

BONOBO_TYPE_FUNC_FULL (SpiAccessible,
		       Accessibility_Accessible,
		       PARENT_TYPE,
		       spi_accessible);

static GHashTable *public_corba_refs = NULL;

static GHashTable *
get_public_refs (void)
{
  if (!public_corba_refs)
    {
      public_corba_refs = g_hash_table_new (NULL, NULL);
    }
  return public_corba_refs;
}

static void
de_register_public_ref (SpiBase *object)
{
  g_hash_table_remove (get_public_refs (), object->atko);
}

SpiAccessible *
spi_accessible_new (AtkObject *o)
{
    SpiAccessible *retval;
    CORBA_Environment ev;

    CORBA_exception_init (&ev);

    g_assert (o);

    if ((retval = g_hash_table_lookup (get_public_refs (), o)))
      {
        bonobo_object_ref (BONOBO_OBJECT (retval));
	return retval;
      }

    retval = g_object_new (SPI_ACCESSIBLE_TYPE, NULL);

    spi_base_construct (SPI_BASE (retval), o);

    g_hash_table_insert (get_public_refs (), o, retval);
    g_signal_connect (G_OBJECT (retval), "destroy",
		      G_CALLBACK (de_register_public_ref),
		      NULL);

    /* aggregate appropriate SPI interfaces based on ATK interfaces */

    if (ATK_IS_ACTION (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_action_interface_new (o)));
      }

    if (ATK_IS_COMPONENT (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_component_interface_new (o)));
      }

    if (ATK_IS_EDITABLE_TEXT (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT(spi_editable_text_interface_new (o)));
      }

    else if (ATK_IS_HYPERTEXT (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_hypertext_interface_new (o)));
      }

    else if (ATK_IS_TEXT (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_text_interface_new (o)));
      }

    if (ATK_IS_IMAGE (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_image_interface_new (o)));
      }

    if (ATK_IS_SELECTION (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_selection_interface_new (o)));
      }

    if (ATK_IS_TABLE (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_table_interface_new (o)));
      }

    if (ATK_IS_VALUE (o))
      {
        bonobo_object_add_interface (bonobo_object (retval),
                                     BONOBO_OBJECT (spi_value_interface_new (o)));
      }

    return retval;
}

/**
 * spi_accessible_new_return:
 * @o: an AtkObject or NULL
 * @release_ref: whether to unref this AtkObject before return
 * @ev: a CORBA environment
 * 
 * A helper function to instantiate a CORBA accessiblility
 * proxy from an AtkObject.
 * 
 * Return value: the proxy or CORBA_OBJECT_NIL
 **/
Accessibility_Accessible
spi_accessible_new_return (AtkObject         *o,
			   gboolean           release_ref,
			   CORBA_Environment *ev)
{
  SpiAccessible *accessible;

  if (!o)
    {
      return CORBA_OBJECT_NIL;
    }

  accessible = spi_accessible_new (o);

  if (release_ref)
    {
      g_object_unref (G_OBJECT (o));
    }

  return CORBA_Object_duplicate (BONOBO_OBJREF (accessible), ev);
}
