#include <stdlib.h> /* for malloc */
#include <cspi/spi-private.h>

static const char *role_names [] =
{
  "<invalid>",
  "alert",
  "canvas",
  "check box",
  "color chooser",
  "column header",
  "combo box",
  "desktop icon",
  "desktop frame",
  "dialog",
  "directory pane",
  "file chooser",
  "filler",
  "focus traversable",
  "frame",
  "glass pane",
  "HTML container",
  "icon",
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
  "root pane",
  "row header",
  "scrollbar",
  "scrollpane",
  "separator",
  "slider",
  "split pane",
  "table",
  "table cell",
  "table column header",
  "table row header",
  "text",
  "toggle button",
  "toolbar",
  "tooltip",
  "tree",
  "<unknown>",
  "viewport",
  "window",

  /* These have no equivalent AccessibleRole enum values */
  "accelerator label",
  "animation",
  "arrow",
  "calendar",
  "menu item",
  "date editor",
  "dial",
  "drawing area",
  "font chooser",
  "image",
  "radio menu item",
  "tearoff menu item",
  "spin button",
  "status bar",
};

#define MAX_ROLES (sizeof (role_names) / sizeof (char *))

/**
 * AccessibleRole_getName:
 * @role: an #AccessibleRole object to query.
 *
 * Get a localizeable string that indicates the name of an #AccessibleRole.
 *
 * Returns: a localizable string name for an #AccessibleRole enumerated type.
 **/
const char *
AccessibleRole_getName (AccessibleRole role)
{
  if (role < MAX_ROLES)
    {
      return role_names [(int) role];
    }
  else
    {
      return "";
    }
  /*
   * TODO: replace with implementation linked to ATK, which
   *  now supports Role/Name mapping
   */
}

/**
 * Accessible_ref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Increment the reference count for an #Accessible object.
 *
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
 *
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
  relations[i] = CORBA_OBJECT_NIL;

  CORBA_free (relation_set);

  return relations;
}

/**
 * Accessible_getRole:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the UI role of an #Accessible object.
 *
 * Returns: a UTF-8 string indicating the UI role of the #Accessible object.
 *
 **/
const char *
Accessible_getRole (Accessible *obj)
{
  const char *retval;

  cspi_return_val_if_fail (obj != NULL, NULL);

  retval = AccessibleRole_getName (
    Accessibility_Accessible_getRole (CSPI_OBJREF (obj), cspi_ev ()));

  cspi_return_val_if_ev ("getRole", NULL); 

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
GenericInterface *
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
  cspi_return_val_if_fail (obj != NULL, -1);
  return 0;
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
  cspi_return_val_if_fail (obj != NULL, -1);
  return 0;
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
  cspi_return_val_if_fail (obj != NULL, NULL);
  return NULL;
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


