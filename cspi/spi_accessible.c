#define MAX_ROLES 100

static char *role_names [MAX_ROLES] =
{
  " ",
  "accelerator label",
  "alert",
  "animation",
  "arrow",
  "calendar",
  "canvas",
  "check box",
  "menu item",
  "color chooser",
  "column header",
  "combo box",
  "date editor",
  "desktop icon",
  "desktop frame",
  "dial",
  "dialog",
  "directory pane",
  "drawing area",
  "file chooser",
  "filler",
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
  "split pane",
  "spin button",
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
  " ",
  "viewport",
  "window",
};

/*
 * Returns a localizable string name for an AtkRole enumerated type.
 */
char*
SpiAccessible_Role_getName (Accessibility_Role role)
{
  if (role < MAX_ROLES) return role_names [(int) role];
  else return "";
}



/**
 * SpiAccessible_ref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Increment the reference count for an #Accessible object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessible_ref (SpiAccessible *obj)
{
  Accessibility_Accessible_ref (*obj, &ev);
  spi_check_ev (&ev, "ref");
  return 0;
}


/**
 * SpiAccessible_unref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Decrement the reference count for an #Accessible object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessible_unref (SpiAccessible *obj)
{
  Accessibility_Accessible_unref (*obj, &ev);
  spi_check_ev (&ev, "unref");
  return 0;
}

/**
 * SpiAccessible_getName:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the name of an #Accessible object.
 *
 * Returns: a UTF-8 string indicating the name of the #Accessible object.
 *
 **/
char *
SpiAccessible_getName (SpiAccessible *obj)
{
  char *retval = 
    (char *)
    Accessibility_Accessible__get_name (*obj, &ev);
  spi_check_ev (&ev, "getName"); 
  return retval;
}

/**
 * SpiAccessible_getDescription:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the description of an #Accessible object.
 *
 * Returns: a UTF-8 string describing the #Accessible object.
 *
 **/
char *
SpiAccessible_getDescription (SpiAccessible *obj)
{
  char *retval = (char *)
    Accessibility_Accessible__get_description (*obj, &ev);
  spi_check_ev (&ev, "getDescription");
  return retval;
}

/**
 * SpiAccessible_getParent:
 * @obj: a pointer to the #Accessible object to query.
 *
 * Get an #Accessible object's parent container.
 *
 * Returns: a pointer tothe #Accessible object which contains the given
 *          #Accessible instance, or NULL if the @obj has no parent container.
 *
 **/
SpiAccessible *
SpiAccessible_getParent (SpiAccessible *obj)
{
  SpiAccessible *retval = 
      Obj_Add (Accessibility_Accessible__get_parent (*obj, &ev));
  spi_check_ev (&ev, "getParent");
  return retval;
}

/**
 * SpiAccessible_getChildCount:
 *
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the number of children contained by an #Accessible object.
 *
 * Returns: a #long indicating the number of #Accessible children
 *          contained by an #Accessible object.
 *
 **/
long
SpiAccessible_getChildCount (SpiAccessible *obj)
{
  long retval = (long) Accessibility_Accessible__get_childCount (*obj, &ev);
  spi_check_ev (&ev, "getChildCount");
  return retval;
}

/**
 * SpiAccessible_getChildAtIndex:
 *
 * @obj: a pointer to the #Accessible object on which to operate.
 * @childIndex: a #long indicating which child is specified.
 *
 * Get the #Accessible child of an #Accessible object at a given index.
 *
 * Returns: a pointer to the #Accessible child object at index
 *          @childIndex.
 *
 **/
SpiAccessible *
SpiAccessible_getChildAtIndex (SpiAccessible *obj,
                            long childIndex)
{
  SpiAccessible *retval = Obj_Add (Accessibility_Accessible_getChildAtIndex (*obj, childIndex, &ev));
  spi_check_ev (&ev, "getChildAtIndex");
  return retval;
}

/**
 * SpiAccessible_getIndexInParent:
 *
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the index of an #Accessible object in its containing #Accessible.
 *
 * Returns: a #long indicating the index of the #Accessible object
 *          in its parent (i.e. containing) #Accessible instance,
 *          or -1 if @obj has no containing parent.
 *
 **/
long
SpiAccessible_getIndexInParent (SpiAccessible *obj)
{
  long retval = (long) Accessibility_Accessible_getIndexInParent (*obj, &ev);
  spi_check_ev (&ev, "getIndexInParent");
  return retval;
}

/**
 * SpiAccessible_getRelationSet:
 *
 * Not Yet Implemented.
 *
 * Returns: a pointer to an array of #AccessibleRelations.
 *
 **/
SpiAccessibleRelation **
SpiAccessible_getRelationSet (SpiAccessible *obj)
{
  return NULL;
}

/**
 * SpiAccessible_getRole:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the UI role of an #Accessible object.
 *
 * Returns: a UTF-8 string indicating the UI role of the #Accessible object.
 *
 **/
char *
SpiAccessible_getRole (SpiAccessible *obj)
{
  char *retval = SpiAccessible_Role_getName (
		  Accessibility_Accessible_getRole (*obj, &ev));
  spi_check_ev (&ev, "getRole");
  return retval;
}

/**
 * SpiAccessible_getStateSet:
 *
 * Not Yet Implemented.
 *
 * Returns: a pointer to an #AccessibleStateSet representing the object's current state.
 **/
SpiAccessibleStateSet *
SpiAccessible_getStateSet (SpiAccessible *obj)
{
  return NULL;
}

/* Interface query methods */

/**
 * SpiAccessible_isAction:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleAction.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleAction interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isAction (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Action:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isAction");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isComponent:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleComponent.
 *
 * Returns: #TRUE if @obj implements the #AccessibleComponent interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isComponent (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Component:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isComponent");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isEditableText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleEditableText.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleEditableText interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isEditableText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/EditableText:1.0",
                                             &ev);
  spi_check_ev (&ev, "isEditableText");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isHypertext:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleHypertext.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleHypertext interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isHypertext (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Hypertext:1.0",
                                             &ev);

  spi_check_ev (&ev, "isHypertext");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isImage:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleImage.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleImage interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isImage (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Image:1.0",
                                             &ev);
  spi_check_ev (&ev, "isImage");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
  * SpiAccessible_isSelection:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleSelection.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleSelection interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isSelection (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Selection:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isSelection");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;

}

/**
 * SpiAccessible_isTable:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleTable.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleTable interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isTable (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Table:1.0",
                                             &ev);
  spi_check_ev (&ev, "isTable");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;

}

/**
 * SpiAccessible_isText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleText.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleText interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Text:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isText");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isValue:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleValue.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleValue interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isValue (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Value:1.0",
                                             &ev);
  spi_check_ev (&ev, "isValue");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_getAction:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleAction *
SpiAccessible_getAction (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Action:1.0",
                                             &ev);
  spi_check_ev (&ev, "getAction");

  return (SpiAccessibleAction *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}

/**
 * SpiAccessible_getComponent:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Get the #AccessibleComponent interface for an #Accessible.
 *
 * Returns: a pointer to an #AccessibleComponent interface instance, or
 *          NULL if @obj does not implement #AccessibleComponent.
 **/
SpiAccessibleComponent *
SpiAccessible_getComponent (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Component:1.0",
                                             &ev);
  spi_check_ev (&ev, "getComponent");

  return (SpiAccessibleComponent *) ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}

SpiAccessibleEditableText *
SpiAccessible_getEditableText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/EditableText:1.0",
                                             &ev);
  spi_check_ev (&ev, "getEditableText");

  return (SpiAccessibleEditableText *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleHypertext *
SpiAccessible_getHypertext (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Hypertext:1.0",
                                             &ev);
  spi_check_ev (&ev, "getHypertext");

  return (SpiAccessibleHypertext *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleImage *
SpiAccessible_getImage (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Image:1.0",
                                             &ev);
  spi_check_ev (&ev, "getImage");

  return (SpiAccessibleImage *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleSelection *
SpiAccessible_getSelection (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Selection:1.0",
                                             &ev);
  spi_warn_ev (&ev, "getSelection");

  return (SpiAccessibleSelection *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleTable *
SpiAccessible_getTable (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Table:1.0",
                                             &ev);
  spi_check_ev (&ev, "getTable");

  return (SpiAccessibleTable *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}

SpiAccessibleText *
SpiAccessible_getText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Text:1.0",
                                             &ev);

  spi_check_ev (&ev, "getText"); 

  return (SpiAccessibleText *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleValue *
SpiAccessible_getValue (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Value:1.0",
                                             &ev);
  return (SpiAccessibleValue *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



/**
 * SpiAccessible_queryInterface:
 * @obj: a pointer to the #Accessible instance to query.
 * @interface_name: a UTF-8 character string specifiying the requested interface.
 *
 * Query an #Accessible object to for a named interface.
 *
 * Returns: an instance of the named interface object, if it is implemented
 *          by @obj, or NULL otherwise.
 *
 **/
GenericInterface *
SpiAccessible_queryInterface (SpiAccessible *obj, char *interface_name)
{
  GenericInterface iface;
  iface = Accessibility_Accessible_queryInterface (*obj,
                                                    interface_name,
                                                    &ev);
  return (iface != NULL) ? Obj_Add (iface) : NULL;
}

