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

#include <stdlib.h> /* for malloc */
#include <libspi/stateset.h>
#include <cspi/spi-private.h>

static const char *role_names [] =
{
  "invalid",
  "accel-label",
  "alert",
  "animation",
  "arrow",
  "calendar",
  "canvas",
  "check-box",
  "check-menu-item",
  "color-chooser",
  "column-header",
  "combo-box",
  "date-editor",
  "desktop-icon",
  "desktop-frame",
  "dial",
  "dialog",
  "directory-pane",
  "drawing-area",
  "file-chooser",
  "filler",
  "font-chooser",
  "frame",
  "glass-pane",
  "html-container",
  "icon",
  "image",
  "internalframe",
  "label",
  "layered-pane",
  "list",
  "list-item",
  "menu",
  "menu-bar",
  "menu-item",
  "option-pane",
  "page-tab",
  "page-tab-list",
  "panel",
  "password-text",
  "popup-menu",
  "progress-bar",
  "push-button",
  "radio-button",
  "radio-menu-item",
  "root-pane",
  "row-header",
  "scroll-bar",
  "scroll-pane",
  "separator",
  "slider",
  "spin-button",
  "split-pane",
  "statusbar",
  "table",
  "table-cell",
  "table-column-header",
  "table-row-header",
  "tear-off-menu-item",
  "terminal",
  "text",
  "toggle-button",
  "tool-bar",
  "tool-tip",
  "tree",
  "tree-table",
  "unknown",
  "viewport",
  "window",
  NULL,
  "header",
  "fooler",
  "paragraph",
  "ruler",
  "application"
};

#define MAX_ROLES (sizeof (role_names) / sizeof (char *))

static SPIBoolean
cspi_init_role_table (AccessibleRole *role_table)
{
  int i;
  for (i = 0; i < Accessibility_ROLE_LAST_DEFINED; ++i)
    {
      role_table [i] = SPI_ROLE_UNKNOWN;
    }
  role_table [Accessibility_ROLE_INVALID] = SPI_ROLE_INVALID;
  role_table [Accessibility_ROLE_ACCELERATOR_LABEL] = SPI_ROLE_ACCEL_LABEL;
  role_table [Accessibility_ROLE_ALERT] = SPI_ROLE_ALERT;
  role_table [Accessibility_ROLE_ANIMATION] = SPI_ROLE_ANIMATION;
  role_table [Accessibility_ROLE_ARROW] = SPI_ROLE_ARROW;
  role_table [Accessibility_ROLE_CALENDAR] = SPI_ROLE_CALENDAR;
  role_table [Accessibility_ROLE_CANVAS] = SPI_ROLE_CANVAS;
  role_table [Accessibility_ROLE_CHECK_BOX] = SPI_ROLE_CHECK_BOX;
  role_table [Accessibility_ROLE_CHECK_MENU_ITEM] = SPI_ROLE_CHECK_MENU_ITEM;
  role_table [Accessibility_ROLE_COLOR_CHOOSER] = SPI_ROLE_COLOR_CHOOSER;
  role_table [Accessibility_ROLE_COLUMN_HEADER] = SPI_ROLE_COLUMN_HEADER;
  role_table [Accessibility_ROLE_COMBO_BOX] = SPI_ROLE_COMBO_BOX;
  role_table [Accessibility_ROLE_DATE_EDITOR] = SPI_ROLE_DATE_EDITOR;
  role_table [Accessibility_ROLE_DESKTOP_ICON] = SPI_ROLE_DESKTOP_ICON;
  role_table [Accessibility_ROLE_DESKTOP_FRAME] = SPI_ROLE_DESKTOP_FRAME;
  role_table [Accessibility_ROLE_DIAL] = SPI_ROLE_DIAL;
  role_table [Accessibility_ROLE_DIALOG] = SPI_ROLE_DIALOG;
  role_table [Accessibility_ROLE_DIRECTORY_PANE] = SPI_ROLE_DIRECTORY_PANE;
  role_table [Accessibility_ROLE_DRAWING_AREA] = SPI_ROLE_DRAWING_AREA;
  role_table [Accessibility_ROLE_FILE_CHOOSER] = SPI_ROLE_FILE_CHOOSER;
  role_table [Accessibility_ROLE_FILLER] = SPI_ROLE_FILLER;
  role_table [Accessibility_ROLE_FONT_CHOOSER] = SPI_ROLE_FONT_CHOOSER;
  role_table [Accessibility_ROLE_FRAME] = SPI_ROLE_FRAME;
  role_table [Accessibility_ROLE_GLASS_PANE] = SPI_ROLE_GLASS_PANE;
  role_table [Accessibility_ROLE_HTML_CONTAINER] = SPI_ROLE_HTML_CONTAINER;
  role_table [Accessibility_ROLE_ICON] = SPI_ROLE_ICON;
  role_table [Accessibility_ROLE_IMAGE] = SPI_ROLE_IMAGE;
  role_table [Accessibility_ROLE_INTERNAL_FRAME] = SPI_ROLE_INTERNAL_FRAME;
  role_table [Accessibility_ROLE_LABEL] = SPI_ROLE_LABEL;
  role_table [Accessibility_ROLE_LAYERED_PANE] = SPI_ROLE_LAYERED_PANE;
  role_table [Accessibility_ROLE_LIST] = SPI_ROLE_LIST;
  role_table [Accessibility_ROLE_LIST_ITEM] = SPI_ROLE_LIST_ITEM;
  role_table [Accessibility_ROLE_MENU] = SPI_ROLE_MENU;
  role_table [Accessibility_ROLE_MENU_BAR] = SPI_ROLE_MENU_BAR;
  role_table [Accessibility_ROLE_MENU_ITEM] = SPI_ROLE_MENU_ITEM;
  role_table [Accessibility_ROLE_OPTION_PANE] = SPI_ROLE_OPTION_PANE;
  role_table [Accessibility_ROLE_PAGE_TAB] = SPI_ROLE_PAGE_TAB;
  role_table [Accessibility_ROLE_PAGE_TAB_LIST] = SPI_ROLE_PAGE_TAB_LIST;
  role_table [Accessibility_ROLE_PANEL] = SPI_ROLE_PANEL;
  role_table [Accessibility_ROLE_PASSWORD_TEXT] = SPI_ROLE_PASSWORD_TEXT;
  role_table [Accessibility_ROLE_POPUP_MENU] = SPI_ROLE_POPUP_MENU;
  role_table [Accessibility_ROLE_PROGRESS_BAR] = SPI_ROLE_PROGRESS_BAR;
  role_table [Accessibility_ROLE_PUSH_BUTTON] = SPI_ROLE_PUSH_BUTTON;
  role_table [Accessibility_ROLE_RADIO_BUTTON] = SPI_ROLE_RADIO_BUTTON;
  role_table [Accessibility_ROLE_RADIO_MENU_ITEM] = SPI_ROLE_RADIO_MENU_ITEM;
  role_table [Accessibility_ROLE_ROOT_PANE] = SPI_ROLE_ROOT_PANE;
  role_table [Accessibility_ROLE_ROW_HEADER] = SPI_ROLE_ROW_HEADER;
  role_table [Accessibility_ROLE_SCROLL_BAR] = SPI_ROLE_SCROLL_BAR;
  role_table [Accessibility_ROLE_SCROLL_PANE] = SPI_ROLE_SCROLL_PANE;
  role_table [Accessibility_ROLE_SEPARATOR] = SPI_ROLE_SEPARATOR;
  role_table [Accessibility_ROLE_SLIDER] = SPI_ROLE_SLIDER;
  role_table [Accessibility_ROLE_SPIN_BUTTON] = SPI_ROLE_SPIN_BUTTON;
  role_table [Accessibility_ROLE_SPLIT_PANE] = SPI_ROLE_SPLIT_PANE;
  role_table [Accessibility_ROLE_STATUS_BAR] = SPI_ROLE_STATUS_BAR;
  role_table [Accessibility_ROLE_TABLE] = SPI_ROLE_TABLE;
  role_table [Accessibility_ROLE_TABLE_CELL] = SPI_ROLE_TABLE_CELL;
  role_table[Accessibility_ROLE_TABLE_COLUMN_HEADER] = SPI_ROLE_TABLE_COLUMN_HEADER;
  role_table[Accessibility_ROLE_TABLE_ROW_HEADER] = SPI_ROLE_TABLE_ROW_HEADER;
  role_table [Accessibility_ROLE_TEAROFF_MENU_ITEM] = SPI_ROLE_TEAROFF_MENU_ITEM;
  role_table[Accessibility_ROLE_TERMINAL] = SPI_ROLE_TERMINAL;
  role_table [Accessibility_ROLE_TEXT] = SPI_ROLE_TEXT;
  role_table [Accessibility_ROLE_TOGGLE_BUTTON] = SPI_ROLE_TOGGLE_BUTTON;
  role_table [Accessibility_ROLE_TOOL_BAR] = SPI_ROLE_TOOL_BAR;
  role_table [Accessibility_ROLE_TOOL_TIP] = SPI_ROLE_TOOL_TIP;
  role_table [Accessibility_ROLE_TREE] = SPI_ROLE_TREE;
  role_table [Accessibility_ROLE_TREE_TABLE] = SPI_ROLE_TREE_TABLE;
  role_table [Accessibility_ROLE_VIEWPORT] = SPI_ROLE_VIEWPORT;
  role_table [Accessibility_ROLE_WINDOW] = SPI_ROLE_WINDOW;
  role_table [Accessibility_ROLE_EXTENDED] = SPI_ROLE_EXTENDED;
  role_table [Accessibility_ROLE_HEADER] = SPI_ROLE_HEADER;
  role_table [Accessibility_ROLE_FOOTER] = SPI_ROLE_FOOTER;
  role_table [Accessibility_ROLE_PARAGRAPH] = SPI_ROLE_PARAGRAPH;
  role_table [Accessibility_ROLE_RULER] = SPI_ROLE_RULER;
  role_table [Accessibility_ROLE_APPLICATION] = SPI_ROLE_APPLICATION;
  role_table [Accessibility_ROLE_AUTOCOMPLETE] = SPI_ROLE_AUTOCOMPLETE;

  return TRUE;
}

static AccessibleRole
cspi_role_from_spi_role (Accessibility_Role role)
{
  /* array is sized according to IDL roles because IDL roles are the index */	
  static AccessibleRole cspi_role_table [Accessibility_ROLE_LAST_DEFINED];
  static SPIBoolean is_initialized = FALSE;
  AccessibleRole cspi_role;
  if (!is_initialized)
    {
      is_initialized = cspi_init_role_table (cspi_role_table);	    
    }
  if (role >= 0 && role < Accessibility_ROLE_LAST_DEFINED)
    {
      cspi_role = cspi_role_table [role];	    
    }
  else
    {
      cspi_role = SPI_ROLE_EXTENDED;
    }
  return cspi_role; 
}

/**
 * AccessibleRole_getName:
 * @role: an #AccessibleRole object to query.
 *
 * Get a localizeable string that indicates the name of an #AccessibleRole.
 * <em>DEPRECATED.</em>
 *
 * Returns: a localizable string name for an #AccessibleRole enumerated type.
 **/
char *
AccessibleRole_getName (AccessibleRole role)
{
  if (role < MAX_ROLES && role_names [(int) role])
    {
      return CORBA_string_dup (role_names [(int) role]);
    }
  else
    {
      return CORBA_string_dup ("");
    }
}

/**
 * Accessible_ref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Increment the reference count for an #Accessible object.
 **/
void
Accessible_ref (Accessible *obj)
{
  cspi_object_ref (obj);
}

/**
 * Accessible_unref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Decrement the reference count for an #Accessible object.
 **/
void
Accessible_unref (Accessible *obj)
{
  cspi_object_unref (obj);
}

/**
 * Accessible_getName:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the name of an #Accessible object.
 *
 * Returns: a UTF-8 string indicating the name of the #Accessible object.
 * or NULL on exception
 **/
char *
Accessible_getName (Accessible *obj)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = Accessibility_Accessible__get_name (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getName", NULL); 

  return retval;
}

/**
 * Accessible_getDescription:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the description of an #Accessible object.
 *
 * Returns: a UTF-8 string describing the #Accessible object.
 * or NULL on exception
 **/
char *
Accessible_getDescription (Accessible *obj)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = Accessibility_Accessible__get_description (CSPI_OBJREF (obj),
					       cspi_ev ());

  cspi_return_val_if_ev ("getDescription", NULL); 

  return retval;
}

/**
 * Accessible_getParent:
 * @obj: a pointer to the #Accessible object to query.
 *
 * Get an #Accessible object's parent container.
 *
 * Returns: a pointer tothe #Accessible object which contains the given
 *          #Accessible instance, or NULL if the @obj has no parent container.
 *
 **/
Accessible *
Accessible_getParent (Accessible *obj)
{
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = cspi_object_add (
    Accessibility_Accessible__get_parent (CSPI_OBJREF (obj),
					  cspi_ev ()));

  cspi_return_val_if_ev ("getParent", NULL); 

  return retval;
}

/**
 * Accessible_getChildCount:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the number of children contained by an #Accessible object.
 *
 * Returns: a #long indicating the number of #Accessible children
 *          contained by an #Accessible object. or -1 on exception
 *
 **/
long
Accessible_getChildCount (Accessible *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval = Accessibility_Accessible__get_childCount (CSPI_OBJREF (obj),
					      cspi_ev ());

  cspi_return_val_if_ev ("getChildCount", -1); 

  return retval;
}

/**
 * Accessible_getChildAtIndex:
 * @obj: a pointer to the #Accessible object on which to operate.
 * @childIndex: a #long indicating which child is specified.
 *
 * Get the #Accessible child of an #Accessible object at a given index.
 *
 * Returns: a pointer to the #Accessible child object at index
 *          @childIndex. or NULL on exception
 **/
Accessible *
Accessible_getChildAtIndex (Accessible *obj,
                            long int    childIndex)
{
  Accessible *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = cspi_object_add (
    Accessibility_Accessible_getChildAtIndex (CSPI_OBJREF (obj),
					      childIndex, cspi_ev ()));

  cspi_return_val_if_ev ("getChildAtIndex", NULL);
  return retval;
}

/**
 * Accessible_getIndexInParent:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the index of an #Accessible object in its containing #Accessible.
 *
 * Returns: a #long indicating the index of the #Accessible object
 *          in its parent (i.e. containing) #Accessible instance,
 *          or -1 if @obj has no containing parent or on exception.
 **/
long
Accessible_getIndexInParent (Accessible *obj)
{
  long retval;

  cspi_return_val_if_fail (obj != NULL, -1);

  retval = Accessibility_Accessible_getIndexInParent (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getIndexInparent", -1); 
  return retval;
}

/**
 * Accessible_getRelationSet:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the set of #AccessibleRelation objects which describe this #Accessible object's
 *       relationships with other #Accessible objects.
 *
 * Returns: an array of #AccessibleRelation pointers. or NULL on exception
 **/
AccessibleRelation **
Accessible_getRelationSet (Accessible *obj)
{
  int i;
  int n_relations;
  AccessibleRelation **relations;
  Accessibility_RelationSet *relation_set;

  cspi_return_val_if_fail (obj != NULL, NULL);

  relation_set =
    Accessibility_Accessible_getRelationSet (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getRelationSet", NULL); 
  
  n_relations = relation_set->_length;
  relations = malloc (sizeof (AccessibleRelation *) * (n_relations + 1));
  
  for (i = 0; i < n_relations; ++i)
    {
      relations[i] = cspi_object_add (CORBA_Object_duplicate (
	      relation_set->_buffer[i], cspi_ev ()));
    }
  relations[i] = NULL;

  CORBA_free (relation_set);

  return relations;
}

/**
 * Accessible_getRole:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the UI role of an #Accessible object.
 * A UTF-8 string describing this role can be obtained via Accessible_getRoleName ().
 *
 * Returns: the #AccessibleRole of the object.
 *
 **/
AccessibleRole
Accessible_getRole (Accessible *obj)
{
  Accessibility_Role retval;

  cspi_return_val_if_fail (obj != NULL, SPI_ROLE_INVALID);

  retval = 
    Accessibility_Accessible_getRole (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getRole", SPI_ROLE_INVALID); 

  return cspi_role_from_spi_role (retval);
}

/**
 * Accessible_getRoleName:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get a UTF-8 string describing the role this object plays in the UI.
 * This method will return useful values for roles that fall outside the
 * enumeration used in Accessible_getRole ().
 *
 * Returns: a UTF-8 string specifying the role of this #Accessible object.
 *
 **/
char *
Accessible_getRoleName (Accessible *obj)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, CORBA_string_dup ("invalid"));

  retval = 
    Accessibility_Accessible_getRoleName (CSPI_OBJREF (obj), cspi_ev ());

  cspi_return_val_if_ev ("getRoleName", CORBA_string_dup ("invalid")); 

  return retval;
}

/**
 * Accessible_getStateSet:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Returns: a pointer to an #AccessibleStateSet representing the object's current state.
 **/
AccessibleStateSet *
Accessible_getStateSet (Accessible *obj)
{
  AccessibleStateSet *retval;
  Accessibility_StateSet corba_stateset;
  Accessibility_StateSeq *corba_seq;

  cspi_return_val_if_fail (obj != NULL, NULL);

  corba_stateset = Accessibility_Accessible_getState (
	  CSPI_OBJREF (obj), cspi_ev ());
  cspi_return_val_if_ev ("getState", NULL);

  cspi_return_val_if_fail (corba_stateset != NULL, NULL);
  cspi_return_val_if_fail (cspi_ping (corba_stateset), NULL);
  corba_seq = Accessibility_StateSet_getStates (corba_stateset, cspi_ev ());
  cspi_return_val_if_ev ("getState", NULL);

  retval = spi_state_set_cache_new (corba_seq);

  CORBA_free (corba_seq);
  cspi_release_unref (corba_stateset);

  return retval;
}

/* Interface query methods */

/**
 * Accessible_isAction:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleAction.
 *
 * Returns: #TRUE if @obj implements the #AccessibleAction interface,
 *          #FALSE otherwise.
 **/
SPIBoolean
Accessible_isAction (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Action:1.0");
}

/**
 * Accessible_isApplication:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleApplication.
 *
 * Returns: #TRUE if @obj implements the #AccessibleApplication interface,
 *          #FALSE otherwise.
 **/
SPIBoolean
Accessible_isApplication (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Application:1.0");
}

/**
 * Accessible_isComponent:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleComponent.
 *
 * Returns: #TRUE if @obj implements the #AccessibleComponent interface,
 *          #FALSE otherwise.
 **/
SPIBoolean
Accessible_isComponent (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Component:1.0");
}

/**
 * Accessible_isEditableText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleEditableText.
 *
 * Returns: #TRUE if @obj implements the #AccessibleEditableText interface,
 *          #FALSE otherwise.
 **/
SPIBoolean
Accessible_isEditableText (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/EditableText:1.0");
}

/**
 * Accessible_isHypertext:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleHypertext.
 *
 * Returns: #TRUE if @obj implements the #AccessibleHypertext interface,
 *          #FALSE otherwise.
 **/
SPIBoolean
Accessible_isHypertext (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Hypertext:1.0");
}

/**
 * Accessible_isImage:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleImage.
 *
 * Returns: #TRUE if @obj implements the #AccessibleImage interface,
 *          #FALSE otherwise.
**/
SPIBoolean
Accessible_isImage (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Image:1.0");
}

/**
 * Accessible_isSelection:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleSelection.
 *
 * Returns: #TRUE if @obj implements the #AccessibleSelection interface,
 *          #FALSE otherwise.
**/
SPIBoolean
Accessible_isSelection (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Selection:1.0");
}

/**
 * Accessible_isTable:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleTable.
 *
 * Returns: #TRUE if @obj implements the #AccessibleTable interface,
 *          #FALSE otherwise.
**/
SPIBoolean
Accessible_isTable (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Table:1.0");
}

/**
 * Accessible_isStreamableContent:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements
 *          #AccessibleStreamableContent.
 *
 * Returns: #TRUE if @obj implements the #AccessibleStreamableContent interface,
 *          #FALSE otherwise.
**/
SPIBoolean
Accessible_isStreamableContent (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/StreamableContent:1.0");
}

/**
 * Accessible_isText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleText.
 *
 * Returns: #TRUE if @obj implements the #AccessibleText interface,
 *          #FALSE otherwise.
**/
SPIBoolean
Accessible_isText (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Text:1.0");
}

/**
 * Accessible_isValue:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleValue.
 *
 * Returns: #TRUE if @obj implements the #AccessibleValue interface,
 *          #FALSE otherwise.
**/
SPIBoolean
Accessible_isValue (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      "IDL:Accessibility/Value:1.0");
}

/**
 * Accessible_getApplication:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleApplication interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleApplication interface instance, or
 *          NULL if @obj does not implement #AccessibleApplication.
 **/
AccessibleApplication *
Accessible_getApplication (Accessible *obj)
{
  return (AccessibleApplication *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Application:1.0");
}

/**
 * Accessible_getAction:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleAction interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleAction interface instance, or
 *          NULL if @obj does not implement #AccessibleAction.
 **/
AccessibleAction *
Accessible_getAction (Accessible *obj)
{
  return (AccessibleAction *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Action:1.0");
}

/**
 * Accessible_getComponent:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleComponent interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleComponent interface instance, or
 *          NULL if @obj does not implement #AccessibleComponent.
 **/
AccessibleComponent *
Accessible_getComponent (Accessible *obj)
{
  return (AccessibleComponent *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Component:1.0");
}

/**
 * Accessible_getEditableText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleEditableText interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleEditableText interface instance, or
 *          NULL if @obj does not implement #AccessibleEditableText.
 **/
AccessibleEditableText *
Accessible_getEditableText (Accessible *obj)
{
  return (AccessibleEditableText *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/EditableText:1.0");
}



/**
 * Accessible_getHypertext:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleHypertext interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleHypertext interface instance, or
 *          NULL if @obj does not implement #AccessibleHypertext.
 **/
AccessibleHypertext *
Accessible_getHypertext (Accessible *obj)
{
  return (AccessibleHypertext *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Hypertext:1.0");
}



/**
 * Accessible_getImage:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleImage interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleImage interface instance, or
 *          NULL if @obj does not implement #AccessibleImage.
 **/
AccessibleImage *
Accessible_getImage (Accessible *obj)
{
  return (AccessibleImage *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Image:1.0");
}



/**
 * Accessible_getSelection:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleSelection interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleSelection interface instance, or
 *          NULL if @obj does not implement #AccessibleSelection.
 **/
AccessibleSelection *
Accessible_getSelection (Accessible *obj)
{
  return (AccessibleSelection *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Selection:1.0");
}



/**
 * Accessible_getStreamableContent:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleStreamableContent interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleStreamableContent interface instance, or
 *          NULL if @obj does not implement #AccessibleStreamableContent.
 **/
AccessibleStreamableContent *
Accessible_getStreamableContent (Accessible *obj)
{
  return (AccessibleStreamableContent *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/StreamableContent:1.0");
}

/**
 * Accessible_getTable:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleTable interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleTable interface instance, or
 *          NULL if @obj does not implement #AccessibleTable.
 **/
AccessibleTable *
Accessible_getTable (Accessible *obj)
{
  return (AccessibleTable *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Table:1.0");
}

/**
 * Accessible_getText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleText interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleText interface instance, or
 *          NULL if @obj does not implement #AccessibleText.
 **/
AccessibleText *
Accessible_getText (Accessible *obj)
{
  return (AccessibleText *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Text:1.0");
}



/**
 * Accessible_getValue:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleValue interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleValue interface instance, or
 *          NULL if @obj does not implement #AccessibleValue.
 **/
AccessibleValue *
Accessible_getValue (Accessible *obj)
{
  return (AccessibleValue *) Accessible_queryInterface (
	  obj, "IDL:Accessibility/Value:1.0");
}



/**
 * Accessible_queryInterface:
 * @obj: a pointer to the #Accessible instance to query.
 * @interface_name: a UTF-8 character string specifiying the requested interface.
 *
 * Query an #Accessible object to for a named interface.
 *
 * Returns: an instance of the named interface object, if it is implemented
 *          by @obj, or NULL otherwise.
 *
 **/
AccessibleUnknown *
Accessible_queryInterface (Accessible *obj,
			   const char *interface_name)
{
  Bonobo_Unknown iface;
  
  if (!obj)
    {
      return NULL;
    }

  iface = Accessibility_Accessible_queryInterface (CSPI_OBJREF (obj),
						   interface_name,
						   cspi_ev ());


  cspi_return_val_if_ev ("queryInterface", NULL); 

  /*
   * FIXME: we need to be fairly sure that references are going
   * to mach up if we are going to expose QueryInterface, ie. we
   * can't allow people to do:
   * b = a.qi ("b"); b.unref, b.unref to release a's reference.
   * this should be no real problem though for this level of API
   * user.
   */

  return cspi_object_add (iface);
}


/**
 * AccessibleRelation_ref:
 * @obj: a pointer to the #AccessibleRelation object on which to operate.
 *
 * Increment the reference count for an #AccessibleRelation object.
 *
 **/
void
AccessibleRelation_ref (AccessibleRelation *obj)
{
  cspi_object_ref (obj);
}

/**
 * AccessibleRelation_unref:
 * @obj: a pointer to the #AccessibleRelation object on which to operate.
 *
 * Decrement the reference count for an #AccessibleRelation object.
 *
 **/
void
AccessibleRelation_unref (AccessibleRelation *obj)
{
  cspi_object_unref (obj);
}

static SPIBoolean
cspi_init_relation_type_table (AccessibleRelationType *relation_type_table)
{
  int i;
  for (i = 0; i < Accessibility_RELATION_LAST_DEFINED; ++i)
    {
      relation_type_table [i] = SPI_RELATION_NULL;
    }
  relation_type_table [Accessibility_RELATION_NULL] = SPI_RELATION_NULL;
  relation_type_table [Accessibility_RELATION_LABEL_FOR] = SPI_RELATION_LABEL_FOR;
  relation_type_table [Accessibility_RELATION_LABELLED_BY] = SPI_RELATION_LABELED_BY;
  relation_type_table [Accessibility_RELATION_CONTROLLER_FOR] = SPI_RELATION_CONTROLLER_FOR;
  relation_type_table [Accessibility_RELATION_CONTROLLED_BY] = SPI_RELATION_CONTROLLED_BY;
  relation_type_table [Accessibility_RELATION_MEMBER_OF] = SPI_RELATION_MEMBER_OF;
  relation_type_table [Accessibility_RELATION_TOOLTIP_FOR] = SPI_RELATION_NULL;
  relation_type_table [Accessibility_RELATION_NODE_CHILD_OF] = SPI_RELATION_NODE_CHILD_OF;
  relation_type_table [Accessibility_RELATION_EXTENDED] = SPI_RELATION_EXTENDED;
  relation_type_table [Accessibility_RELATION_FLOWS_TO] = SPI_RELATION_FLOWS_TO;
  relation_type_table [Accessibility_RELATION_FLOWS_FROM] = SPI_RELATION_FLOWS_FROM;
  relation_type_table [Accessibility_RELATION_SUBWINDOW_OF] = SPI_RELATION_SUBWINDOW_OF;
  relation_type_table [Accessibility_RELATION_EMBEDS] = SPI_RELATION_EMBEDS;
  relation_type_table [Accessibility_RELATION_EMBEDDED_BY] = SPI_RELATION_EMBEDDED_BY;
  relation_type_table [Accessibility_RELATION_POPUP_FOR] = SPI_RELATION_POPUP_FOR;
  relation_type_table [Accessibility_RELATION_LAST_DEFINED] = SPI_RELATION_LAST_DEFINED;
}

static AccessibleRelationType
cspi_relation_type_from_spi_relation_type (Accessibility_RelationType type)
{
  /* array is sized according to IDL RelationType because IDL RelationTypes are the index */	
  static AccessibleRelationType cspi_relation_type_table [Accessibility_RELATION_LAST_DEFINED];
  static SPIBoolean is_initialized = FALSE;
  AccessibleRelationType cspi_type;
  if (!is_initialized)
    {
      is_initialized = cspi_init_relation_type_table (cspi_relation_type_table);	    
    }
  if (type >= 0 && type < Accessibility_RELATION_LAST_DEFINED)
    {
      cspi_type = cspi_relation_type_table [type];	    
    }
  else
    {
      cspi_type = SPI_RELATION_NULL;
    }
  return cspi_type; 
}
/**
 * AccessibleRelation_getRelationType:
 * @obj: a pointer to the #AccessibleRelation object to query.
 *
 * Get the type of relationship represented by an #AccessibleRelation.
 *
 * Returns: an #AccessibleRelationType indicating the type of relation
 *         encapsulated in this #AccessibleRelation object.
 *
 **/
AccessibleRelationType
AccessibleRelation_getRelationType (AccessibleRelation *obj)
{
  Accessibility_RelationType retval;
  
  cspi_return_val_if_fail (obj, SPI_RELATION_NULL);
  retval =
    Accessibility_Relation_getRelationType (CSPI_OBJREF (obj), cspi_ev());
  cspi_return_val_if_ev ("getRelationType", SPI_RELATION_NULL);
  return cspi_relation_type_from_spi_relation_type (retval);
}

/**
 * AccessibleRelation_getNTargets:
 * @obj: a pointer to the #AccessibleRelation object to query.
 *
 * Get the number of objects which this relationship has as its
 *       target objects (the subject is the #Accessible from which this
 *       #AccessibleRelation originated).
 *
 * Returns: a short integer indicating how many target objects which the
 *       originating #Accessible object has the #AccessibleRelation
 *       relationship with.
 **/
int
AccessibleRelation_getNTargets (AccessibleRelation *obj)
{
  int retval;
  
  cspi_return_val_if_fail (obj, -1);
  retval = Accessibility_Relation_getNTargets (CSPI_OBJREF (obj), cspi_ev());
  cspi_return_val_if_ev ("getNTargets", -1);
  return retval;
}

/**
 * AccessibleRelation_getTarget:
 * @obj: a pointer to the #AccessibleRelation object to query.
 * @i: a (zero-index) integer indicating which (of possibly several) target is requested.
 *
 * Get the @i-th target of a specified #AccessibleRelation relationship.
 *
 * Returns: an #Accessible which is the @i-th object with which the
 *      originating #Accessible has relationship specified in the
 *      #AccessibleRelation object.
 *
 **/
Accessible *
AccessibleRelation_getTarget (AccessibleRelation *obj, int i)
{
  Accessible *retval;

  cspi_return_val_if_fail (obj, NULL);

  retval = cspi_object_add (
			 Accessibility_Relation_getTarget (CSPI_OBJREF(obj),
							   i, cspi_ev()));
  cspi_return_val_if_ev ("getTarget", NULL);
  return retval;
}

/**
 * AccessibleStateSet_ref:
 * @obj: a pointer to the #AccessibleStateSet object on which to operate.
 *
 * Increment the reference count for an #AccessibleStateSet object.
 *
 **/
void
AccessibleStateSet_ref (AccessibleStateSet *obj)
{
  spi_state_set_cache_ref (obj);
}

/**
 * AccessibleStateSet_unref:
 * @obj: a pointer to the #AccessibleStateSet object on which to operate.
 *
 * Decrement the reference count for an #AccessibleStateSet object.
 *
 **/
void
AccessibleStateSet_unref (AccessibleStateSet *obj)
{
  spi_state_set_cache_unref (obj);
}

static Accessibility_StateType
spi_state_to_corba (AccessibleState state)
{
#define MAP_STATE(a) \
  case SPI_STATE_##a: \
    return Accessibility_STATE_##a

  switch (state)
    {
      MAP_STATE (INVALID);
      MAP_STATE (ACTIVE);
      MAP_STATE (ARMED);
      MAP_STATE (BUSY);
      MAP_STATE (CHECKED);
      MAP_STATE (DEFUNCT);
      MAP_STATE (EDITABLE);
      MAP_STATE (ENABLED);
      MAP_STATE (EXPANDABLE);
      MAP_STATE (EXPANDED);
      MAP_STATE (FOCUSABLE);
      MAP_STATE (FOCUSED);
      MAP_STATE (HORIZONTAL);
      MAP_STATE (ICONIFIED);
      MAP_STATE (MODAL);
      MAP_STATE (MULTI_LINE);
      MAP_STATE (MULTISELECTABLE);
      MAP_STATE (OPAQUE);
      MAP_STATE (PRESSED);
      MAP_STATE (RESIZABLE);
      MAP_STATE (SELECTABLE);
      MAP_STATE (SELECTED);
      MAP_STATE (SENSITIVE);
      MAP_STATE (SHOWING);
      MAP_STATE (SINGLE_LINE);
      MAP_STATE (STALE);
      MAP_STATE (TRANSIENT);
      MAP_STATE (VERTICAL);
      MAP_STATE (VISIBLE);
      MAP_STATE (MANAGES_DESCENDANTS);
      MAP_STATE (INDETERMINATE);
    default:
      return Accessibility_STATE_INVALID;
  }
#undef MAP_STATE
}	      

/**
 * AccessibleStateSet_contains:
 * @obj: a pointer to the #AccessibleStateSet object on which to operate.
 * @state: an #AccessibleState for which the specified #AccessibleStateSet
 *       will be queried.
 *
 * Determine whether a given #AccessibleStateSet includes a given state; that is,
 *       whether @state is true for the stateset in question.
 *
 * Returns: #TRUE if @state is true/included in the given #AccessibleStateSet,
 *          otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleStateSet_contains (AccessibleStateSet *obj,
			     AccessibleState state)
{
  return spi_state_set_cache_contains (obj, spi_state_to_corba (state));
}

/**
 * AccessibleStateSet_add:
 * @obj: a pointer to the #AccessibleStateSet object on which to operate.
 * @state: an #AccessibleState to be added to the specified #AccessibleStateSet
 *
 * Add a particular #AccessibleState to an #AccessibleStateSet (i.e. set the
 *       given state to #TRUE in the stateset.
 *
 **/
void
AccessibleStateSet_add (AccessibleStateSet *obj,
			AccessibleState state)
{
  spi_state_set_cache_add (obj, spi_state_to_corba (state));
}

/**
 * AccessibleStateSet_remove:
 * @obj: a pointer to the #AccessibleStateSet object on which to operate.
 * @state: an #AccessibleState to be removed from the specified #AccessibleStateSet
 *
 * Remove a particular #AccessibleState to an #AccessibleStateSet (i.e. set the
 *       given state to #FALSE in the stateset.)
 *
 **/
void
AccessibleStateSet_remove (AccessibleStateSet *obj,
			   AccessibleState state)
{
  spi_state_set_cache_remove (obj, spi_state_to_corba (state));
}

/**
 * AccessibleStateSet_equals:
 * @obj: a pointer to the first #AccessibleStateSet object on which to operate.
 * @obj2: a pointer to the second #AccessibleStateSet object on which to operate.
 *
 * Determine whether two instances of #AccessibleStateSet are equivalent (i.e.
 *         consist of the same #AccessibleStates).  Useful for checking multiple
 *         state variables at once; construct the target state then compare against it.
 *
 * @see AccessibleStateSet_compare().
 *
 * Returns: #TRUE if the two #AccessibleStateSets are equivalent,
 *          otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleStateSet_equals (AccessibleStateSet *obj,
                           AccessibleStateSet *obj2)
{
  SPIBoolean   eq;
  AtkStateSet *cmp;

  if (obj == obj2)
    {
      return TRUE;
    }

  cmp = spi_state_set_cache_xor (obj, obj2);
  eq = spi_state_set_cache_is_empty (cmp);
  spi_state_set_cache_unref (cmp);

  return eq;
}

/**
 * AccessibleStateSet_compare:
 * @obj: a pointer to the first #AccessibleStateSet object on which to operate.
 * @obj2: a pointer to the second #AccessibleStateSet object on which to operate.
 *
 * Determine the differences between two instances of #AccessibleStateSet.
 * Not Yet Implemented.
 *.
 * @see AccessibleStateSet_equals().
 *
 * Returns: an #AccessibleStateSet object containing all states contained on one of
 *          the two sets but not the other.
 *
 **/
AccessibleStateSet *
AccessibleStateSet_compare (AccessibleStateSet *obj,
                            AccessibleStateSet *obj2)
{
  return spi_state_set_cache_xor (obj, obj2);
}

/**
 * AccessibleStateSet_isEmpty:
 * @obj: a pointer to the #AccessibleStateSet object on which to operate.
 *
 * Determine whether a given #AccessibleStateSet is the empty set.
 *
 * Returns: #TRUE if the given #AccessibleStateSet contains no (true) states,
 *          otherwise #FALSE.
 *
 **/
SPIBoolean
AccessibleStateSet_isEmpty (AccessibleStateSet *obj)
{
  return spi_state_set_cache_is_empty (obj);
}


