#include <stdlib.h> /* for malloc */
#include <cspi/spi-private.h>

static const char *role_names [] =
{
  "<invalid>",
  "accelerator label",
  "alert",
  "animation",
  "arrow",
  "calendar",
  "canvas",
  "check box",
  "check menu item",
  "color chooser",
  "column header",
  "combo box",
  "date editor",
  "desktop icon",
  "desktop frame",
  "dial",
  "dialog",
  "directory pane",
  "file chooser",
  "filler",
  "focus traversable",
  "font chooser",
  "frame",
  "glass pane",
  "HTML container",
  "icon",
  "image",
  "internal frame",
  "label",
  "layered pane",
  "list",
  "list item",
  "menu",
  "menubar",
  "menu item",
  "option pane",
  "page tab",
  "page tab list",
  "panel",
  "password text",
  "popup menu",
  "progress bar",
  "pushbutton",
  "radiobutton",
  "radio menu item",
  "root pane",
  "row header",
  "scrollbar",
  "scrollpane",
  "separator",
  "slider",
  "spin button",
  "split pane",
  "status bar",
  "table",
  "table cell",
  "table column header",
  "table row header",
  "tearoff menu item",
  "text",
  "toggle button",
  "toolbar",
  "tooltip",
  "tree",
  "tree-table",
  "<unknown>",
  "viewport",
  "window",

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
  role_table [Accessibility_ROLE_TEAROFF_MENU_ITEM] = SPI_ROLE_TEAROFF_MENU_ITEM;
  role_table [Accessibility_ROLE_TEXT] = SPI_ROLE_TEXT;
  role_table [Accessibility_ROLE_TOGGLE_BUTTON] = SPI_ROLE_TOGGLE_BUTTON;
  role_table [Accessibility_ROLE_TOOL_BAR] = SPI_ROLE_TOOL_BAR;
  role_table [Accessibility_ROLE_TOOL_TIP] = SPI_ROLE_TOOL_TIP;
  role_table [Accessibility_ROLE_TREE] = SPI_ROLE_TREE;
  role_table [Accessibility_ROLE_TREE_TABLE] = SPI_ROLE_TREE_TABLE;
  role_table [Accessibility_ROLE_UNKNOWN] = SPI_ROLE_UNKNOWN;
  role_table [Accessibility_ROLE_VIEWPORT] = SPI_ROLE_VIEWPORT;
  role_table [Accessibility_ROLE_WINDOW] = SPI_ROLE_WINDOW;
  role_table [Accessibility_ROLE_EXTENDED] = SPI_ROLE_EXTENDED;
  role_table [Accessibility_ROLE_LAST_DEFINED] = SPI_ROLE_EXTENDED;

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
  if (role < MAX_ROLES)
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
  char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = (char *)
    Accessibility_Accessible__get_name (CSPI_OBJREF (obj), cspi_ev ());

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

  retval = (char *)
    Accessibility_Accessible__get_description (CSPI_OBJREF (obj),
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

  retval = (long) 
    Accessibility_Accessible__get_childCount (CSPI_OBJREF (obj),
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

  retval = (long)
    Accessibility_Accessible_getIndexInParent (CSPI_OBJREF (obj), cspi_ev ());

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
  
  /* this looks hack-ish, but it's based on the CORBA C bindings spec */
  n_relations = relation_set->_length;
  relations = malloc (sizeof (AccessibleRelation *) * n_relations);
  
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
  AccessibleRole retval;

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
 * Not Yet Implemented.
 *
 * Returns: a pointer to an #AccessibleStateSet representing the object's current state.
 **/
AccessibleStateSet *
Accessible_getStateSet (Accessible *obj)
{
  return NULL;
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
  AccessibleRelationType retval;
  
  cspi_return_val_if_fail (obj, SPI_RELATION_NULL);
  retval =
    Accessibility_Relation_getRelationType (CSPI_OBJREF (obj), cspi_ev());
  cspi_return_val_if_ev ("getRelationType", SPI_RELATION_NULL);
  return retval;
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
  retval = (int)
    Accessibility_Relation_getNTargets (CSPI_OBJREF (obj), cspi_ev());
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
							   (CORBA_short) i, cspi_ev()));
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
  cspi_object_ref (obj);
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
  cspi_object_unref (obj);
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
  CORBA_boolean retval;

  cspi_return_val_if_fail (obj != NULL, FALSE);

  retval = Accessibility_StateSet_contains (CSPI_OBJREF (obj),
					    state, cspi_ev ());

  cspi_return_val_if_ev ("contains", FALSE);

  return (SPIBoolean) retval;
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
  cspi_return_if_fail (obj != NULL);

  Accessibility_StateSet_add (CSPI_OBJREF (obj), state, cspi_ev ());
  cspi_check_ev ("add");
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
  cspi_return_if_fail (obj != NULL);

  Accessibility_StateSet_remove (CSPI_OBJREF (obj), state, cspi_ev ());
  cspi_check_ev ("remove");
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
  if (obj == obj2)
    {
      return TRUE;
    }

  cspi_return_val_if_fail (obj != NULL, FALSE);
  cspi_return_val_if_fail (obj2 != NULL, FALSE);

  return Accessibility_StateSet_equals (CSPI_OBJREF (obj),
					CSPI_OBJREF (obj2), cspi_ev ());
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
  cspi_return_val_if_fail (obj != NULL, NULL);
  cspi_return_val_if_fail (obj2 != NULL, NULL);
  return NULL;	
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
  cspi_return_val_if_fail (obj != NULL, FALSE);
  return TRUE;	
  /*  return Accessibility_StateSet_isEmpty (CSPI_OBJREF (obj), cspi_ev ());*/
}


