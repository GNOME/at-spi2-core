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
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 *
 * Increment the reference count for an #SpiAccessible object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessible_ref (SpiAccessible *obj)
{
  Accessibility_SpiAccessible_ref (*obj, &ev);
  spi_check_ev (&ev, "ref");
  return 0;
}


/**
 * SpiAccessible_unref:
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 *
 * Decrement the reference count for an #SpiAccessible object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessible_unref (SpiAccessible *obj)
{
  Accessibility_SpiAccessible_unref (*obj, &ev);
  spi_check_ev (&ev, "unref");
  return 0;
}

/**
 * SpiAccessible_getName:
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 *
 * Get the name of an #SpiAccessible object.
 *
 * Returns: a UTF-8 string indicating the name of the #SpiAccessible object.
 *
 **/
char *
SpiAccessible_getName (SpiAccessible *obj)
{
  char *retval = 
    (char *)
    Accessibility_SpiAccessible__get_name (*obj, &ev);
  spi_check_ev (&ev, "getName"); 
  return retval;
}

/**
 * SpiAccessible_getDescription:
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 *
 * Get the description of an #SpiAccessible object.
 *
 * Returns: a UTF-8 string describing the #SpiAccessible object.
 *
 **/
char *
SpiAccessible_getDescription (SpiAccessible *obj)
{
  char *retval = (char *)
    Accessibility_SpiAccessible__get_description (*obj, &ev);
  spi_check_ev (&ev, "getDescription");
  return retval;
}

/**
 * SpiAccessible_getParent:
 * @obj: a pointer to the #SpiAccessible object to query.
 *
 * Get an #SpiAccessible object's parent container.
 *
 * Returns: a pointer tothe #SpiAccessible object which contains the given
 *          #SpiAccessible instance, or NULL if the @obj has no parent container.
 *
 **/
SpiAccessible *
SpiAccessible_getParent (SpiAccessible *obj)
{
  SpiAccessible *retval = 
      Obj_Add (Accessibility_SpiAccessible__get_parent (*obj, &ev));
  spi_check_ev (&ev, "getParent");
  return retval;
}

/**
 * SpiAccessible_getChildCount:
 *
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 *
 * Get the number of children contained by an #SpiAccessible object.
 *
 * Returns: a #long indicating the number of #SpiAccessible children
 *          contained by an #SpiAccessible object.
 *
 **/
long
SpiAccessible_getChildCount (SpiAccessible *obj)
{
  long retval = (long) Accessibility_SpiAccessible__get_childCount (*obj, &ev);
  spi_check_ev (&ev, "getChildCount");
  return retval;
}

/**
 * SpiAccessible_getChildAtIndex:
 *
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 * @childIndex: a #long indicating which child is specified.
 *
 * Get the #SpiAccessible child of an #SpiAccessible object at a given index.
 *
 * Returns: a pointer to the #SpiAccessible child object at index
 *          @childIndex.
 *
 **/
SpiAccessible *
SpiAccessible_getChildAtIndex (SpiAccessible *obj,
                            long childIndex)
{
  SpiAccessible *retval = Obj_Add (Accessibility_SpiAccessible_getChildAtIndex (*obj, childIndex, &ev));
  spi_check_ev (&ev, "getChildAtIndex");
  return retval;
}

/**
 * SpiAccessible_getIndexInParent:
 *
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 *
 * Get the index of an #SpiAccessible object in its containing #SpiAccessible.
 *
 * Returns: a #long indicating the index of the #SpiAccessible object
 *          in its parent (i.e. containing) #SpiAccessible instance,
 *          or -1 if @obj has no containing parent.
 *
 **/
long
SpiAccessible_getIndexInParent (SpiAccessible *obj)
{
  long retval = (long) Accessibility_SpiAccessible_getIndexInParent (*obj, &ev);
  spi_check_ev (&ev, "getIndexInParent");
  return retval;
}

/**
 * SpiAccessible_getRelationSet:
 *
 * Not Yet Implemented.
 *
 * Returns: a pointer to an array of #SpiAccessibleRelations.
 *
 **/
SpiAccessibleRelation **
SpiAccessible_getRelationSet (SpiAccessible *obj)
{
  return NULL;
}

/**
 * SpiAccessible_getRole:
 * @obj: a pointer to the #SpiAccessible object on which to operate.
 *
 * Get the UI role of an #SpiAccessible object.
 *
 * Returns: a UTF-8 string indicating the UI role of the #SpiAccessible object.
 *
 **/
char *
SpiAccessible_getRole (SpiAccessible *obj)
{
  char *retval = SpiAccessible_Role_getName (
		  Accessibility_SpiAccessible_getRole (*obj, &ev));
  spi_check_ev (&ev, "getRole");
  return retval;
}

/**
 * SpiAccessible_getStateSet:
 *
 * Not Yet Implemented.
 *
 * Returns: a pointer to an #SpiAccessibleStateSet representing the object's current state.
 **/
SpiAccessibleStateSet *
SpiAccessible_getStateSet (SpiAccessible *obj)
{
  return NULL;
}

/* Interface query methods */

/**
 * SpiAccessible_isSpiAction:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleAction.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleAction interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isSpiAction (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiAction:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isSpiAction");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isSpiComponent:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleComponent.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleComponent interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isSpiComponent (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiComponent:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isSpiComponent");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isSpiEditableText:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleEditableText.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleEditableText interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isSpiEditableText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiEditableText:1.0",
                                             &ev);
  spi_check_ev (&ev, "isSpiEditableText");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isSpiHypertext:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleHypertext.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleHypertext interface,
 *          #FALSE otherwise.
 **/
boolean
SpiAccessible_isSpiHypertext (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiHypertext:1.0",
                                             &ev);

  spi_check_ev (&ev, "isSpiHypertext");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isSpiImage:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleImage.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleImage interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isSpiImage (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiImage:1.0",
                                             &ev);
  spi_check_ev (&ev, "isSpiImage");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
  * SpiAccessible_isSpiSelection:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleSelection.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleSelection interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isSpiSelection (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiSelection:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isSpiSelection");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;

}

/**
 * SpiAccessible_isSpiTable:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleTable.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleTable interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isSpiTable (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiTable:1.0",
                                             &ev);
  spi_check_ev (&ev, "isSpiTable");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;

}

/**
 * SpiAccessible_isSpiText:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleText.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleText interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isSpiText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiText:1.0",
                                             &ev);
  spi_warn_ev (&ev, "isSpiText");

  return (CORBA_Object_is_nil (iface, &ev)) ? FALSE : TRUE;
}

/**
 * SpiAccessible_isSpiValue:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Query whether the specified #SpiAccessible implements #SpiAccessibleValue.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #SpiAccessibleValue interface,
 *          #FALSE otherwise.
**/
boolean
SpiAccessible_isSpiValue (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiValue:1.0",
                                             &ev);
  spi_check_ev (&ev, "isSpiValue");

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
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiAction:1.0",
                                             &ev);
  spi_check_ev (&ev, "getAction");

  return (SpiAccessibleAction *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}

/**
 * SpiAccessible_getComponent:
 * @obj: a pointer to the #SpiAccessible instance to query.
 *
 * Get the #SpiAccessibleComponent interface for an #SpiAccessible.
 *
 * Returns: a pointer to an #SpiAccessibleComponent interface instance, or
 *          NULL if @obj does not implement #SpiAccessibleComponent.
 **/
SpiAccessibleComponent *
SpiAccessible_getComponent (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiComponent:1.0",
                                             &ev);
  spi_check_ev (&ev, "getComponent");

  return (SpiAccessibleComponent *) ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}

SpiAccessibleEditableText *
SpiAccessible_getEditableText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiEditableText:1.0",
                                             &ev);
  spi_check_ev (&ev, "getEditableText");

  return (SpiAccessibleEditableText *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleHypertext *
SpiAccessible_getHypertext (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiHypertext:1.0",
                                             &ev);
  spi_check_ev (&ev, "getHypertext");

  return (SpiAccessibleHypertext *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleImage *
SpiAccessible_getImage (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiImage:1.0",
                                             &ev);
  spi_check_ev (&ev, "getImage");

  return (SpiAccessibleImage *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleSelection *
SpiAccessible_getSelection (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiSelection:1.0",
                                             &ev);
  spi_warn_ev (&ev, "getSelection");

  return (SpiAccessibleSelection *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleTable *
SpiAccessible_getTable (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiTable:1.0",
                                             &ev);
  spi_check_ev (&ev, "getTable");

  return (SpiAccessibleTable *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}

SpiAccessibleText *
SpiAccessible_getText (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiText:1.0",
                                             &ev);

  spi_check_ev (&ev, "getText"); 

  return (SpiAccessibleText *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



SpiAccessibleValue *
SpiAccessible_getValue (SpiAccessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_SpiAccessible_queryInterface (*obj,
                                             "IDL:Accessibility/SpiValue:1.0",
                                             &ev);
  return (SpiAccessibleValue *)
	  ((CORBA_Object_is_nil (iface, &ev)) ? 0 : Obj_Add (iface));
}



/**
 * SpiAccessible_queryInterface:
 * @obj: a pointer to the #SpiAccessible instance to query.
 * @interface_name: a UTF-8 character string specifiying the requested interface.
 *
 * Query an #SpiAccessible object to for a named interface.
 *
 * Returns: an instance of the named interface object, if it is implemented
 *          by @obj, or NULL otherwise.
 *
 **/
GenericInterface *
SpiAccessible_queryInterface (SpiAccessible *obj, char *interface_name)
{
  GenericInterface iface;
  iface = Accessibility_SpiAccessible_queryInterface (*obj,
                                                    interface_name,
                                                    &ev);
  return (iface != NULL) ? Obj_Add (iface) : NULL;
}

