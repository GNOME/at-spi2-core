/* Generated from , version 1.24.0



 !\mainpage AT-SPI Interfaces and Subinterfaces

  This is the main documentation page for the 
  Assistive Technology Service Provider Interface (AT-SPI). 
  
  \section apps Applications and Interface Components
  Namespace Accessibility includes service APIs implemented by
  participating applications and their user interface components:\n\n
  Accessibility::Accessible\n 
  Accessibility::Application\n
  Accessibility::Desktop\n
  Accessibility::Collecgtion\n
  Accessibility::Component\n
  Accessibility::Hypertext\n
  Accessibility::Image\n
  Accessibility::Selection\n
  Accessibility::Table\n
  Accessibility::Text\n
  Accessibility::EditableText\n
  Accessibility::Value
 
  \section types Enumerated Types
  Accessibility defines a number of key enumerated types, including:\n\n
  Accessibility::RelationType\n
  Accessibility::Role\n
  Accessibility::StateType\n
  Accessibility::Event\n
  Accessibility::EventDetails \n
 
  \section Registry
  Accessibility also includes Accessibility::Registry,
  which is the service used by assistive technologies and related
  AT-SPI clients to register interest in certain classes of events,
  enumerate the currently available desktop and application list,
  and to synthesize certain kinds of device events.
 
  \section listeners Event Listener Interfaces
  Accessibility::EventListener\n
  Accessibility::DeviceEventListener
 
  \section helpers Helper Interfaces
 
  The following interfaces may be implemented by assistive technologies 
  themselves, in order to export their services in a consistent manner or
  in order to interoperate with other applications or desktop services.\n
 
  Accessibility::LoginHelper : Implemented by adaptive technologies which 
  need to participate in user-authentication or login activities, and which
  therefore may need negotiation with authentication agents or processes.\n
 
  Accessibility::Selector [NEW]: Implemented by user agents or assistive 
  technologies which export lists of choices from which the end-user is 
  expected to make selections.  Useful for various types of remote
  activation or intercommunication between multiple ATs.

 */

#ifndef _ACCESSIBILITY_H_
#define _ACCESSIBILITY_H_
#ifdef __cplusplus
extern "C" {
#endif

/**
 *
Accessibility_LOCALE_TYPE:
 * @Accessibility_LOCALE_TYPE_MESSAGES: <![CDATA[]]>
 * @Accessibility_LOCALE_TYPE_COLLATE: <![CDATA[]]>
 * @Accessibility_LOCALE_TYPE_CTYPE: <![CDATA[]]>
 * @Accessibility_LOCALE_TYPE_MONETARY: <![CDATA[]]>
 * @Accessibility_LOCALE_TYPE_NUMERIC: <![CDATA[]]>
 * @Accessibility_LOCALE_TYPE_TIME: <![CDATA[]]>
 *
 * <![CDATA[   Used by Text and Document interfaces these correspond to the POSIX 'setlocale' enum values.    ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_LOCALE_TYPE_MESSAGES,
    Accessibility_LOCALE_TYPE_COLLATE,
    Accessibility_LOCALE_TYPE_CTYPE,
    Accessibility_LOCALE_TYPE_MONETARY,
    Accessibility_LOCALE_TYPE_NUMERIC,
    Accessibility_LOCALE_TYPE_TIME,
} Accessibility_LOCALE_TYPE;

/**
 * NUM_ACCESSIBILITY_LOCALE_TYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_LOCALE_TYPE.
 */
#define NUM_ACCESSIBILITY_LOCALE_TYPES (5+1)

/**
 *
Accessibility_COORD_TYPE:
 * @Accessibility_COORD_TYPE_SCREEN: <![CDATA[]]>
 * @Accessibility_COORD_TYPE_WINDOW: <![CDATA[]]>
 *
 * <![CDATA[   Used by Component, Image, and Text interfaces to specify whether coordinates are relative to the window or the screen.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_COORD_TYPE_SCREEN,
    Accessibility_COORD_TYPE_WINDOW,
} Accessibility_COORD_TYPE;

/**
 * NUM_ACCESSIBILITY_COORD_TYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_COORD_TYPE.
 */
#define NUM_ACCESSIBILITY_COORD_TYPES (1+1)

/**
 *
Accessibility_Collection_SortOrder:
 * @Accessibility_Collection_SORT_ORDER_INVALID: <![CDATA[]]>
 * @Accessibility_Collection_SORT_ORDER_CANONICAL: <![CDATA[]]>
 * @Accessibility_Collection_SORT_ORDER_FLOW: <![CDATA[]]>
 * @Accessibility_Collection_SORT_ORDER_TAB: <![CDATA[]]>
 * @Accessibility_Collection_SORT_ORDER_REVERSE_CANONICAL: <![CDATA[]]>
 * @Accessibility_Collection_SORT_ORDER_REVERSE_FLOW: <![CDATA[]]>
 * @Accessibility_Collection_SORT_ORDER_REVERSE_TAB: <![CDATA[]]>
 * @Accessibility_Collection_SORT_ORDER_LAST_DEFINED: <![CDATA[]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_Collection_SORT_ORDER_INVALID,
    Accessibility_Collection_SORT_ORDER_CANONICAL,
    Accessibility_Collection_SORT_ORDER_FLOW,
    Accessibility_Collection_SORT_ORDER_TAB,
    Accessibility_Collection_SORT_ORDER_REVERSE_CANONICAL,
    Accessibility_Collection_SORT_ORDER_REVERSE_FLOW,
    Accessibility_Collection_SORT_ORDER_REVERSE_TAB,
    Accessibility_Collection_SORT_ORDER_LAST_DEFINED,
} Accessibility_Collection_SortOrder;

/**
 * NUM_ACCESSIBILITY_SORTORDERS:
 *
 * 1 higher than the highest valid value of #Accessibility_Collection_SortOrder.
 */
#define NUM_ACCESSIBILITY_SORTORDERS (7+1)

/**
 *
Accessibility_Collection_MatchType:
 * @Accessibility_Collection_MATCH_INVALID: <![CDATA[]]>
 * @Accessibility_Collection_MATCH_ALL: <![CDATA[]]>
 * @Accessibility_Collection_MATCH_ANY: <![CDATA[]]>
 * @Accessibility_Collection_MATCH_NONE: <![CDATA[]]>
 * @Accessibility_Collection_MATCH_EMPTY: <![CDATA[]]>
 * @Accessibility_Collection_MATCH_LAST_DEFINED: <![CDATA[]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_Collection_MATCH_INVALID,
    Accessibility_Collection_MATCH_ALL,
    Accessibility_Collection_MATCH_ANY,
    Accessibility_Collection_MATCH_NONE,
    Accessibility_Collection_MATCH_EMPTY,
    Accessibility_Collection_MATCH_LAST_DEFINED,
} Accessibility_Collection_MatchType;

/**
 * NUM_ACCESSIBILITY_MATCHTYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_Collection_MatchType.
 */
#define NUM_ACCESSIBILITY_MATCHTYPES (5+1)

/**
 *
Accessibility_Collection_TreeTraversalType:
 * @Accessibility_Collection_TREE_RESTRICT_CHILDREN: <![CDATA[]]>
 * @Accessibility_Collection_TREE_RESTRICT_SIBLING: <![CDATA[]]>
 * @Accessibility_Collection_TREE_INORDER: <![CDATA[]]>
 * @Accessibility_Collection_TREE_LAST_DEFINED: <![CDATA[]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_Collection_TREE_RESTRICT_CHILDREN,
    Accessibility_Collection_TREE_RESTRICT_SIBLING,
    Accessibility_Collection_TREE_INORDER,
    Accessibility_Collection_TREE_LAST_DEFINED,
} Accessibility_Collection_TreeTraversalType;

/**
 * NUM_ACCESSIBILITY_TREETRAVERSALTYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_Collection_TreeTraversalType.
 */
#define NUM_ACCESSIBILITY_TREETRAVERSALTYPES (3+1)

/**
 *
Accessibility_ComponentLayer:
 * @Accessibility_LAYER_INVALID: <![CDATA[     Indicates an error condition or uninitialized value.    ]]>
 * @Accessibility_LAYER_BACKGROUND: <![CDATA[     The bottom-most layer, over which everything else is painted.        The 'desktop background' is generally in this layer.    ]]>
 * @Accessibility_LAYER_CANVAS: <![CDATA[     The 'background' layer for most content renderers and UI Component        containers.    ]]>
 * @Accessibility_LAYER_WIDGET: <![CDATA[     The layer in which the majority of ordinary 'foreground' widgets reside.   ]]>
 * @Accessibility_LAYER_MDI: <![CDATA[     A special layer between LAYER_CANVAS and LAYER_WIDGET, in which the       'pseudo windows' (e.g. the MDI frames) reside.        (See Component.GetMDIZOrder)    ]]>
 * @Accessibility_LAYER_POPUP: <![CDATA[     A layer for popup window content, above LAYER_WIDGET.    ]]>
 * @Accessibility_LAYER_OVERLAY: <![CDATA[     The topmost layer.    ]]>
 * @Accessibility_LAYER_WINDOW: <![CDATA[     The layer in which a toplevel window background usually resides.    ]]>
 * @Accessibility_LAYER_LAST_DEFINED: <![CDATA[     Used only to determine the end of the enumeration.    ]]>
 *
 * <![CDATA[     The ComponentLayer of a Component instance indicates its relative stacking order       with respect to the onscreen visual representation of the UI.       ComponentLayer, in combination with Component bounds information, can be used       to compute the visibility of all or part of a component.  This is important in       programmatic determination of region-of-interest for magnification, and in       &#168;flat screen review&#168; models of the screen, as well as for other uses.       Objects residing in two of the ComponentLayer categories support       further z-ordering information, with respect to their peers in the same layer:       namely, LAYER_WINDOW and LAYER_MDI.  Relative stacking order for other objects within       the same layer is not available; the recommended heuristic is &#168;first child paints first&#168;,        in other words, assume that the first siblings in the child list are subject to being       overpainted by later siblings if their bounds intersect.        The order of layers, from bottom to top, is: 	     	    	LAYER_BACKGROUND 	    	LAYER_WINDOW 	    	LAYER_MDI 	    	LAYER_CANVAS 	    	LAYER_WIDGET 	    	LAYER_POPUP 	    	LAYER_OVERLAY 	            ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_LAYER_INVALID,
    Accessibility_LAYER_BACKGROUND,
    Accessibility_LAYER_CANVAS,
    Accessibility_LAYER_WIDGET,
    Accessibility_LAYER_MDI,
    Accessibility_LAYER_POPUP,
    Accessibility_LAYER_OVERLAY,
    Accessibility_LAYER_WINDOW,
    Accessibility_LAYER_LAST_DEFINED,
} Accessibility_ComponentLayer;

/**
 * NUM_ACCESSIBILITY_COMPONENTLAYERS:
 *
 * 1 higher than the highest valid value of #Accessibility_ComponentLayer.
 */
#define NUM_ACCESSIBILITY_COMPONENTLAYERS (8+1)

/**
 *
Accessibility_TEXT_BOUNDARY_TYPE:
 * @Accessibility_TEXT_BOUNDARY_CHAR: <![CDATA[     Text is bounded by this character only.        Start and end offsets differ by one, by definition, for this value.    ]]>
 * @Accessibility_TEXT_BOUNDARY_WORD_START: <![CDATA[     Boundary condition is start of a word; i.e. range is from start of       one word to the start of another word.    ]]>
 * @Accessibility_TEXT_BOUNDARY_WORD_END: <![CDATA[     Boundary condition is the end of a word; i.e. range is from  	    the end of one word to the end of another.     Some locales may not distinguish between words and       characters or glyphs, in particular those locales which use       wholly or partially ideographic character sets.  In these cases,       characters may be returned in lieu of multi-character substrings.   ]]>
 * @Accessibility_TEXT_BOUNDARY_SENTENCE_START: <![CDATA[     Boundary condition is start of a sentence, as determined  	    by the application.       Some locales or character sets may not include explicit sentence       delimiters, so this boundary type can not always be honored.       Some locales will return lines of text instead of grammatical sentences.   ]]>
 * @Accessibility_TEXT_BOUNDARY_SENTENCE_END: <![CDATA[     Boundary condition is end of a sentence, as determined by the application,  	    including the sentence-delimiting character, for instance '.'       Some locales or character sets may not include explicit sentence       delimiters, so this boundary type can not always be honored.       Some locales will return lines of text instead of grammatical sentences.   ]]>
 * @Accessibility_TEXT_BOUNDARY_LINE_START: <![CDATA[      Boundary condition is the start of a line; i.e. range is        from start of one line to the start of another.  This generally        means that an end-of-line character will appear at the end of the range.    ]]>
 * @Accessibility_TEXT_BOUNDARY_LINE_END: <![CDATA[      Boundary condition is the end of a line; i.e. range is       from start of one line to the start of another.  This generally        means that an end-of-line character will be the first character of the range.    ]]>
 *
 * <![CDATA[     Specifies the boundary conditions determining a run of text as returned from       GetTextAtOffset, GetTextAfterOffset, and GetTextBeforeOffset.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_TEXT_BOUNDARY_CHAR,
    Accessibility_TEXT_BOUNDARY_WORD_START,
    Accessibility_TEXT_BOUNDARY_WORD_END,
    Accessibility_TEXT_BOUNDARY_SENTENCE_START,
    Accessibility_TEXT_BOUNDARY_SENTENCE_END,
    Accessibility_TEXT_BOUNDARY_LINE_START,
    Accessibility_TEXT_BOUNDARY_LINE_END,
} Accessibility_TEXT_BOUNDARY_TYPE;

/**
 * NUM_ACCESSIBILITY_TEXT_BOUNDARY_TYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_TEXT_BOUNDARY_TYPE.
 */
#define NUM_ACCESSIBILITY_TEXT_BOUNDARY_TYPES (6+1)

/**
 *
Accessibility_TEXT_CLIP_TYPE:
 * @Accessibility_TEXT_CLIP_NONE: <![CDATA[]]>
 * @Accessibility_TEXT_CLIP_MIN: <![CDATA[     Characters/glyphs clipped by the minimum coordinate are omitted    ]]>
 * @Accessibility_TEXT_CLIP_MAX: <![CDATA[     Characters/glyphs which intersect the maximum coordinate are omitted    ]]>
 * @Accessibility_TEXT_CLIP_BOTH: <![CDATA[     Only glyphs falling entirely within the region bounded by min and max are retained.    ]]>
 *
 * <![CDATA[     TEXT_CLIP_TYPE:       CLIP_MIN means text clipped by min coordinate is omitted,       CLIP_MAX clips text interescted by the max coord, and CLIP_BOTH       will retain only text falling fully within the min/max bounds.     ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_TEXT_CLIP_NONE,
    Accessibility_TEXT_CLIP_MIN,
    Accessibility_TEXT_CLIP_MAX,
    Accessibility_TEXT_CLIP_BOTH,
} Accessibility_TEXT_CLIP_TYPE;

/**
 * NUM_ACCESSIBILITY_TEXT_CLIP_TYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_TEXT_CLIP_TYPE.
 */
#define NUM_ACCESSIBILITY_TEXT_CLIP_TYPES (3+1)

/**
 *
Accessibility_StateType:
 * @Accessibility_STATE_INVALID: <![CDATA[]]>
 * @Accessibility_STATE_ACTIVE: <![CDATA[     Indicates a window is currently the active window, or is an active subelement within a container or table    ]]>
 * @Accessibility_STATE_ARMED: <![CDATA[     Indicates that the object is armed    ]]>
 * @Accessibility_STATE_BUSY: <![CDATA[     Indicates the current object is busy, i.e. onscreen representation is in the process of changing, or       the object is temporarily unavailable for interaction due to activity already in progress.   ]]>
 * @Accessibility_STATE_CHECKED: <![CDATA[     Indicates this object is currently checked    ]]>
 * @Accessibility_STATE_COLLAPSED: <![CDATA[     Indicates this object is collapsed    ]]>
 * @Accessibility_STATE_DEFUNCT: <![CDATA[     Indicates that this object no longer has a valid backing widget        (for instance, if its peer object has been destroyed)    ]]>
 * @Accessibility_STATE_EDITABLE: <![CDATA[     Indicates the user can change the contents of this object    ]]>
 * @Accessibility_STATE_ENABLED: <![CDATA[     Indicates that this object is enabled, i.e. that it currently reflects some application state.        Objects that are "greyed out" may lack this state, and may lack the STATE_SENSITIVE if direct user       interaction cannot cause them to acquire STATE_ENABLED.  @see STATE_SENSITIVE.   ]]>
 * @Accessibility_STATE_EXPANDABLE: <![CDATA[     Indicates this object allows progressive disclosure of its children    ]]>
 * @Accessibility_STATE_EXPANDED: <![CDATA[     Indicates this object its expanded    ]]>
 * @Accessibility_STATE_FOCUSABLE: <![CDATA[     Indicates this object can accept keyboard focus, which means all       events resulting from typing on the keyboard will normally be passed       to it when it has focus   ]]>
 * @Accessibility_STATE_FOCUSED: <![CDATA[     Indicates this object currently has the keyboard focus    ]]>
 * @Accessibility_STATE_HAS_TOOLTIP: <![CDATA[     Indicates that the object has an associated tooltip    ]]>
 * @Accessibility_STATE_HORIZONTAL: <![CDATA[     Indicates the orientation of thsi object is horizontal    ]]>
 * @Accessibility_STATE_ICONIFIED: <![CDATA[     Indicates this object is minimized and is represented only by an icon    ]]>
 * @Accessibility_STATE_MODAL: <![CDATA[     Indicates something must be done with this object before the user can       interact with an object in a different window.   ]]>
 * @Accessibility_STATE_MULTI_LINE: <![CDATA[     Indicates this (text) object can contain multiple lines of text    ]]>
 * @Accessibility_STATE_MULTISELECTABLE: <![CDATA[     Indicates this object allows more than one of its children to be       selected at the same time, or in the case of text objects,        that the object supports non-contiguous text selections.   ]]>
 * @Accessibility_STATE_OPAQUE: <![CDATA[     Indicates this object paints every pixel within its rectangular region.       It also indicates an alpha value of unity, if it supports alpha blending.    ]]>
 * @Accessibility_STATE_PRESSED: <![CDATA[     Indicates this object is currently pressed    ]]>
 * @Accessibility_STATE_RESIZABLE: <![CDATA[     Indicates the size of this object's size is not fixed    ]]>
 * @Accessibility_STATE_SELECTABLE: <![CDATA[     Indicates this object is the child of an object that allows its       children to be selected and that this child is one of those children       that can be selected.   ]]>
 * @Accessibility_STATE_SELECTED: <![CDATA[     Indicates this object is the child of an object that allows its       children to be selected and that this child is one of those children       that has been selected.   ]]>
 * @Accessibility_STATE_SENSITIVE: <![CDATA[     Indicates this object is sensitive, e.g. to user interaction.        STATE_SENSITIVE usually accompanies STATE_ENABLED for user-actionable controls,       but may be found in the absence of STATE_ENABLED if the current visible state of the        control is "disconnected" from the application state.  In such cases, direct user interaction       can often result in the object gaining STATE_SENSITIVE, for instance if a user makes        an explicit selection using an object whose current state is ambiguous or undefined.       @see STATE_ENABLED, STATE_INDETERMINATE.    ]]>
 * @Accessibility_STATE_SHOWING: <![CDATA[     Indicates this object, the object's parent, the object's parent's       parent, and so on, are all 'shown' to the end-user, i.e.       subject to "exposure" if blocking or obscuring objects do not interpose       between this object and the top of the window stack.   ]]>
 * @Accessibility_STATE_SINGLE_LINE: <![CDATA[     Indicates this (text) object can contain only a single line of text    ]]>
 * @Accessibility_STATE_STALE: <![CDATA[     Indicates that the information returned for this object may no longer be       synchronized with the application state.  This can occur if the object has STATE_TRANSIENT,       and can also occur towards the end of the object peer's lifecycle.    ]]>
 * @Accessibility_STATE_TRANSIENT: <![CDATA[     Indicates this object is transient    ]]>
 * @Accessibility_STATE_VERTICAL: <![CDATA[     Indicates the orientation of this object is vertical; for example this state may appear on        such objects as scrollbars, text objects (with vertical text flow), separators, etc.   ]]>
 * @Accessibility_STATE_VISIBLE: <![CDATA[ 	  Indicates this object is visible, e.g. has been explicitly marked for exposure to the user. 	  STATE_VISIBLE is no guarantee that the object is actually unobscured on the screen, only       that it is 'potentially' visible, barring obstruction, being scrolled or clipped out of the        field of view, or having an ancestor container that has not yet made visible.       A widget is potentially onscreen if it has both STATE_VISIBLE and STATE_SHOWING.       The absence of STATE_VISIBLE and STATE_SHOWING is semantically equivalent to saying        that an object is 'hidden'.   ]]>
 * @Accessibility_STATE_MANAGES_DESCENDANTS: <![CDATA[     Indicates that "active-descendant-changed" event is sent when children       become 'active' (i.e. are selected or navigated to onscreen).  Used to       prevent need to enumerate all children in very large containers, like       tables.  The presence of STATE_MANAGES_DESCENDANTS is an indication to the client.       that the children should not, and need not, be enumerated by the client.       Objects implementing this state are expected to provide relevant state       notifications to listening clients, for instance notifications of visibility       changes and activation of their contained child objects, without the client        having previously requested references to those children.   ]]>
 * @Accessibility_STATE_INDETERMINATE: <![CDATA[     Indicates that a check box or other boolean indicator is in a state other than        checked or not checked.  This usually means that the boolean value reflected or        controlled by the object does not apply consistently to the entire current context.       For example, a checkbox for the "Bold" attribute of text may have STATE_INDETERMINATE       if the currently selected text contains a mixture of weight attributes.         In many cases interacting with a STATE_INDETERMINATE object will cause        the context's corresponding boolean attribute to be homogenized, whereupon the object       will lose STATE_INDETERMINATE and a corresponding state-changed event will be fired.   ]]>
 * @Accessibility_STATE_REQUIRED: <![CDATA[     Indicates that user interaction with this object is 'required' from the user,        for instance before completing the processing of a form.   ]]>
 * @Accessibility_STATE_TRUNCATED: <![CDATA[ 	  Indicates that an object's onscreen content is truncated, e.g. a text value in a spreadsheet cell.    ]]>
 * @Accessibility_STATE_ANIMATED: <![CDATA[     Indicates this object's visual representation is dynamic, not static.       This state may be applied to an object during an animated 'effect' and        be removed from the object once its visual representation becomes static.     some applications, notably content viewers, may not be able to detect       all kinds of animated content.  Therefore the absence of this state should not       be taken as definitive evidence that the object's visual representation is       static; this state is advisory.   ]]>
 * @Accessibility_STATE_INVALID_ENTRY: <![CDATA[     This object has indicated an error condition due to failure of input       validation.  For instance, a form control may acquire this state in response       to invalid or malformed user input.     ]]>
 * @Accessibility_STATE_SUPPORTS_AUTOCOMPLETION: <![CDATA[     This state indicates that the object in question implements some form of &#168;typeahead&#168; or        pre-selection behavior whereby entering the first character of one or more sub-elements       causes those elements to scroll into view or become selected.  Subsequent character input       may narrow the selection further as long as one or more sub-elements match the string.       This state is normally only useful and encountered on objects that implement Selection.       In some cases the typeahead behavior may result in full or partial &#168;completion&#168; of        the data in the input field, in which case these input events may trigger text-changed       events from the source.     ]]>
 * @Accessibility_STATE_SELECTABLE_TEXT: <![CDATA[     This state indicates that the object in question supports text selection.        It should only be exposed on objects which implement the Text interface,        in order to distinguish this state from STATE_SELECTABLE, which infers that       the object in question is a selectable child of an object which implements       Selection.  While similar, text selection and subelement selection are       distinct operations.     ]]>
 * @Accessibility_STATE_IS_DEFAULT: <![CDATA[     This state indicates that the object in question is the 'default' interaction object        in a dialog, i.e. the one that gets activated if the user presses "Enter" when the       dialog is initially posted.     ]]>
 * @Accessibility_STATE_VISITED: <![CDATA[     This state indicates that the object (typically a hyperlink)       has already been activated or invoked, with the result that some backing data       has been downloaded or rendered.     ]]>
 * @Accessibility_STATE_LAST_DEFINED: <![CDATA[     This value of the enumeration should not be used as a parameter, it indicates the number of       items in the StateType enumeration.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_STATE_INVALID,
    Accessibility_STATE_ACTIVE,
    Accessibility_STATE_ARMED,
    Accessibility_STATE_BUSY,
    Accessibility_STATE_CHECKED,
    Accessibility_STATE_COLLAPSED,
    Accessibility_STATE_DEFUNCT,
    Accessibility_STATE_EDITABLE,
    Accessibility_STATE_ENABLED,
    Accessibility_STATE_EXPANDABLE,
    Accessibility_STATE_EXPANDED,
    Accessibility_STATE_FOCUSABLE,
    Accessibility_STATE_FOCUSED,
    Accessibility_STATE_HAS_TOOLTIP,
    Accessibility_STATE_HORIZONTAL,
    Accessibility_STATE_ICONIFIED,
    Accessibility_STATE_MODAL,
    Accessibility_STATE_MULTI_LINE,
    Accessibility_STATE_MULTISELECTABLE,
    Accessibility_STATE_OPAQUE,
    Accessibility_STATE_PRESSED,
    Accessibility_STATE_RESIZABLE,
    Accessibility_STATE_SELECTABLE,
    Accessibility_STATE_SELECTED,
    Accessibility_STATE_SENSITIVE,
    Accessibility_STATE_SHOWING,
    Accessibility_STATE_SINGLE_LINE,
    Accessibility_STATE_STALE,
    Accessibility_STATE_TRANSIENT,
    Accessibility_STATE_VERTICAL,
    Accessibility_STATE_VISIBLE,
    Accessibility_STATE_MANAGES_DESCENDANTS,
    Accessibility_STATE_INDETERMINATE,
    Accessibility_STATE_REQUIRED,
    Accessibility_STATE_TRUNCATED,
    Accessibility_STATE_ANIMATED,
    Accessibility_STATE_INVALID_ENTRY,
    Accessibility_STATE_SUPPORTS_AUTOCOMPLETION,
    Accessibility_STATE_SELECTABLE_TEXT,
    Accessibility_STATE_IS_DEFAULT,
    Accessibility_STATE_VISITED,
    Accessibility_STATE_LAST_DEFINED,
} Accessibility_StateType;

/**
 * NUM_ACCESSIBILITY_STATETYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_StateType.
 */
#define NUM_ACCESSIBILITY_STATETYPES (41+1)

/**
 *
Accessibility_KeyEventType:
 * @Accessibility_KEY_PRESSED: <![CDATA[]]>
 * @Accessibility_KEY_RELEASED: <![CDATA[]]>
 *
 * <![CDATA[     Deprecated, DO NOT USE!    ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_KEY_PRESSED,
    Accessibility_KEY_RELEASED,
} Accessibility_KeyEventType;

/**
 * NUM_ACCESSIBILITY_KEYEVENTTYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_KeyEventType.
 */
#define NUM_ACCESSIBILITY_KEYEVENTTYPES (1+1)

/**
 *
Accessibility_EventType:
 * @Accessibility_KEY_PRESSED_EVENT: <![CDATA[     < key on a keyboard device was pressed.    ]]>
 * @Accessibility_KEY_RELEASED_EVENT: <![CDATA[     < key on a keyboard device was released.    ]]>
 * @Accessibility_BUTTON_PRESSED_EVENT: <![CDATA[     < button on a non-keyboard human interface device        (HID) was pressed    ]]>
 * @Accessibility_BUTTON_RELEASED_EVENT: <![CDATA[     < button on a non-keyboard human interface device        (HID) was pressed    ]]>
 *
 * <![CDATA[     Used to specify the event types of interest to an EventListener, or       to identify the type of an event for which notification has been sent.        @see EventTypeSeq, DeviceEvent::type   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_KEY_PRESSED_EVENT,
    Accessibility_KEY_RELEASED_EVENT,
    Accessibility_BUTTON_PRESSED_EVENT,
    Accessibility_BUTTON_RELEASED_EVENT,
} Accessibility_EventType;

/**
 * NUM_ACCESSIBILITY_EVENTTYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_EventType.
 */
#define NUM_ACCESSIBILITY_EVENTTYPES (3+1)

/**
 *
Accessibility_KeySynthType:
 * @Accessibility_KEY_PRESS: <![CDATA[     emulate the pressing of a hardware keyboard key.    ]]>
 * @Accessibility_KEY_RELEASE: <![CDATA[     emulate the release of a hardware keyboard key.    ]]>
 * @Accessibility_KEY_PRESSRELEASE: <![CDATA[     a hardware keyboard key is pressed and immediately released.    ]]>
 * @Accessibility_KEY_SYM: <![CDATA[     a symbolic key event is generated, without specifying a hardware key.        @note if the keysym is not present in the current keyboard map,       the DeviceEventController instance has a limited ability to generate       such keysyms on-the-fly.  Reliability of GenerateKeyboardEvent calls       using out-of-keymap keysyms will vary from system to system, and on the       number of different out-of-keymap being generated in quick succession.       In practice this is rarely significant, since the keysyms of interest to       AT clients and keyboard emulators are usually part of the current keymap, i.e.       present on the system keyboard for the current locale (even if a physical       hardware keyboard is not connected.   ]]>
 * @Accessibility_KEY_STRING: <![CDATA[     a string is converted to its equivalent keyboard events and emitted.        If the string consists of complex character or composed characters       which are not in the current keymap, string emission is subject to the       out-of-keymap limitations described for KeySynthType::KEY_SYM.       In practice this limitation primarily effects Chinese and Japanese locales.   ]]>
 *
 * <![CDATA[     Used when synthesizing keyboard input via DeviceEventController:GenerateKeyboardEvent.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_KEY_PRESS,
    Accessibility_KEY_RELEASE,
    Accessibility_KEY_PRESSRELEASE,
    Accessibility_KEY_SYM,
    Accessibility_KEY_STRING,
} Accessibility_KeySynthType;

/**
 * NUM_ACCESSIBILITY_KEYSYNTHTYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_KeySynthType.
 */
#define NUM_ACCESSIBILITY_KEYSYNTHTYPES (4+1)

/**
 *
Accessibility_ModifierType:
 * @Accessibility_MODIFIER_SHIFT: <![CDATA[     The left or right 'Shift' key    ]]>
 * @Accessibility_MODIFIER_SHIFTLOCK: <![CDATA[     The ShiftLock or CapsLock key    ]]>
 * @Accessibility_MODIFIER_CONTROL: <![CDATA[     'Control'/'Ctrl'    ]]>
 * @Accessibility_MODIFIER_ALT: <![CDATA[     The Alt key (as opposed to AltGr)    ]]>
 * @Accessibility_MODIFIER_META: <![CDATA[     depending on the platform this may map to 'Window', 'Function', 'Meta',       'Menu', or 'NumLock'.         Such 'Meta keys' will map to one of META, META2, META3.       On X Windows platforms these META values map to        the modifier masks Mod1Mask, Mod2Mask, Mod3Mask, e.g. an event having       ModifierType::MODIFIER_META2 means that the 'Mod2Mask' bit is       set in the corresponding XEvent.   ]]>
 * @Accessibility_MODIFIER_META2: <![CDATA[]]>
 * @Accessibility_MODIFIER_META3: <![CDATA[]]>
 * @Accessibility_MODIFIER_NUMLOCK: <![CDATA[     A symbolic meta key name that is mapped by AT-SPI to the        appropriate META value, for the convenience of the client.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_MODIFIER_SHIFT,
    Accessibility_MODIFIER_SHIFTLOCK,
    Accessibility_MODIFIER_CONTROL,
    Accessibility_MODIFIER_ALT,
    Accessibility_MODIFIER_META,
    Accessibility_MODIFIER_META2,
    Accessibility_MODIFIER_META3,
    Accessibility_MODIFIER_NUMLOCK,
} Accessibility_ModifierType;

/**
 * NUM_ACCESSIBILITY_MODIFIERTYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_ModifierType.
 */
#define NUM_ACCESSIBILITY_MODIFIERTYPES (7+1)

/**
 *
Accessibility_RelationType:
 * @Accessibility_RELATION_NULL: <![CDATA[     Not a meaningful relationship; clients should not normally encounter this RelationType value.    ]]>
 * @Accessibility_RELATION_LABEL_FOR: <![CDATA[     Object is a label for one or more other objects.    ]]>
 * @Accessibility_RELATION_LABELLED_BY: <![CDATA[     Object is labelled by one or more other objects.    ]]>
 * @Accessibility_RELATION_CONTROLLER_FOR: <![CDATA[     Object is an interactive object which modifies the state, onscreen location, or other attributes       of one or more target objects.    ]]>
 * @Accessibility_RELATION_CONTROLLED_BY: <![CDATA[     Object state, position, etc. is modified/controlled by user interaction with one or        more other objects.   For instance a viewport or scroll pane may be CONTROLLED_BY scrollbars.    ]]>
 * @Accessibility_RELATION_MEMBER_OF: <![CDATA[     Object has a grouping relationship (e.g. 'same group as') to one or more other objects.     ]]>
 * @Accessibility_RELATION_TOOLTIP_FOR: <![CDATA[     Object is a tooltip associated with another object.    ]]>
 * @Accessibility_RELATION_NODE_CHILD_OF: <![CDATA[     Object is a child of the target.   ]]>
 * @Accessibility_RELATION_NODE_PARENT_OF: <![CDATA[     Object is a parent of the target.   ]]>
 * @Accessibility_RELATION_EXTENDED: <![CDATA[     Used to indicate that a relationship exists, but its type is not specified in the enumeration       and must be obtained via a call to getRelationTypeName.     ]]>
 * @Accessibility_RELATION_FLOWS_TO: <![CDATA[     Object renders content which flows logically to another object.         For instance, text in a paragraph may flow to another object which is not the        'next sibling' in the accessibility hierarchy.    ]]>
 * @Accessibility_RELATION_FLOWS_FROM: <![CDATA[     Reciprocal of RELATION_FLOWS_TO.    ]]>
 * @Accessibility_RELATION_SUBWINDOW_OF: <![CDATA[     Object is visually and semantically considered a subwindow of another object, even though       it is not the object's child.  Useful when dealing with embedded applications and other cases       where the widget hierarchy does not map cleanly to the onscreen presentation.     ]]>
 * @Accessibility_RELATION_EMBEDS: <![CDATA[     Similar to SUBWINDOW_OF, but specifically used for cross-process embedding.     ]]>
 * @Accessibility_RELATION_EMBEDDED_BY: <![CDATA[     Reciprocal of RELATION_EMBEDS; Used to denote content rendered by embedded renderers that       live in a separate process space from the embedding context.     ]]>
 * @Accessibility_RELATION_POPUP_FOR: <![CDATA[     Denotes that the object is a transient window or frame associated with another onscreen object.       Similar to TOOLTIP_FOR, but more general.  Useful for windows which are technically        toplevels but which, for one or more reasons, do not explicitly cause their associated       window to lose 'window focus'. Creation of a ROLE_WINDOW object with the POPUP_FOR relation       usually requires some presentation action on the part of assistive technology clients, even though       the previous toplevel ROLE_FRAME object may still be the active window.     ]]>
 * @Accessibility_RELATION_PARENT_WINDOW_OF: <![CDATA[     This is the reciprocal relation to RELATION_POPUP_FOR.    ]]>
 * @Accessibility_RELATION_DESCRIPTION_FOR: <![CDATA[     Indicates that an object provides descriptive information        about another object; more verbose than RELATION_LABEL_FOR.    ]]>
 * @Accessibility_RELATION_DESCRIBED_BY: <![CDATA[     Indicates that another object provides descriptive information        about this object; more verbose than RELATION_LABELLED_BY.    ]]>
 * @Accessibility_RELATION_LAST_DEFINED: <![CDATA[     Do not use as a parameter value, used to determine the size of the enumeration.    ]]>
 *
 * <![CDATA[     RelationType specifies a relationship between objects (possibly one-to-many or many-to-one)       outside of the normal parent/child hierarchical relationship.  It allows better semantic       identification of how objects are associated with one another.       For instance the RELATION_LABELLED_BY relationship may be used to identify labelling information       that should accompany the accessibleName property when presenting an object's content or identity       to the end user.  Similarly, RELATION_CONTROLLER_FOR can be used to further specify the context       in which a valuator is useful, and/or the other UI components which are directly effected by       user interactions with the valuator. Common examples include association of scrollbars with       the viewport or panel which they control.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_RELATION_NULL,
    Accessibility_RELATION_LABEL_FOR,
    Accessibility_RELATION_LABELLED_BY,
    Accessibility_RELATION_CONTROLLER_FOR,
    Accessibility_RELATION_CONTROLLED_BY,
    Accessibility_RELATION_MEMBER_OF,
    Accessibility_RELATION_TOOLTIP_FOR,
    Accessibility_RELATION_NODE_CHILD_OF,
    Accessibility_RELATION_NODE_PARENT_OF,
    Accessibility_RELATION_EXTENDED,
    Accessibility_RELATION_FLOWS_TO,
    Accessibility_RELATION_FLOWS_FROM,
    Accessibility_RELATION_SUBWINDOW_OF,
    Accessibility_RELATION_EMBEDS,
    Accessibility_RELATION_EMBEDDED_BY,
    Accessibility_RELATION_POPUP_FOR,
    Accessibility_RELATION_PARENT_WINDOW_OF,
    Accessibility_RELATION_DESCRIPTION_FOR,
    Accessibility_RELATION_DESCRIBED_BY,
    Accessibility_RELATION_LAST_DEFINED,
} Accessibility_RelationType;

/**
 * NUM_ACCESSIBILITY_RELATIONTYPES:
 *
 * 1 higher than the highest valid value of #Accessibility_RelationType.
 */
#define NUM_ACCESSIBILITY_RELATIONTYPES (19+1)

/**
 *
Accessibility_Role:
 * @Accessibility_ROLE_INVALID: <![CDATA[     A Role indicating an error condition, such as uninitialized Role data.    ]]>
 * @Accessibility_ROLE_ACCELERATOR_LABEL: <![CDATA[     Object is a label indicating the keyboard accelerators for the parent     ]]>
 * @Accessibility_ROLE_ALERT: <![CDATA[     Object is used to alert the user about something    ]]>
 * @Accessibility_ROLE_ANIMATION: <![CDATA[     Object contains a dynamic or moving image of some kind    ]]>
 * @Accessibility_ROLE_ARROW: <![CDATA[     Object is a 2d directional indicator    ]]>
 * @Accessibility_ROLE_CALENDAR: <![CDATA[     Object contains one or more dates, usually arranged into a 2d list    ]]>
 * @Accessibility_ROLE_CANVAS: <![CDATA[     Object that can be drawn into and is used to trap events    ]]>
 * @Accessibility_ROLE_CHECK_BOX: <![CDATA[     A choice that can be checked or unchecked and provides a separate       indicator for the current state.   ]]>
 * @Accessibility_ROLE_CHECK_MENU_ITEM: <![CDATA[     A menu item that behaves like a check box (see ROLE_CHECK_BOX)    ]]>
 * @Accessibility_ROLE_COLOR_CHOOSER: <![CDATA[     A specialized dialog that lets the user choose a color.    ]]>
 * @Accessibility_ROLE_COLUMN_HEADER: <![CDATA[     The header for a column of data    ]]>
 * @Accessibility_ROLE_COMBO_BOX: <![CDATA[     A list of choices the user can select from    ]]>
 * @Accessibility_ROLE_DATE_EDITOR: <![CDATA[     An object which allows entry of a date    ]]>
 * @Accessibility_ROLE_DESKTOP_ICON: <![CDATA[     An inconifed internal frame within a DESKTOP_PANE    ]]>
 * @Accessibility_ROLE_DESKTOP_FRAME: <![CDATA[     A pane that supports internal frames and iconified versions of those       internal frames.   ]]>
 * @Accessibility_ROLE_DIAL: <![CDATA[     An object that allows a value to be changed via rotating a visual element,       or which displays a value via such a rotating element.    ]]>
 * @Accessibility_ROLE_DIALOG: <![CDATA[     A top level window with title bar and a border    ]]>
 * @Accessibility_ROLE_DIRECTORY_PANE: <![CDATA[     A pane that allows the user to navigate through and select the contents       of a directory   ]]>
 * @Accessibility_ROLE_DRAWING_AREA: <![CDATA[     A specialized dialog that displays the files in the directory and lets       the user select a file, browse a different directory, or specify a       filename.   ]]>
 * @Accessibility_ROLE_FILE_CHOOSER: <![CDATA[     An object used for drawing custom user interface elements.   ]]>
 * @Accessibility_ROLE_FILLER: <![CDATA[     A object that fills up space in a user interface   ]]>
 * @Accessibility_ROLE_FOCUS_TRAVERSABLE: <![CDATA[     Don't use, reserved for future use.    ]]>
 * @Accessibility_ROLE_FONT_CHOOSER: <![CDATA[     Allows selection of a display font    ]]>
 * @Accessibility_ROLE_FRAME: <![CDATA[     A top level window with a title bar, border, menubar, etc.    ]]>
 * @Accessibility_ROLE_GLASS_PANE: <![CDATA[     A pane that is guaranteed to be painted on top of all panes beneath it    ]]>
 * @Accessibility_ROLE_HTML_CONTAINER: <![CDATA[     A document container for HTML, whose children       represent the document content.   ]]>
 * @Accessibility_ROLE_ICON: <![CDATA[     A small fixed size picture, typically used to decorate components    ]]>
 * @Accessibility_ROLE_IMAGE: <![CDATA[     An image, typically static.    ]]>
 * @Accessibility_ROLE_INTERNAL_FRAME: <![CDATA[     A frame-like object that is clipped by a desktop pane.    ]]>
 * @Accessibility_ROLE_LABEL: <![CDATA[     An object used to present an icon or short string in an interface    ]]>
 * @Accessibility_ROLE_LAYERED_PANE: <![CDATA[     A specialized pane that allows its children to be drawn in layers,       providing a form of stacking order.   ]]>
 * @Accessibility_ROLE_LIST: <![CDATA[     An object that presents a list of objects to the user and allows the       user to select one or more of them.   ]]>
 * @Accessibility_ROLE_LIST_ITEM: <![CDATA[     An object that represents an element of a list.    ]]>
 * @Accessibility_ROLE_MENU: <![CDATA[     An object usually found inside a menu bar that contains a list of       actions the user can choose from.   ]]>
 * @Accessibility_ROLE_MENU_BAR: <![CDATA[     An object usually drawn at the top of the primary dialog box of an       application that contains a list of menus the user can choose from.   ]]>
 * @Accessibility_ROLE_MENU_ITEM: <![CDATA[     An object usually contained in a menu that presents an action the       user can choose.   ]]>
 * @Accessibility_ROLE_OPTION_PANE: <![CDATA[     A specialized pane whose primary use is inside a DIALOG    ]]>
 * @Accessibility_ROLE_PAGE_TAB: <![CDATA[     An object that is a child of a page tab list    ]]>
 * @Accessibility_ROLE_PAGE_TAB_LIST: <![CDATA[     An object that presents a series of panels (or page tabs), one at a time,       through some mechanism provided by the object.   ]]>
 * @Accessibility_ROLE_PANEL: <![CDATA[     A generic container that is often used to group objects.    ]]>
 * @Accessibility_ROLE_PASSWORD_TEXT: <![CDATA[     A text object uses for passwords, or other places where the text       content is not shown visibly to the user.   ]]>
 * @Accessibility_ROLE_POPUP_MENU: <![CDATA[     A temporary window that is usually used to offer the user a list of       choices, and then hides when the user selects one of those choices.   ]]>
 * @Accessibility_ROLE_PROGRESS_BAR: <![CDATA[     An object used to indicate how much of a task has been completed.    ]]>
 * @Accessibility_ROLE_PUSH_BUTTON: <![CDATA[     An object the user can manipulate to tell the application to do       something.   ]]>
 * @Accessibility_ROLE_RADIO_BUTTON: <![CDATA[     A specialized check box that will cause other radio buttons in the       same group to become uncghecked when this one is checked.   ]]>
 * @Accessibility_ROLE_RADIO_MENU_ITEM: <![CDATA[     Object is both a menu item and a "radio button" (see ROLE_RADIO_BUTTON)    ]]>
 * @Accessibility_ROLE_ROOT_PANE: <![CDATA[     A specialized pane that has a glass pane and a layered pane as its       children.   ]]>
 * @Accessibility_ROLE_ROW_HEADER: <![CDATA[     The header for a row of data    ]]>
 * @Accessibility_ROLE_SCROLL_BAR: <![CDATA[     An object usually used to allow a user to incrementally view a large       amount of data by moving the bounds of a viewport along a one-dimensional axis.   ]]>
 * @Accessibility_ROLE_SCROLL_PANE: <![CDATA[     An object that allows a user to incrementally view a large amount       of information.  ROLE_SCROLL_PANE objects are usually accompanied by       ROLE_SCROLL_BAR controllers, on which the RELATION_CONTROLLER_FOR and       RELATION_CONTROLLED_BY reciprocal relations are set; \see        Accessibility::RelationSet.   ]]>
 * @Accessibility_ROLE_SEPARATOR: <![CDATA[     An object usually contained in a menu to provide a visible and       logical separation of the contents in a menu.   ]]>
 * @Accessibility_ROLE_SLIDER: <![CDATA[     An object that allows the user to select from a bounded range    ]]>
 * @Accessibility_ROLE_SPIN_BUTTON: <![CDATA[     An object which allows one of a set of choices to be selected,        and which displays the current choice.  Unlike ROLE_SCROLL_BAR,       ROLE_SLIDER objects need not control 'viewport'-like objects.   ]]>
 * @Accessibility_ROLE_SPLIT_PANE: <![CDATA[     A specialized panel that presents two other panels at the same time.    ]]>
 * @Accessibility_ROLE_STATUS_BAR: <![CDATA[     Object displays non-quantitative status information (c.f. ROLE_PROGRESS_BAR)    ]]>
 * @Accessibility_ROLE_TABLE: <![CDATA[     An object used to repesent information in terms of rows and columns.    ]]>
 * @Accessibility_ROLE_TABLE_CELL: <![CDATA[     A 'cell' or discrete child within a Table. \note Table cells need not have ROLE_TABLE_CELL,        other RoleType values are valid as well.     ]]>
 * @Accessibility_ROLE_TABLE_COLUMN_HEADER: <![CDATA[     An object which labels a particular column in a Table.    ]]>
 * @Accessibility_ROLE_TABLE_ROW_HEADER: <![CDATA[     An object which labels a particular row in a Table. Table rows and columns may also be        labelled via the RELATION_LABEL_FOR/RELATION_LABELLED_BY relationships;        see Accessibility.RelationSet.    ]]>
 * @Accessibility_ROLE_TEAROFF_MENU_ITEM: <![CDATA[     Object allows menu to be removed from menubar and shown in its own window.    ]]>
 * @Accessibility_ROLE_TERMINAL: <![CDATA[     An object that emulates a terminal    ]]>
 * @Accessibility_ROLE_TEXT: <![CDATA[     An object that presents text to the user, of nonspecific type.    ]]>
 * @Accessibility_ROLE_TOGGLE_BUTTON: <![CDATA[     A specialized push button that can be checked or unchecked, but does       not procide a separate indicator for the current state.   ]]>
 * @Accessibility_ROLE_TOOL_BAR: <![CDATA[     A bar or palette usually composed of push buttons or toggle buttons   ]]>
 * @Accessibility_ROLE_TOOL_TIP: <![CDATA[     An object that provides information about another object   ]]>
 * @Accessibility_ROLE_TREE: <![CDATA[     An object used to repsent hierarchical information to the user.    ]]>
 * @Accessibility_ROLE_TREE_TABLE: <![CDATA[     An object that presents both tabular and hierarchical info to the user    ]]>
 * @Accessibility_ROLE_UNKNOWN: <![CDATA[     The object contains some Accessible information, but its role is       not known.     ]]>
 * @Accessibility_ROLE_VIEWPORT: <![CDATA[     An object usually used in a scroll pane, or to otherwise clip a larger object or        content renderer to a specific onscreen viewport.    ]]>
 * @Accessibility_ROLE_WINDOW: <![CDATA[     A &#168;top level window&#168; with no title or border.    ]]>
 * @Accessibility_ROLE_EXTENDED: <![CDATA[     means that the role for this item is known, but not included in the        core enumeration    ]]>
 * @Accessibility_ROLE_HEADER: <![CDATA[     An object that serves as a document header.    ]]>
 * @Accessibility_ROLE_FOOTER: <![CDATA[     An object that serves as a document footer.    ]]>
 * @Accessibility_ROLE_PARAGRAPH: <![CDATA[     An object which is contains a single paragraph of text content. See also ROLE_TEXT.    ]]>
 * @Accessibility_ROLE_RULER: <![CDATA[     An object which describes margins and tab stops, etc.        for text objects which it controls        (should have CONTROLLER_FOR relation to such).    ]]>
 * @Accessibility_ROLE_APPLICATION: <![CDATA[     An object corresponding to the toplevel accessible of an        application, which may contain ROLE_FRAME objects or other       accessible objects.  Children of AccessibleDesktop objects        are generally ROLE_APPLICATION objects.   ]]>
 * @Accessibility_ROLE_AUTOCOMPLETE: <![CDATA[     The object is a dialog or list containing items for insertion        into an entry widget, for instance a list of words for completion        of a text entry.   ]]>
 * @Accessibility_ROLE_EDITBAR: <![CDATA[     The object is an editable text object in a toolbar.   ]]>
 * @Accessibility_ROLE_EMBEDDED: <![CDATA[     The object is an embedded component container.  This role is a        "grouping" hint that the contained objects share a context which is        different from the container in which this accessible is embedded.       In particular, it is used for some kinds of document embedding, and       for embedding of out-of-process component, "panel applets", etc.   ]]>
 * @Accessibility_ROLE_ENTRY: <![CDATA[     The object is a component whose textual content may be entered or modified by the user, 	    provided STATE_EDITABLE is present.       A readonly ROLE_ENTRY object (i.e. where STATE_EDITABLE is not present) implies a       read-only 'text field' in a form, as opposed to a title, label, or caption.     ]]>
 * @Accessibility_ROLE_CHART: <![CDATA[     The object is a graphical depiction of quantitative data.  It may contain multiple       subelements whose attributes and/or description may be queried to obtain both the       quantitative data and information about how the data is being presented.       The LABELLED_BY relation is particularly important in interpreting objects of this type,       as is the accessible-description property.         See ROLE_CAPTION     ]]>
 * @Accessibility_ROLE_CAPTION: <![CDATA[     The object contains descriptive information, usually textual, about another user interface       element such as a table, chart, or image.     ]]>
 * @Accessibility_ROLE_DOCUMENT_FRAME: <![CDATA[     The object is a visual frame or container which contains a view of document content.         Document frames may occur within another Document instance, in which case the second        document may be said to be embedded in the containing instance.  HTML frames are       often ROLE_DOCUMENT_FRAME.  Either this object, or a singleton descendant, should implement       the Document interface.     ]]>
 * @Accessibility_ROLE_HEADING: <![CDATA[     The object serves as a heading for content which follows it in a document.       The 'heading level' of the heading, if availabe,  may be obtained by       querying the object's attributes.      ]]>
 * @Accessibility_ROLE_PAGE: <![CDATA[     The object is a containing instance which encapsulates a page of        information.  ROLE_PAGE is used in documents and content which support       a paginated navigation model.     ]]>
 * @Accessibility_ROLE_SECTION: <![CDATA[     The object is a containing instance of document content which constitutes       a particular 'logical' section of the document.  The type of content within       a section, and the nature of the section division itself, may be obtained       by querying the object's attributes.  Sections may be nested.     ]]>
 * @Accessibility_ROLE_REDUNDANT_OBJECT: <![CDATA[     The object is redundant with another object in the hierarchy,        and is exposed for purely technical reasons.  Objects of this role        should be ignored by clients, if they are encountered at all.      ]]>
 * @Accessibility_ROLE_FORM: <![CDATA[     The object is a containing instance of document content which       has within it components with which the user can interact in order to       input information; i.e. the object is a container for pushbuttons,       comboboxes, text input fields, and other 'GUI' components.       ROLE_FORM should not, in general, be used for toplevel GUI containers       or dialogs, but should be reserved for 'GUI' containers which occur within       document content, for instance within Web documents, presentations, or        text documents.  Unlike other GUI containers and dialogs which occur inside       application instances, ROLE_FORM containers' components are associated with       the current document, rather than the current foreground application or       viewer instance.     ]]>
 * @Accessibility_ROLE_LINK: <![CDATA[     The object is a hypertext anchor, i.e. a "link" in a       hypertext document.  Such objects are distinct from 'inline'       content which may also use the Hypertext/Hyperlink interfaces       to indicate the range/location within a text object where       an inline or embedded object lies.     ]]>
 * @Accessibility_ROLE_INPUT_METHOD_WINDOW: <![CDATA[     The object is a window or similar viewport which is used       to allow composition or input of a 'complex character',       in other words it is an "input method window."     ]]>
 * @Accessibility_ROLE_TABLE_ROW: <![CDATA[A row in a table.]]>
 * @Accessibility_ROLE_TREE_ITEM: <![CDATA[An object that represents an element of a tree.]]>
 * @Accessibility_ROLE_DOCUMENT_SPREADSHEET: <![CDATA[A document frame which contains a spreadsheet.]]>
 * @Accessibility_ROLE_DOCUMENT_PRESENTATION: <![CDATA[A document frame which contains a presentation or slide content.]]>
 * @Accessibility_ROLE_DOCUMENT_TEXT: <![CDATA[A document frame which contains textual content, such as found in a word processing application.]]>
 * @Accessibility_ROLE_DOCUMENT_WEB: <![CDATA[A document frame which contains HTML or other markup suitable for display in a web browser.]]>
 * @Accessibility_ROLE_DOCUMENT_EMAIL: <![CDATA[A document frame which contains email content to be displayed or composed either in plain text or HTML.]]>
 * @Accessibility_ROLE_COMMENT: <![CDATA[An object found within a document and designed to present a comment, note, or other annotation. In some cases, this object might not be visible until activated.]]>
 * @Accessibility_ROLE_LIST_BOX: <![CDATA[A non-collapsible list of choices the user can select from.]]>
 * @Accessibility_ROLE_GROUPING: <![CDATA[A group of related widgets. This group typically has a label.]]>
 * @Accessibility_ROLE_IMAGE_MAP: <![CDATA[An image map object. Usually a graphic with multiple hotspots, where each hotspot can be activated resulting in the loading of another document or section of a document.]]>
 * @Accessibility_ROLE_NOTIFICATION: <![CDATA[A transitory object designed to present a message to the user, typically at the desktop level rather than inside a particular application.]]>
 * @Accessibility_ROLE_INFO_BAR: <![CDATA[An object designed to present a message to the user within an existing window.]]>
 * @Accessibility_ROLE_LAST_DEFINED: <![CDATA[     Not a valid role, used for finding end of enumeration.    ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    Accessibility_ROLE_INVALID,
    Accessibility_ROLE_ACCELERATOR_LABEL,
    Accessibility_ROLE_ALERT,
    Accessibility_ROLE_ANIMATION,
    Accessibility_ROLE_ARROW,
    Accessibility_ROLE_CALENDAR,
    Accessibility_ROLE_CANVAS,
    Accessibility_ROLE_CHECK_BOX,
    Accessibility_ROLE_CHECK_MENU_ITEM,
    Accessibility_ROLE_COLOR_CHOOSER,
    Accessibility_ROLE_COLUMN_HEADER,
    Accessibility_ROLE_COMBO_BOX,
    Accessibility_ROLE_DATE_EDITOR,
    Accessibility_ROLE_DESKTOP_ICON,
    Accessibility_ROLE_DESKTOP_FRAME,
    Accessibility_ROLE_DIAL,
    Accessibility_ROLE_DIALOG,
    Accessibility_ROLE_DIRECTORY_PANE,
    Accessibility_ROLE_DRAWING_AREA,
    Accessibility_ROLE_FILE_CHOOSER,
    Accessibility_ROLE_FILLER,
    Accessibility_ROLE_FOCUS_TRAVERSABLE,
    Accessibility_ROLE_FONT_CHOOSER,
    Accessibility_ROLE_FRAME,
    Accessibility_ROLE_GLASS_PANE,
    Accessibility_ROLE_HTML_CONTAINER,
    Accessibility_ROLE_ICON,
    Accessibility_ROLE_IMAGE,
    Accessibility_ROLE_INTERNAL_FRAME,
    Accessibility_ROLE_LABEL,
    Accessibility_ROLE_LAYERED_PANE,
    Accessibility_ROLE_LIST,
    Accessibility_ROLE_LIST_ITEM,
    Accessibility_ROLE_MENU,
    Accessibility_ROLE_MENU_BAR,
    Accessibility_ROLE_MENU_ITEM,
    Accessibility_ROLE_OPTION_PANE,
    Accessibility_ROLE_PAGE_TAB,
    Accessibility_ROLE_PAGE_TAB_LIST,
    Accessibility_ROLE_PANEL,
    Accessibility_ROLE_PASSWORD_TEXT,
    Accessibility_ROLE_POPUP_MENU,
    Accessibility_ROLE_PROGRESS_BAR,
    Accessibility_ROLE_PUSH_BUTTON,
    Accessibility_ROLE_RADIO_BUTTON,
    Accessibility_ROLE_RADIO_MENU_ITEM,
    Accessibility_ROLE_ROOT_PANE,
    Accessibility_ROLE_ROW_HEADER,
    Accessibility_ROLE_SCROLL_BAR,
    Accessibility_ROLE_SCROLL_PANE,
    Accessibility_ROLE_SEPARATOR,
    Accessibility_ROLE_SLIDER,
    Accessibility_ROLE_SPIN_BUTTON,
    Accessibility_ROLE_SPLIT_PANE,
    Accessibility_ROLE_STATUS_BAR,
    Accessibility_ROLE_TABLE,
    Accessibility_ROLE_TABLE_CELL,
    Accessibility_ROLE_TABLE_COLUMN_HEADER,
    Accessibility_ROLE_TABLE_ROW_HEADER,
    Accessibility_ROLE_TEAROFF_MENU_ITEM,
    Accessibility_ROLE_TERMINAL,
    Accessibility_ROLE_TEXT,
    Accessibility_ROLE_TOGGLE_BUTTON,
    Accessibility_ROLE_TOOL_BAR,
    Accessibility_ROLE_TOOL_TIP,
    Accessibility_ROLE_TREE,
    Accessibility_ROLE_TREE_TABLE,
    Accessibility_ROLE_UNKNOWN,
    Accessibility_ROLE_VIEWPORT,
    Accessibility_ROLE_WINDOW,
    Accessibility_ROLE_EXTENDED,
    Accessibility_ROLE_HEADER,
    Accessibility_ROLE_FOOTER,
    Accessibility_ROLE_PARAGRAPH,
    Accessibility_ROLE_RULER,
    Accessibility_ROLE_APPLICATION,
    Accessibility_ROLE_AUTOCOMPLETE,
    Accessibility_ROLE_EDITBAR,
    Accessibility_ROLE_EMBEDDED,
    Accessibility_ROLE_ENTRY,
    Accessibility_ROLE_CHART,
    Accessibility_ROLE_CAPTION,
    Accessibility_ROLE_DOCUMENT_FRAME,
    Accessibility_ROLE_HEADING,
    Accessibility_ROLE_PAGE,
    Accessibility_ROLE_SECTION,
    Accessibility_ROLE_REDUNDANT_OBJECT,
    Accessibility_ROLE_FORM,
    Accessibility_ROLE_LINK,
    Accessibility_ROLE_INPUT_METHOD_WINDOW,
    Accessibility_ROLE_TABLE_ROW,
    Accessibility_ROLE_TREE_ITEM,
    Accessibility_ROLE_DOCUMENT_SPREADSHEET,
    Accessibility_ROLE_DOCUMENT_PRESENTATION,
    Accessibility_ROLE_DOCUMENT_TEXT,
    Accessibility_ROLE_DOCUMENT_WEB,
    Accessibility_ROLE_DOCUMENT_EMAIL,
    Accessibility_ROLE_COMMENT,
    Accessibility_ROLE_LIST_BOX,
    Accessibility_ROLE_GROUPING,
    Accessibility_ROLE_IMAGE_MAP,
    Accessibility_ROLE_NOTIFICATION,
    Accessibility_ROLE_INFO_BAR,
    Accessibility_ROLE_LAST_DEFINED,
} Accessibility_Role;

/**
 * NUM_ACCESSIBILITY_ROLES:
 *
 * 1 higher than the highest valid value of #Accessibility_Role.
 */
#define NUM_ACCESSIBILITY_ROLES (103+1)


#ifdef __cplusplus
}
#endif
#endif
