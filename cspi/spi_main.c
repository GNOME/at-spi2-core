#include <libbonobo.h>
#include <stdio.h>
#include "spi.h"

static CORBA_Environment ev;
static AccessibilityRegistry registry;

static Accessible *
Obj_Add (Accessible object)
{
  /* TODO: keep list of live object refs */
  Accessible *oref = g_malloc (sizeof (Accessible));
  *oref = object;
  return oref;
}

/*
 *
 * Basic SPI initialization and event loop function prototypes
 *
 */

/**
 * SPI_init:
 *
 * Connects to the accessibility registry and initializes the SPI.
 *
 * Returns: 0 on success, otherwise an integer error code.
 **/
int
SPI_init (void)
{
  int argc = 0;
  CORBA_Object oclient;
  char *obj_id;

  CORBA_exception_init(&ev);

  if (!bonobo_init (&argc, NULL))
    {
      g_error ("Could not initialize Bonobo");
    }

  obj_id = "OAFIID:Accessibility_Registry:proto0.1";

  oclient = bonobo_activation_activate_from_id (obj_id, 0, NULL, &ev);
  if (ev._major != CORBA_NO_EXCEPTION) {
    fprintf (stderr,
            ("AT-SPI error: during registry activation: %s\n"),
            CORBA_exception_id(&ev));
    CORBA_exception_free(&ev);
    exit(-1);
  }

  if (CORBA_Object_is_nil (oclient, &ev))
    {
      g_error ("Could not locate registry");
      exit(-1);
    }

  registry = (Accessibility_Registry) oclient;

  bonobo_activate ();

  return 0;
}

/**
 * SPI_event_main:
 * @isGNOMEApp: a #boolean indicating whether the client of the SPI
 *              will use the Gnome event loop or not.
 *
 * Starts/enters the main event loop for the SPI services.
 *
 * (NOTE: This method does not return control, it is exited via a call to exit()
 * from within an event handler).
 *
 **/
void
SPI_event_main (boolean isGNOMEApp)
{
  if (isGNOMEApp) bonobo_main();
  else CORBA_ORB_run (bonobo_orb(), &ev);
}

/**
 * SPI_event_is_ready:
 *
 * Checks to see if an SPI event is waiting in the event queue.
 * Used by clients that don't wish to use SPI_event_main().
 * Not Yet Implemented.
 *
 * Returns: #TRUE if an event is waiting, otherwise #FALSE.
 *
 **/
boolean
SPI_eventIsReady ()
{
  return FALSE;
}

/**
 * SPI_nextEvent:
 *
 * Gets the next event in the SPI event queue; blocks if no event
 * is pending.
 * Used by clients that don't wish to use SPI_event_main().
 * Not Yet Implemented.
 *
 * Returns: the next #AccessibleEvent in the SPI event queue.
 *
 **/
AccessibleEvent *
SPI_nextEvent (boolean waitForEvent)
{
  return NULL;
}

/**
 * SPI_exit:
 *
 * Disconnects from the Accessibility Registry and releases resources.
 * Not Yet Implemented.
 *
 **/
void
SPI_exit (void)
{
  exit(0);
}

/**
 * createEventListener:
 * @callback : an #AccessibleEventListenerCB callback function, or NULL.
 *
 * Create a new #AccessibleEventListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #AccessibleEventListener.
 *
 **/
AccessibleEventListener *
createEventListener (AccessibleEventListenerCB callback)
{
  AccessibleEventListener *listener = accessible_event_listener_new ();
  if (callback)
    {
      accessible_event_listener_add_callback (listener, callback);
    }
  return listener;
}

/**
 * EventListener_addCallback:
 * @listener: the #AccessibleEventListener instance to modify.
 * @callback: an #AccessibleEventListenerCB function pointer.
 *
 * Add an in-process callback function to an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_addCallback (AccessibleEventListener *listener,
                           AccessibleEventListenerCB callback)
{
  accessible_event_listener_add_callback (listener, callback);
  return TRUE;
}

/**
 * EventListener_removeCallback:
 * @listener: the #AccessibleEventListener instance to modify.
 * @callback: an #AccessibleEventListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_removeCallback (AccessibleEventListener *listener,
                           AccessibleEventListenerCB callback)
{
  accessible_event_listener_remove_callback (listener, callback);
  return TRUE;
}

/*
 *
 * Global functions serviced by the registry
 *
 */

/**
 * registerGlobalEventListener:
 * @listener: the #AccessibleEventListener to be registered against an event type.
 * @callback: a character string indicating the type of events for which
 *            notification is requested.  Format is
 *            EventClass:major_type:minor_type:detail
 *            where all subfields other than EventClass are optional.
 *            EventClasses include "Focus", "Window", "Mouse",
 *            and toolkit events (e.g. "Gtk", "AWT").
 *            Examples: "focus:", "Gtk:GtkWidget:button_press_event".
 *
 * NOTE: this string may be UTF-8, but should not contain byte value 56 (ascii ':'),
 *            except as a delimiter, since non-UTF-8 string delimiting
 *            functions are used internally.  In general, listening to
 *            toolkit-specific events is not recommended.
 *
 * Add an in-process callback function to an existing AccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
registerGlobalEventListener (AccessibleEventListener *listener,
                             char *eventType)
{
  Accessibility_Registry_registerGlobalEventListener (
                         registry,
                         (Accessibility_EventListener)
                            bonobo_object_corba_objref (bonobo_object (listener)),
                         eventType,
                         &ev);

  if (ev._major != CORBA_NO_EXCEPTION)
    {
    return FALSE;
    }
  else
    {
      return TRUE;
    }
}

/**
 * getDesktopCount:
 *
 * Get the number of virtual desktops.
 * NOTE: currently multiple virtual desktops are not implemented, this
 *       function always returns '1'.
 *
 * Returns: an integer indicating the number of active virtual desktops.
 *
 **/
int
getDesktopCount ()
{
  return Accessibility_Registry_getDesktopCount (registry, &ev);
}

/**
 * getDesktop:
 * @i: an integer indicating which of the accessible desktops is to be returned.
 *
 * Get the virtual desktop indicated by index @i.
 * NOTE: currently multiple virtual desktops are not implemented, this
 *       function always returns '1'.
 *
 * Returns: a pointer to the 'i-th' virtual desktop's #Accessible representation.
 *
 **/
Accessible*
getDesktop (int n)
{
  return Obj_Add (Accessibility_Registry_getDesktop (registry, (CORBA_short) n, &ev));
}

/**
 * getDesktopList:
 * @list: a pointer to an array of #Accessible objects.
 *
 * Get the list of virtual desktops.  On return, @list will point
 *     to a newly-created array of virtual desktop pointers.
 *     It is the responsibility of the caller to free this array when
 *     it is no longer needed.
 *
 * Not Yet Implemented.
 *
 * Returns: an integer indicating how many virtual desktops have been
 *          placed in the list pointed to by parameter @list.
 **/
int
getDesktopList (Accessible **list)
{
  *list = NULL;
  return 0;
}

/**
 * registerKeystrokeListener:
 * @listener: a pointer to the #KeystrokeListener for which
 *            keystroke events are requested.
 *
 * Not Yet Implemented.
 *
 **/
void
registerKeystrokeListener (KeystrokeListener *listener)
{
  ;
}

/**
 * generateKeyEvent:
 * @keycode: a #long indicating the keycode of the key event
 *           being synthesized.
 * @meta: a #long indicating the key modifiers to be sent
 *        with the event, if any.
 *
 * Synthesize a keyboard event (as if a hardware keyboard event occurred in the
 * current UI context).
 * Not Yet Implemented.
 *
 **/
void
generateKeyEvent (long keyCode, long meta)
{
  ;
}

/**
 * generateMouseEvent:
 * @x: a #long indicating the screen x coordinate of the mouse event.
 * @y: a #long indicating the screen y coordinate of the mouse event.
 * @name: a string indicating which mouse event to be synthesized
 *        (e.g. "button1", "button2", "mousemove").
 *
 * Synthesize a mouse event at a specific screen coordinate.
 * Not Yet Implemented.
 *
 **/
void
generateMouseEvent (long x, long y, char *name)
{
  ;
}

/*
 *
 * Accessible function prototypes
 *
 */

/**
 * Accessible_ref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Increment the reference count for an #Accessible object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
Accessible_ref (Accessible *obj)
{
  Accessibility_Accessible_ref (*obj, &ev);
  return 0;
}


/**
 * Accessible_unref:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Decrement the reference count for an #Accessible object.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
Accessible_unref (Accessible *obj)
{
  Accessibility_Accessible_unref (*obj, &ev);
  return 0;
}

/**
 * Accessible_getName:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the name of an #Accessible object.
 *
 * Returns: a UTF-8 string indicating the name of the #Accessible object.
 *
 **/
char *
Accessible_getName (Accessible *obj)
{
  return Accessibility_Accessible__get_name (*obj, &ev);
}

/**
 * Accessible_getDescription:
 * @obj: a pointer to the #Accessible object on which to operate.
 *
 * Get the description of an #Accessible object.
 *
 * Returns: a UTF-8 string describing the #Accessible object.
 *
 **/
char *
Accessible_getDescription (Accessible *obj)
{
  return Accessibility_Accessible__get_description (*obj, &ev);
}

/**
 * Accessible_getParent:
 * @obj: a pointer to the #Accessible object to query.
 *
 * Get an #Accessible object's parent container.
 *
 * Returns: a pointer to the #Accessible object which contains the given
 *          #Accessible instance, or NULL if the @obj has no parent container.
 *
 **/
Accessible *
Accessible_getParent (Accessible *obj)
{
  return Obj_Add (Accessibility_Accessible__get_parent (*obj, &ev));
}

/**
 * Accessible_getChildCount:
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
Accessible_getChildCount (Accessible *obj)
{
  return Accessibility_Accessible__get_childCount (*obj, &ev);
}

/**
 * Accessible_getChildAtIndex:
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
Accessible *
Accessible_getChildAtIndex (Accessible *obj,
                            long childIndex)
{
  return Obj_Add (Accessibility_Accessible_getChildAtIndex (*obj, childIndex, &ev));
}

/**
 * Accessible_getIndexInParent:
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
Accessible_getIndexInParent (Accessible *obj)
{
  return Accessibility_Accessible_getIndexInParent (*obj, &ev);
}

/**
 * Accessible_getRelationSet:
 *
 * Not Yet Implemented.
 *
 * Returns: a pointer to an array of #AccessibleRelations.
 *
 **/
AccessibleRelation **
Accessible_getRelationSet (Accessible *obj)
{
  return NULL;
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
char *
Accessible_getRole (Accessible *obj)
{
  return "";
}

/**
 * Accessible_getStateSet:
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
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleAction interface,
 *          #FALSE otherwise.
 **/
boolean
Accessible_isAction (Accessible *obj)
{
  return FALSE;
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
boolean
Accessible_isComponent (Accessible *obj)
{
  Bonobo_Unknown iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Component:1.0",
                                             &ev);
  return (iface != NULL) ? TRUE : FALSE;
}

/**
 * Accessible_isEditableText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleEditableText.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleEditableText interface,
 *          #FALSE otherwise.
 **/
boolean
Accessible_isEditableText (Accessible *obj)
{
  return FALSE;
}

/**
 * Accessible_isHypertext:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleHypertext.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleHypertext interface,
 *          #FALSE otherwise.
 **/
boolean
Accessible_isHypertext (Accessible *obj)
{
  return FALSE;
}

/**
 * Accessible_isImage:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleImage.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleImage interface,
 *          #FALSE otherwise.
**/
boolean
Accessible_isImage (Accessible *obj)
{
  return FALSE;
}

/**
  * Accessible_isSelection:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleSelection.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleSelection interface,
 *          #FALSE otherwise.
**/
boolean
Accessible_isSelection (Accessible *obj)
{
  return FALSE;
}

/**
 * Accessible_isTable:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleTable.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleTable interface,
 *          #FALSE otherwise.
**/
boolean
Accessible_isTable (Accessible *obj)
{
  return FALSE;
}

/**
 * Accessible_isText:
 * @obj: a pointer to the #Accessible instance to query.
 *
 * Query whether the specified #Accessible implements #AccessibleText.
 * Not Yet Implemented.
 *
 * Returns: #TRUE if @obj implements the #AccessibleText interface,
 *          #FALSE otherwise.
**/
boolean
Accessible_isText (Accessible *obj)
{
  return FALSE;
}

/**
 * Accessible_getAction:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleAction *
Accessible_getAction (Accessible *obj)
{
  return NULL;
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
  AccessibleComponent iface =
    Accessibility_Accessible_queryInterface (*obj,
                                             "IDL:Accessibility/Component:1.0",
                                             &ev);
  return Obj_Add (iface);
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
Accessible_queryInterface (Accessible *obj, char *interface_name)
{
  GenericInterface iface;
  iface = Accessibility_Accessible_queryInterface (*obj,
                                                    interface_name,
                                                    &ev);
  return (iface != NULL) ? Obj_Add (iface) : NULL;
}

/*
 *
 * AccessibleApplication function prototypes
 *
 */

/**
 * AccessibleApplication_ref:
 * @obj: a pointer to the #AccessibleApplication on which to operate.
 *
 * Increment the reference count for an #AccessibleApplication.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleApplication_ref (AccessibleApplication *obj)
{
  Accessibility_Application_ref (*obj, &ev);
  return 0;
}

/**
 * AccessibleApplication_unref:
 * @obj: a pointer to the #AccessibleApplication object on which to operate.
 *
 * Decrement the reference count for an #AccessibleApplication.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleApplication_unref (AccessibleApplication *obj)
{
  Accessibility_Application_unref (*obj, &ev);
  return 0;
}

/**
 * AccessibleApplication_getToolkitName:
 * @obj: a pointer to the #AccessibleApplication to query.
 *
 * Get the name of the UI toolkit used by an #AccessibleApplication.
 *
 * Returns: a UTF-8 string indicating which UI toolkit is
 *          used by an application.
 *
 **/
char *
AccessibleApplication_getToolkitName (AccessibleApplication *obj)
{
  return Accessibility_Application__get_toolkitName (*obj, &ev);
}

/**
 * AccessibleApplication_getVersion:
 * @obj: a pointer to the #AccessibleApplication being queried.
 *
 * Get the version of the at-spi bridge exported by an
 *      #AccessibleApplication instance.
 *
 * Returns: a UTF-8 string indicating the application's
 *          at-spi version.
 *
 **/
char *
AccessibleApplication_getVersion (AccessibleApplication *obj)
{
  return Accessibility_Application__get_version (*obj, &ev);
}

/**
 * AccessibleApplication_getID:
 * @obj: a pointer to the #AccessibleApplication being queried.
 *
 * Get the unique ID assigned by the Registry to an
 *      #AccessibleApplication instance.
 * (Not Yet Implemented by the registry).
 *
 * Returns: a unique #long integer associated with the application
 *          by the Registry, or 0 if the application is not registered.
long
AccessibleApplication_getID (AccessibleApplication *obj)
{
  return Accessibility_Application__get_id (*obj, &ev);
}

/**
 * AccessibleApplication_pause:
 *
 * Attempt to pause the application (used when client event queue is
 *  over-full).
 * Not Yet Implemented.
 *
 * Returns: #TRUE if the application was paused successfully, #FALSE otherwise.
 *
 **/
boolean
AccessibleApplication_pause (AccessibleApplication *obj)
{
  return FALSE;
}

/**
 * AccessibleApplication_resume:
 *
 * Attempt to resume the application (used after #AccessibleApplication_pause).
 * Not Yet Implemented.
 *
 * Returns: #TRUE if application processing resumed successfully, #FALSE otherwise.
 *
 **/
boolean
AccessibleApplication_resume (AccessibleApplication *obj)
{
  return FALSE;
}

/*
 *
 * AccessibleComponent function implementations
 *
 */

/**
 * AccessibleComponent_ref:
 * @obj: a pointer to an object implementing #AccessibleComponent on which to operate.
 *
 * Increment the reference count for an #AccessibleComponent.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleComponent_ref (AccessibleComponent *obj)
{
  Accessibility_Component_ref (*obj, &ev);
  return 0;
}

/**
 * AccessibleComponent_unref:
 * @obj: a pointer to the object implementing #AccessibleComponent on which to operate.
 *
 * Decrement the reference count for an #AccessibleComponent.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleComponent_unref (AccessibleComponent *obj)
{
  Accessibility_Component_unref (*obj, &ev);
  return 0;
}

/**
 * AccessibleComponent_contains:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a #long specifying the x coordinate in question.
 * @y: a #long specifying the y coordinate in question.
 * @ctype: the desired coordinate system of the point (@x, @y)
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Query whether a given #AccessibleComponent contains a particular point.
 *
 **/
boolean
AccessibleComponent_contains (AccessibleComponent *obj,
                              long x,
                              long y,
                              AccessibleCoordType ctype)
{
  return Accessibility_Component_contains (*obj,
                                           (CORBA_long) x,
                                           (CORBA_long) y,
                                           ctype,
                                           &ev);
}

/**
 * AccessibleComponent_getAccessibleAtPoint:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a #long specifying the x coordinate of the point in question.
 * @y: a #long specifying the y coordinate of the point in question.
 * @ctype: the coordinate system of the point (@x, @y)
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Get the accessible child at a given coordinate within an #AccessibleComponent.
 *
 * Returns: a pointer to an #Accessible child of the specified component which
 *          contains the point (@x, @y), or NULL of no child contains the point.
 **/
Accessible *
AccessibleComponent_getAccessibleAtPoint (AccessibleComponent *obj,
                                          long x,
                                          long y,
                                          AccessibleCoordType ctype)
{
  Accessible child;
  child = Accessibility_Component_getAccessibleAtPoint(*obj,
                                                       (CORBA_long) x,
                                                       (CORBA_long) y,
                                                       ctype,
                                                       &ev);
  return (child != NULL) ? Obj_Add (child) : NULL;
}

/**
 * AccessibleComponent_getExtents:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a pointer to a #long into which the minimum x coordinate will be returned.
 * @y: a pointer to a #long into which the minimum y coordinate will be returned.
 * @width: a pointer to a #long into which the x extents (width) will be returned.
 * @height: a pointer to a #long into which the y extents (height) will be returned.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Get the bounding box of the specified #AccessibleComponent.
 *
 **/
void
AccessibleComponent_getExtents (AccessibleComponent *obj,
                                long *x,
                                long *y,
                                long *width,
                                long *height,
                                AccessibleCoordType ctype)
{
  /* TODO: remove assumption that CORBA_long == long in typecast */
  Accessibility_Component_getExtents (*obj,
                                     (CORBA_long *) x,
                                     (CORBA_long *) y,
                                     (CORBA_long *) width,
                                     (CORBA_long *) height,
                                     ctype,
                                     &ev);
}

/**
 * AccessibleComponent_getPosition:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @x: a pointer to a #long into which the minimum x coordinate will be returned.
 * @y: a pointer to a #long into which the minimum y coordinate will be returned.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Get the minimum x and y coordinates of the specified #AccessibleComponent.
 *
 **/
void
AccessibleComponent_getPosition (AccessibleComponent *obj,
                                 long *x,
                                 long *y,
                                 AccessibleCoordType ctype)
{
  Accessibility_Component_getPosition (*obj,
                                       (CORBA_long *) x,
                                       (CORBA_long *) y,
                                       ctype,
                                       &ev);
}

/**
 * AccessibleComponent_getSize:
 * @obj: a pointer to the #AccessibleComponent to query.
 * @width: a pointer to a #long into which the x extents (width) will be returned.
 * @height: a pointer to a #long into which the y extents (height) will be returned.
 *
 * Get the size of the specified #AccessibleComponent.
 *
 **/
void
AccessibleComponent_getSize (AccessibleComponent *obj,
                             long *width,
                             long *height)
{
  Accessibility_Component_getSize (*obj,
                                   (CORBA_long *) width,
                                   (CORBA_long *) height,
                                   &ev);
}

/* Not Yet Implemented */
void
AccessibleComponent_grabFocus (AccessibleComponent *obj)
{
  ;
}
