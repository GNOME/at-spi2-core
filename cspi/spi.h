
#ifndef _SPI_H
#define _SPI_H

#include "spi-impl.h"

/* definitions for ACCESSIBLE_STATE */
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



typedef enum _AccessibleCoordType {
  COORD_TYPE_WINDOW,
  COORD_TYPE_SCREEN
} AccessibleCoordType;


/*
 *
 * Basic SPI initialization and event loop function prototypes
 *
 */

int
SPI_init (void);

void
SPI_event_main (boolean isGNOMEApp);

boolean
SPI_eventIsReady ();

AccessibleEvent *
SPI_nextEvent (boolean waitForEvent);

void
SPI_exit (void);

/*
 * Event Listener creation and support.
 */

AccessibleEventListener *
CreateEventListener (AccessibleEventListenerCB callback);

boolean
EventListener_addCallback (AccessibleEventListener *listener,
                           AccessibleEventListenerCB callback);

boolean
EventListener_removeCallback (AccessibleEventListener *listener,
                              AccessibleEventListenerCB callback);

/*
 *
 * Global functions serviced by the registry
 *
 */

boolean
RegisterGlobalEventListener (AccessibleEventListener *listener,
                             char *eventType);

int
GetDesktopCount ();

Accessible
*getDesktop (int n);

int
getDesktopList (Accessible **list);

void
registerKeystrokeListener (KeystrokeListener *listener);

void
generateKeyEvent (long keyCode, long meta);

void
generateMouseEvent (long x, long y, char *name);

/*
 *
 * Accessible function prototypes
 *
 */

int
Accessible_ref (Accessible *obj);

int
Accessible_unref (Accessible *obj);

char *
Accessible_getName (Accessible *obj);

char *
Accessible_getDescription (Accessible *obj);

Accessible *
Accessible_getParent (Accessible *obj);

long
Accessible_getChildCount (Accessible *obj);

Accessible *
Accessible_getChildAtIndex (Accessible *obj,
                            long childIndex);

long
Accessible_getIndexInParent (Accessible *obj);

AccessibleRelation **
Accessible_getRelationSet (Accessible *obj);

char *
Accessible_getRole (Accessible *obj);

AccessibleStateSet *
Accessible_getStateSet (Accessible *obj);


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

char *
AccessibleAction_getDescription (AccessibleAction *obj,
                                 long index);

void
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

int
AccessibleApplication_ref (AccessibleApplication *obj);

int
AccessibleApplication_unref (AccessibleApplication *obj);

char *
AccessibleApplication_getToolkitName (AccessibleApplication *obj);

char *
AccessibleApplication_getVersion (AccessibleApplication *obj);

long
AccessibleApplication_getID (AccessibleApplication *obj);

boolean
AccessibleApplication_pause (AccessibleApplication *obj);

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

void
AccessibleEditableText_setTextContents (AccessibleEditableText *obj,
                                        char *newContents);

void
AccessibleEditableText_insertText (AccessibleEditableText *obj,
                                   long position,
                                   char *text,
                                   long length);

boolean
AccessibleEditableText_selectText (AccessibleEditableText *obj,
                                   long startPos,
                                   long endPos);

boolean
AccessibleEditableText_setAttributes (AccessibleEditableText *obj,
                                      long startPos,
                                      long endPos,
                                      char *attributes);

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
AccessibleHyperlink_getAnchor (AccessibleHyperlink *obj,
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
                              long *height,
                              AccessibleCoordType ctype);
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
AccessibleSelwection_getNSelectedChildren (AccessibleSelection *obj);

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
AccessibleTable_getAccessibleAt (AccessibleTable *obj,
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
AccessibleTable_getRowDescription (AccessibleTable *obj);

char *
AccessibleTable_getColumnDescription (AccessibleTable *obj);

long
AccessibleTable_getRowExtentAt (AccessibleTable *obj,
                                long row,
                                long column);

long
AccessibleTable_getColumnExtentAt (AccessibleTable *obj,
                                   long row,
                                   long column);

AccessibleTable *
AccessibleTable_getRowHeaders (AccessibleTable *obj);

AccessibleTable *
AccessibleTable_getColumnHeaders (AccessibleTable *obj);

long
AccessibleTable_getNSelectedRows (AccessibleTable *obj);

void
AccessibleTable_getSelectedRows (AccessibleTable *obj,
                                 long **selectedRows);

long
AccessibleTable_getNSelectedColumns (AccessibleTable *obj);

void
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

boolean
AccessibleText_setCaretOffset (AccessibleText *obj,
                               long newOffset);

char *
AccessibleText_getTextBeforeOffset (AccessibleText *obj,
                                    long offset,
                                    TEXT_BOUNDARY_TYPE type);

char *
AccessibleText_getTextAtOffset (Accessible *obj,
                                long offset,
                                TEXT_BOUNDARY_TYPE type);

char *
AccessibleText_getTextAfterOffset (AccessibleText *obj,
                                   long offset,
                                   TEXT_BOUNDARY_TYPE type);

char
AccessibleText_getCharacterAtOffset (AccessibleText *obj,
                                     long offset);

char *
AccessibleText_getAttributes (AccessibleText *obj,
                              long startOffset,
                              long endOffset);

boolean
AccessibleText_getCharacterExtents (AccessibleText *obj,
                                    long offset,
                                    long *x,
                                    long *y,
                                    long *width,
                                    long *height);

long
AccessibleText_getOffsetAtPoint (AccessibleText *obj,
                                 long x,
                                 long y);

char *
AccessibleText_getSelectedText (AccessibleText *obj);

void
AccessibleText_getSelectionBounds (AccessibleText *obj,
                                   long *startOffset,
                                   long *endOffset);

boolean
AccessibleText_setSelectionBounds (AccessibleText *obj,
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

#endif
