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
#include "spi-common/spi-stateset.h"
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
  "application",
  "autocomplete",
  "editbar",
  "embedded",
  "entry",
  "chart",
  "caption",
  "document_frame",
  "heading",
  "page",
  "section",
  "form",
  "redundant object",
  "link",
  "input method window"
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
  role_table [Accessibility_ROLE_EDITBAR] = SPI_ROLE_EDITBAR;
  role_table [Accessibility_ROLE_EMBEDDED] = SPI_ROLE_EMBEDDED;
  role_table [Accessibility_ROLE_ENTRY] = SPI_ROLE_ENTRY;
  role_table [Accessibility_ROLE_CHART] = SPI_ROLE_CHART;
  role_table [Accessibility_ROLE_CAPTION] = SPI_ROLE_CAPTION;
  role_table [Accessibility_ROLE_DOCUMENT_FRAME] = SPI_ROLE_DOCUMENT_FRAME;
  role_table [Accessibility_ROLE_HEADING] = SPI_ROLE_HEADING;
  role_table [Accessibility_ROLE_PAGE] = SPI_ROLE_PAGE;
  role_table [Accessibility_ROLE_SECTION] = SPI_ROLE_SECTION;
  role_table [Accessibility_ROLE_FORM] = SPI_ROLE_FORM;
  role_table [Accessibility_ROLE_REDUNDANT_OBJECT] = SPI_ROLE_REDUNDANT_OBJECT;
  role_table [Accessibility_ROLE_LINK] = SPI_ROLE_LINK;
  role_table [Accessibility_ROLE_INPUT_METHOD_WINDOW] = SPI_ROLE_INPUT_METHOD_WINDOW;

  return TRUE;
}

AccessibleRole
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

AccessibleAttributeSet *
_cspi_attribute_set_from_sequence (const GArray *seq)
{
    AccessibleAttributeSet *set = g_new0 (AccessibleAttributeSet, 1);
    int i;

    set->len = seq->len;
    set->attributes = g_new0 (char *, set->len);
    for (i = 0; i < set->len; ++i)
    {
	set->attributes[i] = g_array_index (seq, char *, i);
    }

  g_array_free (seq, TRUE);
    return set;
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
      return g_strdup (role_names [(int) role]);
    }
  else
    {
      return g_strdup ("");
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
  cspi_return_val_if_fail (obj != NULL, NULL);
  return g_strdup (obj->name);
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
  cspi_return_val_if_fail (obj != NULL, NULL);

  return g_strdup (obj->description);
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
  cspi_return_val_if_fail (obj != NULL, NULL);

  return cspi_object_add (obj->parent);
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
  cspi_return_val_if_fail (obj != NULL, -1);

  return g_list_length (obj->children);
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
  Accessible *child;

  cspi_return_val_if_fail (obj != NULL, NULL);

  child = g_list_nth_data (obj->children, childIndex);
  return cspi_object_add(child);
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
  GList *l;
  gint i;

  cspi_return_val_if_fail (obj != NULL, -1);
  if (!obj->parent) return -1;
  l = obj->parent->children;
  while (l)
  {
    if (l->data == obj) return i;
    l = g_list_next (l);
    i++;
  }
  return -1;
}

typedef struct
{
  dbus_uint32_t type;
  GArray *targets;
} Accessibility_Relation;

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
  GArray *relation_set;
  DBusError error;

  cspi_return_val_if_fail (obj != NULL, NULL);

  g_assert (!cspi_exception ());

  dbus_error_init (&error);
  cspi_dbus_call (obj, spi_interface_accessible, "getAttributes", &error, "=>a(uao)", &relation_set);

  cspi_return_val_if_ev ("getRelationSet", NULL); 
  
  n_relations = relation_set->len;
  relations = malloc (sizeof (AccessibleRelation *) * (n_relations + 1));
  
  for (i = 0; i < n_relations; ++i)
    {
      Accessibility_Relation *r = g_array_index (relation_set, Accessibility_Relation *, i);
      relations[i] = g_new (AccessibleRelation, 1);
      if (!relations[i]) continue;
      relations[i]->ref_count = 1;
      relations[i]->type = r->type;
      relations[i]->targets = r->targets;
    }
  relations[i] = NULL;

  g_array_free (relation_set, TRUE);

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
  cspi_return_val_if_fail (obj != NULL, SPI_ROLE_INVALID);

  return obj->role;
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

  cspi_return_val_if_fail (obj != NULL, g_strdup ("invalid"));

  cspi_dbus_call (obj, spi_interface_accessible, "getRoleName", NULL, "=>s", &retval);

  cspi_return_val_if_ev ("getRoleName", g_strdup ("invalid")); 

  return retval;
}

/**
 * Accessible_getLocalizedRoleName:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get a UTF-8 string describing the (localized) role this object plays in the UI.
 * This method will return useful values for roles that fall outside the
 * enumeration used in Accessible_getRole ().
 *
 * @Since: AT-SPI 1.4
 *
 * Returns: a UTF-8 string specifying the role of this #Accessible object.
 *
 **/
char *
Accessible_getLocalizedRoleName (Accessible *obj)
{
  char *retval;

  cspi_return_val_if_fail (obj != NULL, g_strdup ("invalid"));

  cspi_dbus_call (obj, spi_interface_accessible, "getLocalizedRoleName", NULL, "=>s", &retval);

  cspi_return_val_if_ev ("getLocalizedRoleName", g_strdup ("invalid")); 

  return retval;
}

/**
 * Accessible_getStateSet:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Gets the current state of an object.
 *
 * Returns: a pointer to an #AccessibleStateSet representing the object's current state.
 **/
AccessibleStateSet *
Accessible_getStateSet (Accessible *obj)
{
  return obj->states;
}

/**
 * Accessible_getAttributes:
 * @obj: The #Accessible being queried.
 *
 * Get the #AttributeSet representing any assigned 
 * name-value pair attributes or annotations for this object.
 * For typographic, textual, or textually-semantic attributes, see
 * AccessibleText_getAttributes instead.
 *
 * Returns: The name-value-pair attributes assigned to this object.
 */
AccessibleAttributeSet *
Accessible_getAttributes (Accessible *obj)
{
    AccessibleAttributeSet *retval;
  GArray *dbus_seq;

    cspi_return_val_if_fail (obj != NULL, NULL);

  cspi_dbus_call (obj, spi_interface_accessible, "getAttributes", NULL, "=>as", &dbus_seq);

    cspi_return_val_if_ev ("getAttributes", NULL);
    
    retval = _cspi_attribute_set_from_sequence (dbus_seq);

    return retval;
}

/**
 * Accessible_getHostApplication:
 * @obj: The #Accessible being queried.
 *
 * Get the containing #AccessibleApplication for an object.
 *
 * Returns: the containing AccessibleApplication instance for this object.
 */
AccessibleApplication *
Accessible_getHostApplication (Accessible *obj)
{
  while (obj->parent) obj = obj->parent;
  return obj;
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
			      spi_interface_action);
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
			      spi_interface_application);
}

/**                      
 * Accessible_isCollection:                                                                                                                                                                          * @obj: a pointer to the #Accessible instance to query.                                                                                                                                          
 *                          
 * Query whether the specified #Accessible implements #AccessibleCollection.    
 * Returns: #TRUE if @obj implements the #AccessibleCollection interface,                                                                                                               
 *          #FALSE otherwise.
 **/

SPIBoolean
Accessible_isCollection (Accessible *obj)
{
#if 0
     g_warning ("Collections not implemented");
     return cspi_accessible_is_a (obj,
			      spi_interface_collection);
#else
     return FALSE;
#endif
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
			      spi_interface_component);
}

/**
 * Accessible_isDocument:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleDocument.
 *
 * Returns: #TRUE if @obj implements the #AccessibleDocument interface,
 *          #FALSE otherwise.
 **/
SPIBoolean
Accessible_isDocument (Accessible *obj)
{
  return cspi_accessible_is_a (obj,
			      spi_interface_document);
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
			      spi_interface_editable_text);
}
                                                                                                                                                                        
/**
 * Accessible_isMatchRule:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleMatchRule.
 *
 * Returns: #TRUE if @obj implements the #AccessibleMatchRule interface,
 *          #FALSE otherwise.
 **/
SPIBoolean
Accessible_isMatchRule (Accessible *obj)
{
#if 0
     return cspi_accessible_is_a (obj, 
				  spi_interface_match_rule);
#else
     g_warning ("Match rules not implemented");
     return FALSE;
#endif
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
			      spi_interface_hypertext);
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
			      spi_interface_image);
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
			      spi_interface_selection);
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
			      spi_interface_table);
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
#if 0
  return cspi_accessible_is_a (obj,
			      spi_interface_streamable_content);
#else
  g_warning ("Streamable content not implemented");
  return FALSE;
#endif
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
			      spi_interface_text);
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
			      spi_interface_value);
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
	  obj, spi_interface_application);
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
	  obj, spi_interface_action);
}


/**
 * Accessible_getCollection:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleCollection interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleCollection interface instance, or
 *          NULL if @obj does not implement #AccessibleCollection.
 **/
AccessibleCollection *
Accessible_getCollection (Accessible *obj)
{
#if 0
  return (AccessibleCollection *) Accessible_queryInterface (
	  obj, spi_interface_collection);
#else
  g_warning ("Collections not implemented");
#endif
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
	  obj, spi_interface_component);
}

/**
 * Accessible_getDocument:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleDocument interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleDocument interface instance, or
 *          NULL if @obj does not implement #AccessibleDocument.
 **/
AccessibleDocument *
Accessible_getDocument (Accessible *obj)
{
  return (AccessibleDocument *) Accessible_queryInterface (
	  obj, spi_interface_document);
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
	  obj, spi_interface_editable_text);
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
	  obj, spi_interface_hypertext);
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
	  obj, spi_interface_image);
}

/**
 * Accessible_getMatchRule:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleMatchRule interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleMatchRule interface instance, or
 *          NULL if @obj does not implement #AccessibleMatchRule.
 **/
AccessibleMatchRule *
Accessible_getMatchRule (Accessible *obj)
{
#if 0
  return (AccessibleMatchRule *) Accessible_queryInterface (
	  obj, spi_interface_match_rule);
#else
  g_warning ("Match rules not supported");
#endif
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
	  obj, spi_interface_selection);
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
#if 0
  return (AccessibleStreamableContent *) Accessible_queryInterface (
	  obj, spi_interface_streamable_content);
#else
  g_warning ("Streamable content not supported");
#endif
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
	  obj, spi_interface_table);
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
	  obj, spi_interface_text);
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
	  obj, spi_interface_value);
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
  if (cspi_accessible_is_a (obj, interface_name))
  {
    /* The original code called cspi_object_add(obj) instead, but gok and
     * simple-at don't treat interfaces as references, so I'm confused
     * and not going to replicate this bit of code */
    return obj;
  }
  return NULL;
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
  obj->ref_count++;
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
  obj->ref_count--;
  if (obj->ref_count <= 0)
  {
    g_array_free (obj->targets, TRUE);
    g_free (obj);
  }
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
  relation_type_table [Accessibility_RELATION_PARENT_WINDOW_OF] = SPI_RELATION_PARENT_WINDOW_OF;
  relation_type_table [Accessibility_RELATION_DESCRIBED_BY] = SPI_RELATION_DESCRIBED_BY;
  relation_type_table [Accessibility_RELATION_DESCRIPTION_FOR] = SPI_RELATION_DESCRIPTION_FOR;
  return TRUE;
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
  cspi_return_val_if_fail (obj, SPI_RELATION_NULL);
  return cspi_relation_type_from_spi_relation_type (obj->type);
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
  cspi_return_val_if_fail (obj, -1);
  return obj->targets->len;
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
  cspi_return_val_if_fail (obj, NULL);

  if (i < 0 || i >= obj->targets->len) return NULL;
  return cspi_object_add (
			 g_array_index (obj->targets, Accessible *, i));
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
spi_state_to_dbus (AccessibleState state)
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
      MAP_STATE (TRUNCATED);
      MAP_STATE (REQUIRED);
      MAP_STATE (INVALID_ENTRY);
      MAP_STATE (SUPPORTS_AUTOCOMPLETION);
      MAP_STATE (SELECTABLE_TEXT);
      MAP_STATE (IS_DEFAULT);
      MAP_STATE (VISITED);
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
  return spi_state_set_cache_contains (obj, spi_state_to_dbus (state));
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
  spi_state_set_cache_add (obj, spi_state_to_dbus (state));
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
  spi_state_set_cache_remove (obj, spi_state_to_dbus (state));
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

