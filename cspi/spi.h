
#ifndef _SPI_H
#define _SPI_H

/* Implementation private definitions */
#include "spi-impl.h"

/*
 * Definitions for SPI_ACCESSIBLE_STATE , SPI_ACCESSIBLE_ROLE, SpiAccessibleEvent,
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
  SPI_TEXT_BOUNDARY_CHAR,
  SPI_TEXT_BOUNDARY_CURSOR_POS,
  SPI_TEXT_BOUNDARY_WORD_START,
  SPI_TEXT_BOUNDARY_WORD_END,
  SPI_TEXT_BOUNDARY_SENTENCE_START,
  SPI_TEXT_BOUNDARY_SENTENCE_END,
  SPI_TEXT_BOUNDARY_LINE_START,
  SPI_TEXT_BOUNDARY_LINE_END,
  SPI_TEXT_BOUNDARY_ATTRIBUTE_RANGE
} SPI_TEXT_BOUNDARY_TYPE;



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
typedef enum _SpiAccessibleCoordType {
  COORD_TYPE_SCREEN,
  COORD_TYPE_WINDOW
} SpiAccessibleCoordType;

typedef enum _KeyEventType {
  KEY_PRESSED,
  KEY_RELEASED
} KeyEventType;

typedef enum _KeySynthType {
  KEY_PRESS,
  KEY_RELEASE, 
  KEY_PRESSRELEASE,
  KEY_SYM
} KeySynthType;

typedef enum _KeySpiListenerSyncType {
  KEYSPI_LISTENER_SYNCHRONOUS = 1,
  KEYSPI_LISTENER_CANCONSUME = 2,
  KEYSPI_LISTENER_ALLWINDOWS = 4
} KeySpiListenerSyncType;

typedef unsigned long KeyEventMask;

typedef struct _KeyStroke
{
	long keyID;
	short keycode;
	KeyEventType type;
	unsigned short modifiers;
} KeyStroke;

typedef struct _KeySet
{
	unsigned long *keysyms;
	unsigned short *keycodes;
	short len;
} KeySet;

#define ALL_KEYS ((void *)NULL)

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
 * Returns: the next #SpiAccessibleEvent in the SPI event queue.
 *
 **/
SpiAccessibleEvent *
SPI_nextEvent (boolean waitForEvent);

/**
 * SPI_exit:
 *
 * Disconnects from the Accessibility SpiRegistry and releases resources.
 * Not Yet Implemented.
 *
 **/
void
SPI_exit (void);

/*
 * Event SpiListener creation and support.
 */

/**
 * createEventListener:
 * @callback : an #SpiAccessibleEventListenerCB callback function, or NULL.
 *
 * Create a new #SpiAccessibleEventListener with a specified callback function.
 *
 * Returns: a pointer to a newly-created #SpiAccessibleEventListener.
 *
 **/
SpiAccessibleEventListener *
createEventListener (SpiAccessibleEventListenerCB callback);

/**
 * EventListener_addCallback:
 * @listener: the #SpiAccessibleEventListener instance to modify.
 * @callback: an #SpiAccessibleEventListenerCB function pointer.
 *
 * Add an in-process callback function to an existing SpiAccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_addCallback (SpiAccessibleEventListener *listener,
                           SpiAccessibleEventListenerCB callback);

/**
 * EventListener_removeCallback:
 * @listener: the #SpiAccessibleEventListener instance to modify.
 * @callback: an #SpiAccessibleEventListenerCB function pointer.
 *
 * Remove an in-process callback function from an existing SpiAccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
EventListener_removeCallback (SpiAccessibleEventListener *listener,
                              SpiAccessibleEventListenerCB callback);

/**
 * createKeySpiListener:
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
 * @listener: the #SpiAccessibleEventListener to be registered against an event type.
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
 * Add an in-process callback function to an existing SpiAccessibleEventListener.
 *
 * Returns: #TRUE if successful, otherwise #FALSE.
 *
 **/
boolean
registerGlobalEventListener (SpiAccessibleEventListener *listener,
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
 * Returns: a pointer to the 'i-th' virtual desktop's #SpiAccessible representation.
 *
 **/
SpiAccessible*
getDesktop (int n);

/**
 * getDesktopList:
 * @list: a pointer to an array of #SpiAccessible objects.
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
getDesktopList (SpiAccessible **list);

/**
 * registerKeystrokeListener:
 * @listener: a pointer to the #KeystrokeListener for which
 *            keystroke events are requested.
 *
 * Not Yet Implemented.
 *
 **/
void
registerKeystrokeListener (KeystrokeListener *listener,
			   KeySet *keys,
			   KeyMaskType modmask,
			   KeyEventMask eventmask,
			   KeySpiListenerSyncType sync_type);

/**
 * generateKeyEvent:
 * @keycode: a #long indicating the keycode of the key event
 *           being synthesized.
 * @synth_type: a #KeySynthType indicating whether this should be a
 *        KEY_PRESS, KEY_RELEASE, both (KEY_PRESSRELEASE), or
 *        a press/release pair for a KEYSYM.
 *
 * Synthesize a keyboard event (as if a hardware keyboard event occurred in the
 * current UI context).
 *
 **/
void
generateKeyEvent (long keyCode, KeySynthType synth_type);

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
 * SpiAccessible function prototypes
 *
 */

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
SpiAccessible_ref (SpiAccessible *obj);

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
SpiAccessible_unref (SpiAccessible *obj);

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
SpiAccessible_getName (SpiAccessible *obj);

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
SpiAccessible_getDescription (SpiAccessible *obj);

/**
 * SpiAccessible_getParent:
 * @obj: a pointer to the #SpiAccessible object to query.
 *
 * Get an #SpiAccessible object's parent container.
 *
 * Returns: a pointer to the #SpiAccessible object which contains the given
 *          #SpiAccessible instance, or NULL if the @obj has no parent container.
 *
 **/
SpiAccessible *
SpiAccessible_getParent (SpiAccessible *obj);

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
SpiAccessible_getChildCount (SpiAccessible *obj);

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
                            long childIndex);

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
SpiAccessible_getIndexInParent (SpiAccessible *obj);

/**
 * SpiAccessible_getRelationSet:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleRelation **
SpiAccessible_getRelationSet (SpiAccessible *obj);

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
SpiAccessible_getRole (SpiAccessible *obj);

/**
 * SpiAccessible_getStateSet:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleStateSet *
SpiAccessible_getStateSet (SpiAccessible *obj);

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
SpiAccessible_isSpiAction (SpiAccessible *obj);

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
SpiAccessible_isSpiComponent (SpiAccessible *obj);

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
SpiAccessible_isSpiEditableText (SpiAccessible *obj);

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
SpiAccessible_isSpiHypertext (SpiAccessible *obj);

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
SpiAccessible_isSpiImage (SpiAccessible *obj);

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
SpiAccessible_isSpiSelection (SpiAccessible *obj);

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
SpiAccessible_isSpiTable (SpiAccessible *obj);

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
SpiAccessible_isSpiText (SpiAccessible *obj);

/**
 * SpiAccessible_getAction:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleAction *
SpiAccessible_getAction (SpiAccessible *obj);

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
SpiAccessible_getComponent (SpiAccessible *obj);

/**
 * SpiAccessible_getEditableText:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleEditableText *
SpiAccessible_getEditableText (SpiAccessible *obj);

/**
 * SpiAccessible_getHypertext:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleHypertext *
SpiAccessible_getHypertext (SpiAccessible *obj);

/**
 * SpiAccessible_getImage:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleImage *
SpiAccessible_getImage (SpiAccessible *obj);

/**
 * SpiAccessible_getSelection:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleSelection *
SpiAccessible_getSelection (SpiAccessible *obj);

/**
 * SpiAccessible_getTable:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleTable *
SpiAccessible_getTable (SpiAccessible *obj);

/**
 * SpiAccessible_getText:
 *
 * Not Yet Implemented.
 *
 **/
SpiAccessibleText *
SpiAccessible_getText (SpiAccessible *obj);

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
SpiAccessible_queryInterface (SpiAccessible *obj, char *interface_name);

/*
 *
 * SpiAccessibleAction function prototypes
 *
 */

int
SpiAccessibleAction_ref (
                      SpiAccessibleAction *obj);

int
SpiAccessibleAction_unref (SpiAccessibleAction *obj);

long
SpiAccessibleAction_getNSpiActions (SpiAccessibleAction *obj);

/**
 * SpiAccessibleAction_getDescription:
 * @obj: a pointer to the #SpiAccessibleAction to query.
 *
 * Get the description of 'i-th' action invokable on an
 *      object implementing #SpiAccessibleAction.
 *
 * Not Yet Implemented.
 *
 * Returns: a UTF-8 string describing the 'i-th' invokable action.
 *
 **/
char *
SpiAccessibleAction_getDescription (SpiAccessibleAction *obj,
                                 long index);

boolean
SpiAccessibleAction_doSpiAction (SpiAccessibleAction *obj,
                           long index);

char *
SpiAccessibleAction_getKeyBinding (SpiAccessibleAction *obj,
                                long index);

/*
 *
 * SpiAccessibleApplication function prototypes
 *
 */

/**
 * SpiAccessibleApplication_unref:
 * @obj: a pointer to the #SpiAccessibleApplication on which to operate.
 *
 * Decrement the reference count for an #SpiAccessibleApplication.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessibleApplication_ref (SpiAccessibleApplication *obj);

/**
 * SpiAccessibleApplication_unref:
 * @obj: a pointer to the #SpiAccessibleApplication object on which to operate.
 *
 * Decrement the reference count for an #SpiAccessibleApplication.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessibleApplication_unref (SpiAccessibleApplication *obj);

/**
 * SpiAccessibleApplication_getToolkitName:
 * @obj: a pointer to the #SpiAccessibleApplication to query.
 *
 * Get the name of the UI toolkit used by an #SpiAccessibleApplication.
 *
 * Returns: a UTF-8 string indicating which UI toolkit is
 *          used by an application.
 *
 **/
char *
SpiAccessibleApplication_getToolkitName (SpiAccessibleApplication *obj);

/**
 * SpiAccessibleApplication_getVersion:
 * @obj: a pointer to the #SpiAccessibleApplication being queried.
 *
 * Get the version of the at-spi bridge exported by an
 *      #SpiAccessibleApplication instance.
 *
 * Returns: a UTF-8 string indicating the application's
 *          at-spi version.
 *
 **/
char *
SpiAccessibleApplication_getVersion (SpiAccessibleApplication *obj);

/**
 * SpiAccessibleApplication_getID:
 * @obj: a pointer to the #SpiAccessibleApplication being queried.
 *
 * Get the unique ID assigned by the SpiRegistry to an
 *      #SpiAccessibleApplication instance.
 * (Not Yet Implemented by the registry).
 *
 * Returns: a unique #long integer associated with the application
 *          by the SpiRegistry, or 0 if the application is not registered.
 **/
long
SpiAccessibleApplication_getID (SpiAccessibleApplication *obj);

/**
 * SpiAccessibleApplication_pause:
 *
 * Attempt to pause the application (used when client event queue is
 *  over-full).
 * Not Yet Implemented.
 *
 * Returns: #TRUE if the application was paused successfully, #FALSE otherwise.
 *
 **/
boolean
SpiAccessibleApplication_pause (SpiAccessibleApplication *obj);

/**
 * SpiAccessibleApplication_pause:
 *
 * Attempt to resume the application (used after #SpiAccessibleApplication_pause).
 * Not Yet Implemented.
 *
 * Returns: #TRUE if application processing resumed successfully, #FALSE otherwise.
 *
 **/
boolean
SpiAccessibleApplication_resume (SpiAccessibleApplication *obj);

/*
 *
 * SpiAccessibleComponent function prototypes
 *
 */

int
SpiAccessibleComponent_ref (SpiAccessibleComponent *obj);

int
SpiAccessibleComponent_unref (SpiAccessibleComponent *obj);

boolean
SpiAccessibleComponent_contains (SpiAccessibleComponent *obj,
                              long x,
                              long y,
                              SpiAccessibleCoordType ctype);

SpiAccessible *
SpiAccessibleComponent_getAccessibleAtPoint (SpiAccessibleComponent *obj,
                                          long x,
                                          long y,
                                          SpiAccessibleCoordType ctype);

/**
 * SpiAccessibleComponent_getExtents:
 * @obj: a pointer to the #SpiAccessibleComponent to query.
 * @x: a pointer to a #long into which the minimum x coordinate will be returned.
 * @y: a pointer to a #long into which the minimum y coordinate will be returned.
 * @width: a pointer to a #long into which the x extents (width) will be returned.
 * @height: a pointer to a #long into which the y extents (height) will be returned.
 * @ctype: the desired coordinate system into which to return the results,
 *         (e.g. COORD_TYPE_WINDOW, COORD_TYPE_SCREEN).
 *
 * Get the bounding box of the specified #SpiAccessibleComponent.
 *
 **/
void
SpiAccessibleComponent_getExtents (SpiAccessibleComponent *obj,
                                long *x,
                                long *y,
                                long *width,
                                long *height,
                                SpiAccessibleCoordType ctype);

void
SpiAccessibleComponent_getPosition (SpiAccessibleComponent *obj,
                                 long *x,
                                 long *y,
                                 SpiAccessibleCoordType ctype);

void
SpiAccessibleComponent_getSize (SpiAccessibleComponent *obj,
                             long *width,
                             long *height);

void
SpiAccessibleComponent_grabFocus (SpiAccessibleComponent *obj);

/*
 *
 * SpiAccessibleEditableText function prototypes
 *
 */

int
SpiAccessibleEditableText_ref (SpiAccessibleEditableText *obj);

int
SpiAccessibleEditableText_unref (SpiAccessibleEditableText *obj);

boolean
SpiAccessibleEditableText_setRunAttributes (SpiAccessibleEditableText *obj,
					 const char *attributes,
					 long startPos, long endPos);

void
SpiAccessibleEditableText_setTextContents (SpiAccessibleEditableText *obj,
                                        const char *newContents);

void
SpiAccessibleEditableText_insertText (SpiAccessibleEditableText *obj,
                                   long position,
                                   char *text,
                                   long length);

void
SpiAccessibleEditableText_copySpiText (SpiAccessibleText *obj,
                                 long startPos,
                                 long endPos);

void
SpiAccessibleEditableText_cutText (SpiAccessibleEditableText *obj,
                                long startPos,
                                long endPos);

void
SpiAccessibleEditableText_deleteText (SpiAccessibleEditableText *obj,
                                   long startPos,
                                   long endPos);

void
SpiAccessibleEditableText_pasteText (SpiAccessibleEditableText *obj,
                                  long position);

/*
 *
 * SpiAccessibleHyperlink function prototypes
 *
 */

int
SpiAccessibleHyperlink_ref (SpiAccessibleHyperlink *obj);

int
SpiAccessibleHyperlink_unref (SpiAccessibleHyperlink *obj);

long
SpiAccessibleHyperlink_getNAnchors (SpiAccessibleHyperlink *obj);

char *
SpiAccessibleHyperlink_getURI (SpiAccessibleHyperlink *obj,
                            long i);

SpiAccessible
SpiAccessibleHyperlink_getObject (SpiAccessibleHyperlink *obj,
                               long i);

void
SpiAccessibleHyperlink_getIndexRange (SpiAccessibleHyperlink *obj,
                                   long *startIndex,
                                   long *endIndex);

boolean
SpiAccessibleHyperlink_isValid (SpiAccessibleHyperlink *obj);

/*
 *
 * SpiAccessibleHypertext function prototypes
 *
 */

int
SpiAccessibleHyperspi_text_ref (SpiAccessibleHypertext *obj);

int
SpiAccessibleHyperspi_text_unref (SpiAccessibleHypertext *obj);

long
SpiAccessibleHyperspi_text_getNLinks (SpiAccessibleHypertext *obj);

SpiAccessibleHyperlink *
SpiAccessibleHyperSpiText_getLink (SpiAccessibleHypertext *obj,
                             long linkIndex);

long
SpiAccessibleHyperspi_text_getLinkIndex (SpiAccessibleHypertext *obj,
                                  long characterIndex);

/*
 *
 * SpiAccessibleImage function prototypes
 *
 */

int
SpiAccessibleImage_ref (SpiAccessibleImage *obj);

int
SpiAccessibleImage_unref (SpiAccessibleImage *obj);

char *
SpiAccessibleImage_getImageDescription (SpiAccessibleImage *obj);

void
SpiAccessibleImage_getImageSize (SpiAccessibleImage *obj,
                              long *width,
                              long *height);

void
SpiAccessibleImage_getImagePosition (SpiAccessibleImage *obj,
                                  long *x,
                                  long *y,
                                  SpiAccessibleCoordType ctype);

/*
 *
 * SpiAccessibleRelation function prototypes
 *
 */

int
SpiAccessibleRelation_ref (SpiAccessibleRelation *obj);

int
SpiAccessibleRelation_unref (SpiAccessibleRelation *obj);

RELATION_TYPE
SpiAccessibleRelation_getRelationType (SpiAccessibleRelation *obj);

SpiAccessible *
SpiAccessibleRelation_getTarget (SpiAccessibleRelation *obj);



/*
 *
 * SpiAccessibleSelection function prototypes
 *
 */

int
SpiAccessibleSelection_ref (SpiAccessibleSelection *obj);

int
SpiAccessibleSelection_unref (SpiAccessibleSelection *obj);

long
SpiAccessibleSelection_getNSelectedChildren (SpiAccessibleSelection *obj);

SpiAccessible *
SpiAccessibleSelection_getSelectedChild (SpiAccessibleSelection *obj,
                                      long selectedChildIndex);

boolean
SpiAccessibleSelection_selectChild (SpiAccessibleSelection *obj,
                                 long childIndex);

boolean
SpiAccessibleSelection_deselectSelectedChild (SpiAccessibleSelection *obj,
                                           long selectedChildIndex);

boolean
SpiAccessibleSelection_isChildSelected (SpiAccessibleSelection *obj,
                                     long childIndex);

void
SpiAccessibleSelection_selectAll (SpiAccessibleSelection *obj);

void
SpiAccessibleSelection_clearSpiSelection (SpiAccessibleSelection *obj);


/*
 *
 * SpiAccessibleStateSet function prototypes
 *
 */

int
SpiAccessibleStateSet_ref (SpiAccessibleStateSet *obj);

int
SpiAccessibleStateSet_unref (SpiAccessibleStateSet *obj);

boolean
SpiAccessibleStateSet_contains (SpiAccessibleStateSet *obj,
                             SPI_ACCESSIBLE_STATE state);

void
SpiAccessibleStateSet_add (SpiAccessibleStateSet *obj,
                        SPI_ACCESSIBLE_STATE state);

void
SpiAccessibleStateSet_remove (SpiAccessibleStateSet *obj,
                           SPI_ACCESSIBLE_STATE state);

boolean
SpiAccessibleStateSet_equals (SpiAccessibleStateSet *obj,
                           SpiAccessibleStateSet *obj2);

void
SpiAccessibleStateSet_compare (SpiAccessibleStateSet *obj,
                            SpiAccessibleStateSet *obj2,
                            SpiAccessibleStateSet **differenceSet);

boolean
SpiAccessibleStateSet_isEmpty (SpiAccessibleStateSet *obj);


/*
 *
 * SpiAccessibleTable function prototypes
 *
 */

int
SpiAccessibleTable_ref (SpiAccessibleTable *obj);

int
SpiAccessibleTable_unref (SpiAccessibleTable *obj);

SpiAccessible *
SpiAccessibleTable_getCaption (SpiAccessibleTable *obj);

SpiAccessible *
SpiAccessibleTable_getSummary (SpiAccessibleTable *obj);

long
SpiAccessibleTable_getNRows (SpiAccessibleTable *obj);

long
SpiAccessibleTable_getNColumns (SpiAccessibleTable *obj);

SpiAccessible *
SpiAccessibleTable_refAt (SpiAccessibleTable *obj,
                                 long row,
                                 long column);

long
SpiAccessibleTable_getIndexAt (SpiAccessibleTable *obj,
                            long row,
                            long column);

long
SpiAccessibleTable_getRowAtIndex (SpiAccessibleTable *obj,
                               long index);

long
SpiAccessibleTable_getColumnAtIndex (SpiAccessibleTable *obj,
                                  long index);

char *
SpiAccessibleTable_getRowDescription (SpiAccessibleTable *obj,
				   long row);

char *
SpiAccessibleTable_getColumnDescription (SpiAccessibleTable *obj,
				      long column);

long
SpiAccessibleTable_getRowExtentAt (SpiAccessibleTable *obj,
                                long row,
                                long column);

long
SpiAccessibleTable_getColumnExtentAt (SpiAccessibleTable *obj,
                                   long row,
                                   long column);

SpiAccessible *
SpiAccessibleTable_getRowHeader (SpiAccessibleTable *obj,
			      long row);

SpiAccessible *
SpiAccessibleTable_getColumnHeader (SpiAccessibleTable *obj,
				 long column);

long
SpiAccessibleTable_getNSelectedRows (SpiAccessibleTable *obj);

long
SpiAccessibleTable_getSelectedRows (SpiAccessibleTable *obj,
                                 long **selectedRows);

long
SpiAccessibleTable_getNSelectedColumns (SpiAccessibleTable *obj);

long
SpiAccessibleTable_getSelectedColumns (SpiAccessibleTable *obj,
                                    long **selectedColumns);

boolean
SpiAccessibleTable_isRowSelected (SpiAccessibleTable *obj,
                               long row);

boolean
SpiAccessibleTable_isColumnSelected (SpiAccessibleTable *obj,
                                  long column);

boolean
SpiAccessibleTable_isSelected (SpiAccessibleTable *obj,
                            long row,
                            long column);

/*
 *
 * SpiAccessibleText function prototypes
 *
 */

int
SpiAccessibleText_ref (SpiAccessibleText *obj);

int
SpiAccessibleText_unref (SpiAccessibleText *obj);

long
SpiAccessibleText_getCharacterCount (SpiAccessibleText *obj);

char *
SpiAccessibleText_getText (SpiAccessibleText *obj,
                        long startOffset,
                        long endOffset);

long
SpiAccessibleText_getCaretOffset (SpiAccessibleText *obj);

char *
SpiAccessibleText_getAttributes (SpiAccessibleText *obj,
				 long offset,
				 long *startOffset,
				 long *endOfset);


boolean
SpiAccessibleText_setCaretOffset (SpiAccessibleText *obj,
                               long newOffset);

char *
SpiAccessibleText_getTextBeforeOffset (SpiAccessibleText *obj,
                                    long offset,
                                    SPI_TEXT_BOUNDARY_TYPE type,
				    long *startOffset, long *endOffset);

char *
SpiAccessibleText_getTextAtOffset (SpiAccessibleText *obj,
                                    long offset,
                                    SPI_TEXT_BOUNDARY_TYPE type,
				long *startOffset, long *endOffset);

char *
SpiAccessibleText_getTextAfterOffset (SpiAccessibleText *obj,
                                    long offset,
                                    SPI_TEXT_BOUNDARY_TYPE type,
				   long *startOffset, long *endOffset);
unsigned long
SpiAccessibleText_getCharacterAtOffset (SpiAccessibleText *obj,
                                     long offset);

void
SpiAccessibleText_getCharacterExtents (SpiAccessibleText *obj,
                                    long offset,
                                    long *x,
                                    long *y,
                                    long *width,
                                    long *height, SpiAccessibleCoordType type);

long
SpiAccessibleText_getOffsetAtPoint (SpiAccessibleText *obj,
                                 long x,
                                 long y, SpiAccessibleCoordType type);

long
SpiAccessibleText_getNSpiSelections (SpiAccessibleText *obj);

void
SpiAccessibleText_getSelection (SpiAccessibleText *obj,
			     long selectionNum, long *startOffset,
			     long *endOffset);


boolean
SpiAccessibleText_addSpiSelection (SpiAccessibleText *obj,
			     long startOffset, long endOffset);

boolean
SpiAccessibleText_removeSelection (SpiAccessibleText *obj,
				long selectionNum);

boolean
SpiAccessibleText_setSelection (SpiAccessibleText *obj,
			     long selectionNum,
			     long startOffset,
			     long endOffset);

/*
 *
 * SpiAccessibleValue Function Prototypes:
 *
 */

float
SpiAccessibleValue_getMinimumSpiValue (SpiAccessibleValue *value);

float
SpiAccessibleValue_getCurrentValue (SpiAccessibleValue *value);

float
SpiAccessibleValue_getMaximumSpiValue (SpiAccessibleValue *value);

boolean
SpiAccessibleValue_setCurrentValue (SpiAccessibleValue *value,
                                 float newSpiValue);

void
spi_freeString (char *s);

#endif
