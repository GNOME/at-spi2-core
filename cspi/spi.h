
#ifndef _SPI_H
#define _SPI_H

/* Implementation private definitions */
#include "spi-impl.h"

/*
 * Definitions for ACCESSIBLE_STATE , ACCESSIBLE_ROLE, AccessibleEvent,
 *      and event listeners.
 */

#include "spi-roletypes.h"
#include "spi-statetypes.h"
#include "spi-listener.h"

/*
 *
 * Enumerated type for text boundary types
 *
 */

typedef enum
{
  TEXT_BOUNDARY_CHAR,
  TEXT_BOUNDARY_CURSOR_POS,
  TEXT_BOUNDARY_WORD_START,
  TEXT_BOUNDARY_WORD_END,
  TEXT_BOUNDARY_SENTENCE_START,
  TEXT_BOUNDARY_SENTENCE_END,
  TEXT_BOUNDARY_LINE_START,
  TEXT_BOUNDARY_LINE_END,
  TEXT_BOUNDARY_ATTRIBUTE_RANGE
} TEXT_BOUNDARY_TYPE;



/*
 *
 * Enumerated type for relation types
 *
 */

typedef enum
{
  RELATION_LABEL_FOR,
  RELATION_LABELED_BY,
  RELATION_CONTROLLER_FOR,
  RELATION_CONTROLLED_BY,
  RELATION_MEMBER_OF
} RELATION_TYPE;


/* don't change the order of these ! */
typedef enum _AccessibleCoordType {
  COORD_TYPE_SCREEN,
  COORD_TYPE_WINDOW
} AccessibleCoordType;

typedef enum _KeyEventType {
  KEY_PRESSED,
  KEY_RELEASED
} KeyEventType;

typedef struct _KeyStroke
{
	long keyID;
	KeyEventType type;
	unsigned short modifiers;
} KeyStroke;


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
SPI_init (void);

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
SPI_event_main (boolean isGNOMEApp);

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
SPI_eventIsReady ();

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
SPI_nextEvent (boolean waitForEvent);

/**
 * SPI_exit:
 *
 * Disconnects from the Accessibility Registry and releases resources.
 * Not Yet Implemented.
 *
 **/
void
SPI_exit (void);

/*
 * Event Listener creation and support.
 */

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
createEventListener (AccessibleEventListenerCB callback);

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
                           AccessibleEventListenerCB callback);

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
                              AccessibleEventListenerCB callback);

/**
 * createKeyListener:
 * @callback : an #KeystrokeListenerCB callback function, or NULL.
 *
 * Create a new #KeystrokeListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #KeystrokeListener.
 *
 **/
KeystrokeListener *
createKeystrokeListener (KeystrokeListenerCB callback);

/**
 * KeystrokeListener_addCallback:
 * @listener: the #KeystrokeListener instance to modify.
 * @callback: an #KeystrokeListenerCB function pointer.
 *
 * Add an in-process callback function to an existing #KeystrokeListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
KeystrokeListener_addCallback (KeystrokeListener *listener,
			       KeystrokeListenerCB callback);

/**
 * KeystrokeListener_removeCallback:
 * @listener: the #KeystrokeListener instance to modify.
 * @callback: an #KeystrokeListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing #KeystrokeListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
KeystrokeListener_removeCallback (KeystrokeListener *listener,
				  KeystrokeListenerCB callback);

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
                             char *eventType);

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
getDesktopCount ();

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
getDesktop (int n);

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
getDesktopList (Accessible **list);

/**
 * registerKeystrokeListener:
 * @listener: a pointer to the #KeystrokeListener for which
 *            keystroke events are requested.
 *
 * Not Yet Implemented.
 *
 **/
void
registerKeystrokeListener (KeystrokeListener *listener, KeyMaskType keymask);

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
generateKeyEvent (long keyCode, long meta);

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
generateMouseEvent (long x, long y, char *name);

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
Accessible_ref (Accessible *obj);

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
Accessible_unref (Accessible *obj);

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
Accessible_getName (Accessible *obj);

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
Accessible_getDescription (Accessible *obj);

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
Accessible_getParent (Accessible *obj);

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
Accessible_getChildCount (Accessible *obj);

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
                            long childIndex);

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
Accessible_getIndexInParent (Accessible *obj);

/**
 * Accessible_getRelationSet:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleRelation **
Accessible_getRelationSet (Accessible *obj);

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
Accessible_getRole (Accessible *obj);

/**
 * Accessible_getStateSet:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleStateSet *
Accessible_getStateSet (Accessible *obj);

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
Accessible_isAction (Accessible *obj);

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
Accessible_isComponent (Accessible *obj);

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
Accessible_isEditableText (Accessible *obj);

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
Accessible_isHypertext (Accessible *obj);

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
Accessible_isImage (Accessible *obj);

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
Accessible_isSelection (Accessible *obj);

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
Accessible_isTable (Accessible *obj);

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
Accessible_isText (Accessible *obj);

/**
 * Accessible_getAction:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleAction *
Accessible_getAction (Accessible *obj);

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
Accessible_getComponent (Accessible *obj);

/**
 * Accessible_getEditableText:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleEditableText *
Accessible_getEditableText (Accessible *obj);

/**
 * Accessible_getHypertext:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleHypertext *
Accessible_getHypertext (Accessible *obj);

/**
 * Accessible_getImage:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleImage *
Accessible_getImage (Accessible *obj);

/**
 * Accessible_getSelection:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleSelection *
Accessible_getSelection (Accessible *obj);

/**
 * Accessible_getTable:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleTable *
Accessible_getTable (Accessible *obj);

/**
 * Accessible_getText:
 *
 * Not Yet Implemented.
 *
 **/
AccessibleText *
Accessible_getText (Accessible *obj);

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
Accessible_queryInterface (Accessible *obj, char *interface_name);

/*
 *
 * AccessibleAction function prototypes
 *
 */

int
AccessibleAction_ref (
                      AccessibleAction *obj);

int
AccessibleAction_unref (AccessibleAction *obj);

long
AccessibleAction_getNActions (AccessibleAction *obj);

/**
 * AccessibleAction_getDescription:
 * @obj: a pointer to the #AccessibleAction to query.
 *
 * Get the description of 'i-th' action invokable on an
 *      object implementing #AccessibleAction.
 *
 * Not Yet Implemented.
 *
 * Returns: a UTF-8 string describing the 'i-th' invokable action.
 *
 **/
char *
AccessibleAction_getDescription (AccessibleAction *obj,
                                 long index);

boolean
AccessibleAction_doAction (AccessibleAction *obj,
                           long index);

char *
AccessibleAction_getKeyBinding (AccessibleAction *obj,
                                long index);

/*
 *
 * AccessibleApplication function prototypes
 *
 */

/**
 * AccessibleApplication_unref:
 * @obj: a pointer to the #AccessibleApplication on which to operate.
 *
 * Decrement the reference count for an #AccessibleApplication.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
AccessibleApplication_ref (AccessibleApplication *obj);

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
AccessibleApplication_unref (AccessibleApplication *obj);

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
AccessibleApplication_getToolkitName (AccessibleApplication *obj);

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
AccessibleApplication_getVersion (AccessibleApplication *obj);

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
 **/
long
AccessibleApplication_getID (AccessibleApplication *obj);

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
AccessibleApplication_pause (AccessibleApplication *obj);

/**
 * AccessibleApplication_pause:
 *
 * Attempt to resume the application (used after #AccessibleApplication_pause).
 * Not Yet Implemented.
 *
 * Returns: #TRUE if application processing resumed successfully, #FALSE otherwise.
 *
 **/
boolean
AccessibleApplication_resume (AccessibleApplication *obj);

/*
 *
 * AccessibleComponent function prototypes
 *
 */

int
AccessibleComponent_ref (AccessibleComponent *obj);

int
AccessibleComponent_unref (AccessibleComponent *obj);

boolean
AccessibleComponent_contains (AccessibleComponent *obj,
                              long x,
                              long y,
                              AccessibleCoordType ctype);

Accessible *
AccessibleComponent_getAccessibleAtPoint (AccessibleComponent *obj,
                                          long x,
                                          long y,
                                          AccessibleCoordType ctype);

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
                                AccessibleCoordType ctype);

void
AccessibleComponent_getPosition (AccessibleComponent *obj,
                                 long *x,
                                 long *y,
                                 AccessibleCoordType ctype);

void
AccessibleComponent_getSize (AccessibleComponent *obj,
                             long *width,
                             long *height);

void
AccessibleComponent_grabFocus (AccessibleComponent *obj);

/*
 *
 * AccessibleEditableText function prototypes
 *
 */

int
AccessibleEditableText_ref (AccessibleEditableText *obj);

int
AccessibleEditableText_unref (AccessibleEditableText *obj);

boolean
AccessibleEditableText_setRunAttributes (AccessibleEditableText *obj,
					 const char *attributes,
					 long startPos, long endPos);

void
AccessibleEditableText_setTextContents (AccessibleEditableText *obj,
                                        const char *newContents);

void
AccessibleEditableText_insertText (AccessibleEditableText *obj,
                                   long position,
                                   char *text,
                                   long length);

void
AccessibleEditableText_copyText (AccessibleText *obj,
                                 long startPos,
                                 long endPos);

void
AccessibleEditableText_cutText (AccessibleEditableText *obj,
                                long startPos,
                                long endPos);

void
AccessibleEditableText_deleteText (AccessibleEditableText *obj,
                                   long startPos,
                                   long endPos);

void
AccessibleEditableText_pasteText (AccessibleEditableText *obj,
                                  long position);

/*
 *
 * AccessibleHyperlink function prototypes
 *
 */

int
AccessibleHyperlink_ref (AccessibleHyperlink *obj);

int
AccessibleHyperlink_unref (AccessibleHyperlink *obj);

long
AccessibleHyperlink_getNAnchors (AccessibleHyperlink *obj);

char *
AccessibleHyperlink_getURI (AccessibleHyperlink *obj,
                            long i);

Accessible
AccessibleHyperlink_getObject (AccessibleHyperlink *obj,
                               long i);

void
AccessibleHyperlink_getIndexRange (AccessibleHyperlink *obj,
                                   long *startIndex,
                                   long *endIndex);

boolean
AccessibleHyperlink_isValid (AccessibleHyperlink *obj);

/*
 *
 * AccessibleHypertext function prototypes
 *
 */

int
AccessibleHypertext_ref (AccessibleHypertext *obj);

int
AccessibleHypertext_unref (AccessibleHypertext *obj);

long
AccessibleHypertext_getNLinks (AccessibleHypertext *obj);

AccessibleHyperlink *
AccessibleHyperText_getLink (AccessibleHypertext *obj,
                             long linkIndex);

long
AccessibleHypertext_getLinkIndex (AccessibleHypertext *obj,
                                  long characterIndex);

/*
 *
 * AccessibleImage function prototypes
 *
 */

int
AccessibleImage_ref (AccessibleImage *obj);

int
AccessibleImage_unref (AccessibleImage *obj);

char *
AccessibleImage_getImageDescription (AccessibleImage *obj);

void
AccessibleImage_getImageSize (AccessibleImage *obj,
                              long *width,
                              long *height);

void
AccessibleImage_getImagePosition (AccessibleImage *obj,
                                  long *x,
                                  long *y,
                                  AccessibleCoordType ctype);

/*
 *
 * AccessibleRelation function prototypes
 *
 */

int
AccessibleRelation_ref (AccessibleRelation *obj);

int
AccessibleRelation_unref (AccessibleRelation *obj);

RELATION_TYPE
AccessibleRelation_getRelationType (AccessibleRelation *obj);

Accessible *
AccessibleRelation_getTarget (AccessibleRelation *obj);



/*
 *
 * AccessibleSelection function prototypes
 *
 */

int
AccessibleSelection_ref (AccessibleSelection *obj);

int
AccessibleSelection_unref (AccessibleSelection *obj);

long
AccessibleSelection_getNSelectedChildren (AccessibleSelection *obj);

Accessible *
AccessibleSelection_getSelectedChild (AccessibleSelection *obj,
                                      long selectedChildIndex);

boolean
AccessibleSelection_selectChild (AccessibleSelection *obj,
                                 long childIndex);

boolean
AccessibleSelection_deselectSelectedChild (AccessibleSelection *obj,
                                           long selectedChildIndex);

boolean
AccessibleSelection_isChildSelected (AccessibleSelection *obj,
                                     long childIndex);

void
AccessibleSelection_selectAll (AccessibleSelection *obj);

void
AccessibleSelection_clearSelection (AccessibleSelection *obj);


/*
 *
 * AccessibleStateSet function prototypes
 *
 */

int
AccessibleStateSet_ref (AccessibleStateSet *obj);

int
AccessibleStateSet_unref (AccessibleStateSet *obj);

boolean
AccessibleStateSet_contains (AccessibleStateSet *obj,
                             ACCESSIBLE_STATE state);

void
AccessibleStateSet_add (AccessibleStateSet *obj,
                        ACCESSIBLE_STATE state);

void
AccessibleStateSet_remove (AccessibleStateSet *obj,
                           ACCESSIBLE_STATE state);

boolean
AccessibleStateSet_equals (AccessibleStateSet *obj,
                           AccessibleStateSet *obj2);

void
AccessibleStateSet_compare (AccessibleStateSet *obj,
                            AccessibleStateSet *obj2,
                            AccessibleStateSet **differenceSet);

boolean
AccessibleStateSet_isEmpty (AccessibleStateSet *obj);


/*
 *
 * AccessibleTable function prototypes
 *
 */

int
AccessibleTable_ref (AccessibleTable *obj);

int
AccessibleTable_unref (AccessibleTable *obj);

Accessible *
AccessibleTable_getCaption (AccessibleTable *obj);

Accessible *
AccessibleTable_getSummary (AccessibleTable *obj);

long
AccessibleTable_getNRows (AccessibleTable *obj);

long
AccessibleTable_getNColumns (AccessibleTable *obj);

Accessible *
AccessibleTable_refAt (AccessibleTable *obj,
                                 long row,
                                 long column);

long
AccessibleTable_getIndexAt (AccessibleTable *obj,
                            long row,
                            long column);

long
AccessibleTable_getRowAtIndex (AccessibleTable *obj,
                               long index);

long
AccessibleTable_getColumnAtIndex (AccessibleTable *obj,
                                  long index);

char *
AccessibleTable_getRowDescription (AccessibleTable *obj,
				   long row);

char *
AccessibleTable_getColumnDescription (AccessibleTable *obj,
				      long column);

long
AccessibleTable_getRowExtentAt (AccessibleTable *obj,
                                long row,
                                long column);

long
AccessibleTable_getColumnExtentAt (AccessibleTable *obj,
                                   long row,
                                   long column);

Accessible *
AccessibleTable_getRowHeader (AccessibleTable *obj,
			      long row);

Accessible *
AccessibleTable_getColumnHeader (AccessibleTable *obj,
				 long column);

long
AccessibleTable_getNSelectedRows (AccessibleTable *obj);

long
AccessibleTable_getSelectedRows (AccessibleTable *obj,
                                 long **selectedRows);

long
AccessibleTable_getNSelectedColumns (AccessibleTable *obj);

long
AccessibleTable_getSelectedColumns (AccessibleTable *obj,
                                    long **selectedColumns);

boolean
AccessibleTable_isRowSelected (AccessibleTable *obj,
                               long row);

boolean
AccessibleTable_isColumnSelected (AccessibleTable *obj,
                                  long column);

boolean
AccessibleTable_isSelected (AccessibleTable *obj,
                            long row,
                            long column);

/*
 *
 * AccessibleText function prototypes
 *
 */

int
AccessibleText_ref (AccessibleText *obj);

int
AccessibleText_unref (AccessibleText *obj);

long
AccessibleText_getCharacterCount (AccessibleText *obj);

char *
AccessibleText_getText (AccessibleText *obj,
                        long startOffset,
                        long endOffset);

long
AccessibleText_getCaretOffset (AccessibleText *obj);

char *
AccessibleText_getAttributes (AccessibleText *obj,
				 long offset,
				 long *startOffset,
				 long *endOfset);


boolean
AccessibleText_setCaretOffset (AccessibleText *obj,
                               long newOffset);

char *
AccessibleText_getTextBeforeOffset (AccessibleText *obj,
                                    long offset,
                                    TEXT_BOUNDARY_TYPE type,
				    long *startOffset, long *endOffset);

char *
AccessibleText_getTextAtOffset (AccessibleText *obj,
                                    long offset,
                                    TEXT_BOUNDARY_TYPE type,
				long *startOffset, long *endOffset);

char *
AccessibleText_getTextAfterOffset (AccessibleText *obj,
                                    long offset,
                                    TEXT_BOUNDARY_TYPE type,
				   long *startOffset, long *endOffset);
unsigned long
AccessibleText_getCharacterAtOffset (AccessibleText *obj,
                                     long offset);

void
AccessibleText_getCharacterExtents (AccessibleText *obj,
                                    long offset,
                                    long *x,
                                    long *y,
                                    long *width,
                                    long *height, AccessibleCoordType type);

long
AccessibleText_getOffsetAtPoint (AccessibleText *obj,
                                 long x,
                                 long y, AccessibleCoordType type);

long
AccessibleText_getNSelections (AccessibleText *obj);

void
AccessibleText_getSelection (AccessibleText *obj,
			     long selectionNum, long *startOffset,
			     long *endOffset);


boolean
AccessibleText_addSelection (AccessibleText *obj,
			     long startOffset, long endOffset);

boolean
AccessibleText_removeSelection (AccessibleText *obj,
				long selectionNum);

boolean
AccessibleText_setSelection (AccessibleText *obj,
			     long selectionNum,
			     long startOffset,
			     long endOffset);

/*
 *
 * AccessibleValue Function Prototypes:
 *
 */

float
AccessibleValue_getMinimumValue (AccessibleValue *value);

float
AccessibleValue_getCurrentValue (AccessibleValue *value);

float
AccessibleValue_getMaximumValue (AccessibleValue *value);

boolean
AccessibleValue_setCurrentValue (AccessibleValue *value,
                                 float newValue);

void
spi_freeString (char *s);

#endif
