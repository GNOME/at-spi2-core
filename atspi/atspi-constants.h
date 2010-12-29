/* TODO: Auto-generate this file again



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

#ifndef _ATSPI_CONSTANTS_H_
#define _ATSPI_CONSTANTS_H_
#ifdef __cplusplus
extern "C" {
#endif

/**
 *
ATSPI_LOCALE_TYPE:
 * @ATSPI_LOCALE_TYPE_MESSAGES: <![CDATA[]]>
 * @ATSPI_LOCALE_TYPE_COLLATE: <![CDATA[]]>
 * @ATSPI_LOCALE_TYPE_CTYPE: <![CDATA[]]>
 * @ATSPI_LOCALE_TYPE_MONETARY: <![CDATA[]]>
 * @ATSPI_LOCALE_TYPE_NUMERIC: <![CDATA[]]>
 * @ATSPI_LOCALE_TYPE_TIME: <![CDATA[]]>
 *
 * <![CDATA[   Used by Text and Document interfaces these correspond to the POSIX 'setlocale' enum values.    ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_LOCALE_TYPE_MESSAGES,
    ATSPI_LOCALE_TYPE_COLLATE,
    ATSPI_LOCALE_TYPE_CTYPE,
    ATSPI_LOCALE_TYPE_MONETARY,
    ATSPI_LOCALE_TYPE_NUMERIC,
    ATSPI_LOCALE_TYPE_TIME,
} AtspiLocaleType;

/**
 * ATSPI_LOCALE_TYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiLocaleType.
 */
#define ATSPI_LOCALE_TYPE _COUNT(5+1)

/**
 *
ATSPI_COORD_TYPE:
 * @ATSPI_COORD_TYPE_SCREEN: <![CDATA[]]>
 * @ATSPI_COORD_TYPE_WINDOW: <![CDATA[]]>
 *
 * <![CDATA[   Used by Component, Image, and Text interfaces to specify whether coordinates are relative to the window or the screen.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_COORD_TYPE_SCREEN,
    ATSPI_COORD_TYPE_WINDOW,
} AtspiCoordType;

/**
 * ATSPI_COORD_TYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiCoordType.
 */
#define ATSPI_COORD_TYPE_COUNT (1+1)

/**
 *
ATSPI_Collection_SortOrder:
 * @ATSPI_Collection_SORT_ORDER_INVALID: <![CDATA[]]>
 * @ATSPI_Collection_SORT_ORDER_CANONICAL: <![CDATA[]]>
 * @ATSPI_Collection_SORT_ORDER_FLOW: <![CDATA[]]>
 * @ATSPI_Collection_SORT_ORDER_TAB: <![CDATA[]]>
 * @ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL: <![CDATA[]]>
 * @ATSPI_Collection_SORT_ORDER_REVERSE_FLOW: <![CDATA[]]>
 * @ATSPI_Collection_SORT_ORDER_REVERSE_TAB: <![CDATA[]]>
 * @ATSPI_Collection_SORT_ORDER_LAST_DEFINED: <![CDATA[]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_Collection_SORT_ORDER_INVALID,
    ATSPI_Collection_SORT_ORDER_CANONICAL,
    ATSPI_Collection_SORT_ORDER_FLOW,
    ATSPI_Collection_SORT_ORDER_TAB,
    ATSPI_Collection_SORT_ORDER_REVERSE_CANONICAL,
    ATSPI_Collection_SORT_ORDER_REVERSE_FLOW,
    ATSPI_Collection_SORT_ORDER_REVERSE_TAB,
    ATSPI_Collection_SORT_ORDER_LAST_DEFINED,
} AtspiCollectionSortOrder;

/**
 * ATSPI_SORTORDER_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiCollectionSortOrder.
 */
#define ATSPI_SORTORDER_COUNT (7+1)

/**
 *
ATSPI_Collection_MatchType:
 * @ATSPI_Collection_MATCH_INVALID: <![CDATA[]]>
 * @ATSPI_Collection_MATCH_ALL: <![CDATA[]]>
 * @ATSPI_Collection_MATCH_ANY: <![CDATA[]]>
 * @ATSPI_Collection_MATCH_NONE: <![CDATA[]]>
 * @ATSPI_Collection_MATCH_EMPTY: <![CDATA[]]>
 * @ATSPI_Collection_MATCH_LAST_DEFINED: <![CDATA[]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_Collection_MATCH_INVALID,
    ATSPI_Collection_MATCH_ALL,
    ATSPI_Collection_MATCH_ANY,
    ATSPI_Collection_MATCH_NONE,
    ATSPI_Collection_MATCH_EMPTY,
    ATSPI_Collection_MATCH_LAST_DEFINED,
} AtspiCollectionMatchType;

/**
 * ATSPI_MATCHTYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiCollectionMatchType.
 */
#define ATSPI_MATCHTYPES_COUNT (5+1)

/**
 *
ATSPI_Collection_TreeTraversalType:
 * @ATSPI_Collection_TREE_RESTRICT_CHILDREN: <![CDATA[]]>
 * @ATSPI_Collection_TREE_RESTRICT_SIBLING: <![CDATA[]]>
 * @ATSPI_Collection_TREE_INORDER: <![CDATA[]]>
 * @ATSPI_Collection_TREE_LAST_DEFINED: <![CDATA[]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_Collection_TREE_RESTRICT_CHILDREN,
    ATSPI_Collection_TREE_RESTRICT_SIBLING,
    ATSPI_Collection_TREE_INORDER,
    ATSPI_Collection_TREE_LAST_DEFINED,
} AtspiCollectionTreeTraversalType;

/**
 * ATSPI_TREETRAVERSALTYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiCollection_TreeTraversalType.
 */
#define ATSPI_TREETRAVERSALTYPE _COUNT(3+1)

/**
 *
ATSPI_ComponentLayer:
 * @ATSPI_LAYER_INVALID: <![CDATA[     Indicates an error condition or uninitialized value.    ]]>
 * @ATSPI_LAYER_BACKGROUND: <![CDATA[     The bottom-most layer, over which everything else is painted.        The 'desktop background' is generally in this layer.    ]]>
 * @ATSPI_LAYER_CANVAS: <![CDATA[     The 'background' layer for most content renderers and UI Component        containers.    ]]>
 * @ATSPI_LAYER_WIDGET: <![CDATA[     The layer in which the majority of ordinary 'foreground' widgets reside.   ]]>
 * @ATSPI_LAYER_MDI: <![CDATA[     A special layer between LAYER_CANVAS and LAYER_WIDGET, in which the       'pseudo windows' (e.g. the MDI frames) reside.        (See Component.GetMDIZOrder)    ]]>
 * @ATSPI_LAYER_POPUP: <![CDATA[     A layer for popup window content, above LAYER_WIDGET.    ]]>
 * @ATSPI_LAYER_OVERLAY: <![CDATA[     The topmost layer.    ]]>
 * @ATSPI_LAYER_WINDOW: <![CDATA[     The layer in which a toplevel window background usually resides.    ]]>
 * @ATSPI_LAYER_LAST_DEFINED: <![CDATA[     Used only to determine the end of the enumeration.    ]]>
 *
 * <![CDATA[     The ComponentLayer of a Component instance indicates its relative stacking order       with respect to the onscreen visual representation of the UI.       ComponentLayer, in combination with Component bounds information, can be used       to compute the visibility of all or part of a component.  This is important in       programmatic determination of region-of-interest for magnification, and in       &#168;flat screen review&#168; models of the screen, as well as for other uses.       Objects residing in two of the ComponentLayer categories support       further z-ordering information, with respect to their peers in the same layer:       namely, LAYER_WINDOW and LAYER_MDI.  Relative stacking order for other objects within       the same layer is not available; the recommended heuristic is &#168;first child paints first&#168;,        in other words, assume that the first siblings in the child list are subject to being       overpainted by later siblings if their bounds intersect.        The order of layers, from bottom to top, is: 	     	    	LAYER_BACKGROUND 	    	LAYER_WINDOW 	    	LAYER_MDI 	    	LAYER_CANVAS 	    	LAYER_WIDGET 	    	LAYER_POPUP 	    	LAYER_OVERLAY 	            ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_LAYER_INVALID,
    ATSPI_LAYER_BACKGROUND,
    ATSPI_LAYER_CANVAS,
    ATSPI_LAYER_WIDGET,
    ATSPI_LAYER_MDI,
    ATSPI_LAYER_POPUP,
    ATSPI_LAYER_OVERLAY,
    ATSPI_LAYER_WINDOW,
    ATSPI_LAYER_LAST_DEFINED,
} AtspiComponentLayer;

/**
 * ATSPI_COMPONENTLAYER:_COUNT
 *
 * 1 higher than the highest valid value of #AtspiComponentLayer.
 */
#define ATSPI_COMPONENTLAYER_COUNT (8+1)

/**
 *
ATSPI_TEXT_BOUNDARY_TYPE:
 * @ATSPI_TEXT_BOUNDARY_CHAR: <![CDATA[     Text is bounded by this character only.        Start and end offsets differ by one, by definition, for this value.    ]]>
 * @ATSPI_TEXT_BOUNDARY_WORD_START: <![CDATA[     Boundary condition is start of a word; i.e. range is from start of       one word to the start of another word.    ]]>
 * @ATSPI_TEXT_BOUNDARY_WORD_END: <![CDATA[     Boundary condition is the end of a word; i.e. range is from  	    the end of one word to the end of another.     Some locales may not distinguish between words and       characters or glyphs, in particular those locales which use       wholly or partially ideographic character sets.  In these cases,       characters may be returned in lieu of multi-character substrings.   ]]>
 * @ATSPI_TEXT_BOUNDARY_SENTENCE_START: <![CDATA[     Boundary condition is start of a sentence, as determined  	    by the application.       Some locales or character sets may not include explicit sentence       delimiters, so this boundary type can not always be honored.       Some locales will return lines of text instead of grammatical sentences.   ]]>
 * @ATSPI_TEXT_BOUNDARY_SENTENCE_END: <![CDATA[     Boundary condition is end of a sentence, as determined by the application,  	    including the sentence-delimiting character, for instance '.'       Some locales or character sets may not include explicit sentence       delimiters, so this boundary type can not always be honored.       Some locales will return lines of text instead of grammatical sentences.   ]]>
 * @ATSPI_TEXT_BOUNDARY_LINE_START: <![CDATA[      Boundary condition is the start of a line; i.e. range is        from start of one line to the start of another.  This generally        means that an end-of-line character will appear at the end of the range.    ]]>
 * @ATSPI_TEXT_BOUNDARY_LINE_END: <![CDATA[      Boundary condition is the end of a line; i.e. range is       from start of one line to the start of another.  This generally        means that an end-of-line character will be the first character of the range.    ]]>
 *
 * <![CDATA[     Specifies the boundary conditions determining a run of text as returned from       GetTextAtOffset, GetTextAfterOffset, and GetTextBeforeOffset.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_TEXT_BOUNDARY_CHAR,
    ATSPI_TEXT_BOUNDARY_WORD_START,
    ATSPI_TEXT_BOUNDARY_WORD_END,
    ATSPI_TEXT_BOUNDARY_SENTENCE_START,
    ATSPI_TEXT_BOUNDARY_SENTENCE_END,
    ATSPI_TEXT_BOUNDARY_LINE_START,
    ATSPI_TEXT_BOUNDARY_LINE_END,
} AtspiTextBoundaryType;

/**
 * ATSPI_TEXT_BOUNDARY_TYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiTextBOundaryType.
 */
#define ATSPI_TEXT_BOUNDARY_TYPE_COUNT (6+1)

/**
 *
ATSPI_TEXT_CLIP_TYPE:
 * @ATSPI_TEXT_CLIP_NONE: <![CDATA[]]>
 * @ATSPI_TEXT_CLIP_MIN: <![CDATA[     Characters/glyphs clipped by the minimum coordinate are omitted    ]]>
 * @ATSPI_TEXT_CLIP_MAX: <![CDATA[     Characters/glyphs which intersect the maximum coordinate are omitted    ]]>
 * @ATSPI_TEXT_CLIP_BOTH: <![CDATA[     Only glyphs falling entirely within the region bounded by min and max are retained.    ]]>
 *
 * <![CDATA[     TEXT_CLIP_TYPE:       CLIP_MIN means text clipped by min coordinate is omitted,       CLIP_MAX clips text interescted by the max coord, and CLIP_BOTH       will retain only text falling fully within the min/max bounds.     ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_TEXT_CLIP_NONE,
    ATSPI_TEXT_CLIP_MIN,
    ATSPI_TEXT_CLIP_MAX,
    ATSPI_TEXT_CLIP_BOTH,
} AtspiTextClipType;

/**
 * ATSPI_TEXT_CLIP_TYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiTextClipType.
 */
#define ATSPI_TEXT_CLIP_TYPE_COUNT (3+1)

/**
 *
ATSPI_StateType:
 * @ATSPI_STATE_INVALID: <![CDATA[]]>
 * @ATSPI_STATE_ACTIVE: <![CDATA[     Indicates a window is currently the active window, or is an active subelement within a container or table    ]]>
 * @ATSPI_STATE_ARMED: <![CDATA[     Indicates that the object is armed    ]]>
 * @ATSPI_STATE_BUSY: <![CDATA[     Indicates the current object is busy, i.e. onscreen representation is in the process of changing, or       the object is temporarily unavailable for interaction due to activity already in progress.   ]]>
 * @ATSPI_STATE_CHECKED: <![CDATA[     Indicates this object is currently checked    ]]>
 * @ATSPI_STATE_COLLAPSED: <![CDATA[     Indicates this object is collapsed    ]]>
 * @ATSPI_STATE_DEFUNCT: <![CDATA[     Indicates that this object no longer has a valid backing widget        (for instance, if its peer object has been destroyed)    ]]>
 * @ATSPI_STATE_EDITABLE: <![CDATA[     Indicates the user can change the contents of this object    ]]>
 * @ATSPI_STATE_ENABLED: <![CDATA[     Indicates that this object is enabled, i.e. that it currently reflects some application state.        Objects that are "greyed out" may lack this state, and may lack the STATE_SENSITIVE if direct user       interaction cannot cause them to acquire STATE_ENABLED.  @see STATE_SENSITIVE.   ]]>
 * @ATSPI_STATE_EXPANDABLE: <![CDATA[     Indicates this object allows progressive disclosure of its children    ]]>
 * @ATSPI_STATE_EXPANDED: <![CDATA[     Indicates this object its expanded    ]]>
 * @ATSPI_STATE_FOCUSABLE: <![CDATA[     Indicates this object can accept keyboard focus, which means all       events resulting from typing on the keyboard will normally be passed       to it when it has focus   ]]>
 * @ATSPI_STATE_FOCUSED: <![CDATA[     Indicates this object currently has the keyboard focus    ]]>
 * @ATSPI_STATE_HAS_TOOLTIP: <![CDATA[     Indicates that the object has an associated tooltip    ]]>
 * @ATSPI_STATE_HORIZONTAL: <![CDATA[     Indicates the orientation of thsi object is horizontal    ]]>
 * @ATSPI_STATE_ICONIFIED: <![CDATA[     Indicates this object is minimized and is represented only by an icon    ]]>
 * @ATSPI_STATE_MODAL: <![CDATA[     Indicates something must be done with this object before the user can       interact with an object in a different window.   ]]>
 * @ATSPI_STATE_MULTI_LINE: <![CDATA[     Indicates this (text) object can contain multiple lines of text    ]]>
 * @ATSPI_STATE_MULTISELECTABLE: <![CDATA[     Indicates this object allows more than one of its children to be       selected at the same time, or in the case of text objects,        that the object supports non-contiguous text selections.   ]]>
 * @ATSPI_STATE_OPAQUE: <![CDATA[     Indicates this object paints every pixel within its rectangular region.       It also indicates an alpha value of unity, if it supports alpha blending.    ]]>
 * @ATSPI_STATE_PRESSED: <![CDATA[     Indicates this object is currently pressed    ]]>
 * @ATSPI_STATE_RESIZABLE: <![CDATA[     Indicates the size of this object's size is not fixed    ]]>
 * @ATSPI_STATE_SELECTABLE: <![CDATA[     Indicates this object is the child of an object that allows its       children to be selected and that this child is one of those children       that can be selected.   ]]>
 * @ATSPI_STATE_SELECTED: <![CDATA[     Indicates this object is the child of an object that allows its       children to be selected and that this child is one of those children       that has been selected.   ]]>
 * @ATSPI_STATE_SENSITIVE: <![CDATA[     Indicates this object is sensitive, e.g. to user interaction.        STATE_SENSITIVE usually accompanies STATE_ENABLED for user-actionable controls,       but may be found in the absence of STATE_ENABLED if the current visible state of the        control is "disconnected" from the application state.  In such cases, direct user interaction       can often result in the object gaining STATE_SENSITIVE, for instance if a user makes        an explicit selection using an object whose current state is ambiguous or undefined.       @see STATE_ENABLED, STATE_INDETERMINATE.    ]]>
 * @ATSPI_STATE_SHOWING: <![CDATA[     Indicates this object, the object's parent, the object's parent's       parent, and so on, are all 'shown' to the end-user, i.e.       subject to "exposure" if blocking or obscuring objects do not interpose       between this object and the top of the window stack.   ]]>
 * @ATSPI_STATE_SINGLE_LINE: <![CDATA[     Indicates this (text) object can contain only a single line of text    ]]>
 * @ATSPI_STATE_STALE: <![CDATA[     Indicates that the information returned for this object may no longer be       synchronized with the application state.  This can occur if the object has STATE_TRANSIENT,       and can also occur towards the end of the object peer's lifecycle.    ]]>
 * @ATSPI_STATE_TRANSIENT: <![CDATA[     Indicates this object is transient    ]]>
 * @ATSPI_STATE_VERTICAL: <![CDATA[     Indicates the orientation of this object is vertical; for example this state may appear on        such objects as scrollbars, text objects (with vertical text flow), separators, etc.   ]]>
 * @ATSPI_STATE_VISIBLE: <![CDATA[ 	  Indicates this object is visible, e.g. has been explicitly marked for exposure to the user. 	  STATE_VISIBLE is no guarantee that the object is actually unobscured on the screen, only       that it is 'potentially' visible, barring obstruction, being scrolled or clipped out of the        field of view, or having an ancestor container that has not yet made visible.       A widget is potentially onscreen if it has both STATE_VISIBLE and STATE_SHOWING.       The absence of STATE_VISIBLE and STATE_SHOWING is semantically equivalent to saying        that an object is 'hidden'.   ]]>
 * @ATSPI_STATE_MANAGES_DESCENDANTS: <![CDATA[     Indicates that "active-descendant-changed" event is sent when children       become 'active' (i.e. are selected or navigated to onscreen).  Used to       prevent need to enumerate all children in very large containers, like       tables.  The presence of STATE_MANAGES_DESCENDANTS is an indication to the client.       that the children should not, and need not, be enumerated by the client.       Objects implementing this state are expected to provide relevant state       notifications to listening clients, for instance notifications of visibility       changes and activation of their contained child objects, without the client        having previously requested references to those children.   ]]>
 * @ATSPI_STATE_INDETERMINATE: <![CDATA[     Indicates that a check box or other boolean indicator is in a state other than        checked or not checked.  This usually means that the boolean value reflected or        controlled by the object does not apply consistently to the entire current context.       For example, a checkbox for the "Bold" attribute of text may have STATE_INDETERMINATE       if the currently selected text contains a mixture of weight attributes.         In many cases interacting with a STATE_INDETERMINATE object will cause        the context's corresponding boolean attribute to be homogenized, whereupon the object       will lose STATE_INDETERMINATE and a corresponding state-changed event will be fired.   ]]>
 * @ATSPI_STATE_REQUIRED: <![CDATA[     Indicates that user interaction with this object is 'required' from the user,        for instance before completing the processing of a form.   ]]>
 * @ATSPI_STATE_TRUNCATED: <![CDATA[ 	  Indicates that an object's onscreen content is truncated, e.g. a text value in a spreadsheet cell.    ]]>
 * @ATSPI_STATE_ANIMATED: <![CDATA[     Indicates this object's visual representation is dynamic, not static.       This state may be applied to an object during an animated 'effect' and        be removed from the object once its visual representation becomes static.     some applications, notably content viewers, may not be able to detect       all kinds of animated content.  Therefore the absence of this state should not       be taken as definitive evidence that the object's visual representation is       static; this state is advisory.   ]]>
 * @ATSPI_STATE_INVALID_ENTRY: <![CDATA[     This object has indicated an error condition due to failure of input       validation.  For instance, a form control may acquire this state in response       to invalid or malformed user input.     ]]>
 * @ATSPI_STATE_SUPPORTS_AUTOCOMPLETION: <![CDATA[     This state indicates that the object in question implements some form of &#168;typeahead&#168; or        pre-selection behavior whereby entering the first character of one or more sub-elements       causes those elements to scroll into view or become selected.  Subsequent character input       may narrow the selection further as long as one or more sub-elements match the string.       This state is normally only useful and encountered on objects that implement Selection.       In some cases the typeahead behavior may result in full or partial &#168;completion&#168; of        the data in the input field, in which case these input events may trigger text-changed       events from the source.     ]]>
 * @ATSPI_STATE_SELECTABLE_TEXT: <![CDATA[     This state indicates that the object in question supports text selection.        It should only be exposed on objects which implement the Text interface,        in order to distinguish this state from STATE_SELECTABLE, which infers that       the object in question is a selectable child of an object which implements       Selection.  While similar, text selection and subelement selection are       distinct operations.     ]]>
 * @ATSPI_STATE_IS_DEFAULT: <![CDATA[     This state indicates that the object in question is the 'default' interaction object        in a dialog, i.e. the one that gets activated if the user presses "Enter" when the       dialog is initially posted.     ]]>
 * @ATSPI_STATE_VISITED: <![CDATA[     This state indicates that the object (typically a hyperlink)       has already been activated or invoked, with the result that some backing data       has been downloaded or rendered.     ]]>
 * @ATSPI_STATE_LAST_DEFINED: <![CDATA[     This value of the enumeration should not be used as a parameter, it indicates the number of       items in the StateType enumeration.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_STATE_INVALID,
    ATSPI_STATE_ACTIVE,
    ATSPI_STATE_ARMED,
    ATSPI_STATE_BUSY,
    ATSPI_STATE_CHECKED,
    ATSPI_STATE_COLLAPSED,
    ATSPI_STATE_DEFUNCT,
    ATSPI_STATE_EDITABLE,
    ATSPI_STATE_ENABLED,
    ATSPI_STATE_EXPANDABLE,
    ATSPI_STATE_EXPANDED,
    ATSPI_STATE_FOCUSABLE,
    ATSPI_STATE_FOCUSED,
    ATSPI_STATE_HAS_TOOLTIP,
    ATSPI_STATE_HORIZONTAL,
    ATSPI_STATE_ICONIFIED,
    ATSPI_STATE_MODAL,
    ATSPI_STATE_MULTI_LINE,
    ATSPI_STATE_MULTISELECTABLE,
    ATSPI_STATE_OPAQUE,
    ATSPI_STATE_PRESSED,
    ATSPI_STATE_RESIZABLE,
    ATSPI_STATE_SELECTABLE,
    ATSPI_STATE_SELECTED,
    ATSPI_STATE_SENSITIVE,
    ATSPI_STATE_SHOWING,
    ATSPI_STATE_SINGLE_LINE,
    ATSPI_STATE_STALE,
    ATSPI_STATE_TRANSIENT,
    ATSPI_STATE_VERTICAL,
    ATSPI_STATE_VISIBLE,
    ATSPI_STATE_MANAGES_DESCENDANTS,
    ATSPI_STATE_INDETERMINATE,
    ATSPI_STATE_REQUIRED,
    ATSPI_STATE_TRUNCATED,
    ATSPI_STATE_ANIMATED,
    ATSPI_STATE_INVALID_ENTRY,
    ATSPI_STATE_SUPPORTS_AUTOCOMPLETION,
    ATSPI_STATE_SELECTABLE_TEXT,
    ATSPI_STATE_IS_DEFAULT,
    ATSPI_STATE_VISITED,
    ATSPI_STATE_LAST_DEFINED,
} AtspiStateType;

/**
 * ATSPI_STATETYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiStateType.
 */
#define ATSPI_STATETYPE_COUNT (41+1)

/**
 *
ATSPI_KeyEventType:
 * @ATSPI_KEY_PRESSED: <![CDATA[]]>
 * @ATSPI_KEY_RELEASED: <![CDATA[]]>
 *
 * <![CDATA[     Deprecated, DO NOT USE!    ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_KEY_PRESSED,
    ATSPI_KEY_RELEASED,
} AtspiKeyEventType;

/**
 * ATSPI_KEYEVENTTYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiKeyEventType.
 */
#define ATSPI_KEYEVENTTYPE_COUNT (1+1)

/**
 *
ATSPI_EventType:
 * @ATSPI_KEY_PRESSED_EVENT: <![CDATA[     < key on a keyboard device was pressed.    ]]>
 * @ATSPI_KEY_RELEASED_EVENT: <![CDATA[     < key on a keyboard device was released.    ]]>
 * @ATSPI_BUTTON_PRESSED_EVENT: <![CDATA[     < button on a non-keyboard human interface device        (HID) was pressed    ]]>
 * @ATSPI_BUTTON_RELEASED_EVENT: <![CDATA[     < button on a non-keyboard human interface device        (HID) was pressed    ]]>
 *
 * <![CDATA[     Used to specify the event types of interest to an EventListener, or       to identify the type of an event for which notification has been sent.        @see EventTypeSeq, DeviceEvent::type   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_KEY_PRESSED_EVENT,
    ATSPI_KEY_RELEASED_EVENT,
    ATSPI_BUTTON_PRESSED_EVENT,
    ATSPI_BUTTON_RELEASED_EVENT,
} AtspiEventType;

/**
 * ATSPI_EVENTTYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiEventType.
 */
#define ATSPI_EVENTTYPE_COUNT (3+1)

/**
 *
AtspiKeySynthType:
 * @ATSPI_KEY_PRESS: <![CDATA[     emulate the pressing of a hardware keyboard key.    ]]>
 * @ATSPI_KEY_RELEASE: <![CDATA[     emulate the release of a hardware keyboard key.    ]]>
 * @ATSPI_KEY_PRESSRELEASE: <![CDATA[     a hardware keyboard key is pressed and immediately released.    ]]>
 * @ATSPI_KEY_SYM: <![CDATA[     a symbolic key event is generated, without specifying a hardware key.        @note if the keysym is not present in the current keyboard map,       the DeviceEventController instance has a limited ability to generate       such keysyms on-the-fly.  Reliability of GenerateKeyboardEvent calls       using out-of-keymap keysyms will vary from system to system, and on the       number of different out-of-keymap being generated in quick succession.       In practice this is rarely significant, since the keysyms of interest to       AT clients and keyboard emulators are usually part of the current keymap, i.e.       present on the system keyboard for the current locale (even if a physical       hardware keyboard is not connected.   ]]>
 * @ATSPI_KEY_STRING: <![CDATA[     a string is converted to its equivalent keyboard events and emitted.        If the string consists of complex character or composed characters       which are not in the current keymap, string emission is subject to the       out-of-keymap limitations described for KeySynthType::KEY_SYM.       In practice this limitation primarily effects Chinese and Japanese locales.   ]]>
 *
 * <![CDATA[     Used when synthesizing keyboard input via DeviceEventController:GenerateKeyboardEvent.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_KEY_PRESS,
    ATSPI_KEY_RELEASE,
    ATSPI_KEY_PRESSRELEASE,
    ATSPI_KEY_SYM,
    ATSPI_KEY_STRING,
} AtspiKeySynthType;

/**
 * ATSPI_KEYSYNTHTYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiKeySynthType.
 */
#define ATSPI_KEYSYNTHTYPE_COUNT (4+1)

/**
 *
ATSPI_ModifierType:
 * @ATSPI_MODIFIER_SHIFT: <![CDATA[     The left or right 'Shift' key    ]]>
 * @ATSPI_MODIFIER_SHIFTLOCK: <![CDATA[     The ShiftLock or CapsLock key    ]]>
 * @ATSPI_MODIFIER_CONTROL: <![CDATA[     'Control'/'Ctrl'    ]]>
 * @ATSPI_MODIFIER_ALT: <![CDATA[     The Alt key (as opposed to AltGr)    ]]>
 * @ATSPI_MODIFIER_META: <![CDATA[     depending on the platform this may map to 'Window', 'Function', 'Meta',       'Menu', or 'NumLock'.         Such 'Meta keys' will map to one of META, META2, META3.       On X Windows platforms these META values map to        the modifier masks Mod1Mask, Mod2Mask, Mod3Mask, e.g. an event having       ModifierType::MODIFIER_META2 means that the 'Mod2Mask' bit is       set in the corresponding XEvent.   ]]>
 * @ATSPI_MODIFIER_META2: <![CDATA[]]>
 * @ATSPI_MODIFIER_META3: <![CDATA[]]>
 * @ATSPI_MODIFIER_NUMLOCK: <![CDATA[     A symbolic meta key name that is mapped by AT-SPI to the        appropriate META value, for the convenience of the client.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_MODIFIER_SHIFT,
    ATSPI_MODIFIER_SHIFTLOCK,
    ATSPI_MODIFIER_CONTROL,
    ATSPI_MODIFIER_ALT,
    ATSPI_MODIFIER_META,
    ATSPI_MODIFIER_META2,
    ATSPI_MODIFIER_META3,
    ATSPI_MODIFIER_NUMLOCK,
} AtspiModifierType;

/**
 * ATSPI_MODIFIERTYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiModifierType.
 */
#define ATSPI_MODIFIERTYPE_COUNT (7+1)

/**
 *
ATSPI_RelationType:
 * @ATSPI_RELATION_NULL: <![CDATA[     Not a meaningful relationship; clients should not normally encounter this RelationType value.    ]]>
 * @ATSPI_RELATION_LABEL_FOR: <![CDATA[     Object is a label for one or more other objects.    ]]>
 * @ATSPI_RELATION_LABELLED_BY: <![CDATA[     Object is labelled by one or more other objects.    ]]>
 * @ATSPI_RELATION_CONTROLLER_FOR: <![CDATA[     Object is an interactive object which modifies the state, onscreen location, or other attributes       of one or more target objects.    ]]>
 * @ATSPI_RELATION_CONTROLLED_BY: <![CDATA[     Object state, position, etc. is modified/controlled by user interaction with one or        more other objects.   For instance a viewport or scroll pane may be CONTROLLED_BY scrollbars.    ]]>
 * @ATSPI_RELATION_MEMBER_OF: <![CDATA[     Object has a grouping relationship (e.g. 'same group as') to one or more other objects.     ]]>
 * @ATSPI_RELATION_TOOLTIP_FOR: <![CDATA[     Object is a tooltip associated with another object.    ]]>
 * @ATSPI_RELATION_NODE_CHILD_OF: <![CDATA[     Object is a child of the target.   ]]>
 * @ATSPI_RELATION_NODE_PARENT_OF: <![CDATA[     Object is a parent of the target.   ]]>
 * @ATSPI_RELATION_EXTENDED: <![CDATA[     Used to indicate that a relationship exists, but its type is not specified in the enumeration       and must be obtained via a call to getRelationTypeName.     ]]>
 * @ATSPI_RELATION_FLOWS_TO: <![CDATA[     Object renders content which flows logically to another object.         For instance, text in a paragraph may flow to another object which is not the        'next sibling' in the accessibility hierarchy.    ]]>
 * @ATSPI_RELATION_FLOWS_FROM: <![CDATA[     Reciprocal of RELATION_FLOWS_TO.    ]]>
 * @ATSPI_RELATION_SUBWINDOW_OF: <![CDATA[     Object is visually and semantically considered a subwindow of another object, even though       it is not the object's child.  Useful when dealing with embedded applications and other cases       where the widget hierarchy does not map cleanly to the onscreen presentation.     ]]>
 * @ATSPI_RELATION_EMBEDS: <![CDATA[     Similar to SUBWINDOW_OF, but specifically used for cross-process embedding.     ]]>
 * @ATSPI_RELATION_EMBEDDED_BY: <![CDATA[     Reciprocal of RELATION_EMBEDS; Used to denote content rendered by embedded renderers that       live in a separate process space from the embedding context.     ]]>
 * @ATSPI_RELATION_POPUP_FOR: <![CDATA[     Denotes that the object is a transient window or frame associated with another onscreen object.       Similar to TOOLTIP_FOR, but more general.  Useful for windows which are technically        toplevels but which, for one or more reasons, do not explicitly cause their associated       window to lose 'window focus'. Creation of a ROLE_WINDOW object with the POPUP_FOR relation       usually requires some presentation action on the part of assistive technology clients, even though       the previous toplevel ROLE_FRAME object may still be the active window.     ]]>
 * @ATSPI_RELATION_PARENT_WINDOW_OF: <![CDATA[     This is the reciprocal relation to RELATION_POPUP_FOR.    ]]>
 * @ATSPI_RELATION_DESCRIPTION_FOR: <![CDATA[     Indicates that an object provides descriptive information        about another object; more verbose than RELATION_LABEL_FOR.    ]]>
 * @ATSPI_RELATION_DESCRIBED_BY: <![CDATA[     Indicates that another object provides descriptive information        about this object; more verbose than RELATION_LABELLED_BY.    ]]>
 * @ATSPI_RELATION_LAST_DEFINED: <![CDATA[     Do not use as a parameter value, used to determine the size of the enumeration.    ]]>
 *
 * <![CDATA[     RelationType specifies a relationship between objects (possibly one-to-many or many-to-one)       outside of the normal parent/child hierarchical relationship.  It allows better semantic       identification of how objects are associated with one another.       For instance the RELATION_LABELLED_BY relationship may be used to identify labelling information       that should accompany the accessibleName property when presenting an object's content or identity       to the end user.  Similarly, RELATION_CONTROLLER_FOR can be used to further specify the context       in which a valuator is useful, and/or the other UI components which are directly effected by       user interactions with the valuator. Common examples include association of scrollbars with       the viewport or panel which they control.   ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_RELATION_NULL,
    ATSPI_RELATION_LABEL_FOR,
    ATSPI_RELATION_LABELLED_BY,
    ATSPI_RELATION_CONTROLLER_FOR,
    ATSPI_RELATION_CONTROLLED_BY,
    ATSPI_RELATION_MEMBER_OF,
    ATSPI_RELATION_TOOLTIP_FOR,
    ATSPI_RELATION_NODE_CHILD_OF,
    ATSPI_RELATION_NODE_PARENT_OF,
    ATSPI_RELATION_EXTENDED,
    ATSPI_RELATION_FLOWS_TO,
    ATSPI_RELATION_FLOWS_FROM,
    ATSPI_RELATION_SUBWINDOW_OF,
    ATSPI_RELATION_EMBEDS,
    ATSPI_RELATION_EMBEDDED_BY,
    ATSPI_RELATION_POPUP_FOR,
    ATSPI_RELATION_PARENT_WINDOW_OF,
    ATSPI_RELATION_DESCRIPTION_FOR,
    ATSPI_RELATION_DESCRIBED_BY,
    ATSPI_RELATION_LAST_DEFINED,
} AtspiRelationType;

/**
 * ATSPI_RELATIONTYPE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiRelationType.
 */
#define ATSPI_RELATIONTYPE_COUNT (19+1)

/**
 *
ATSPI_Role:
 * @ATSPI_ROLE_INVALID: <![CDATA[     A Role indicating an error condition, such as uninitialized Role data.    ]]>
 * @ATSPI_ROLE_ACCELERATOR_LABEL: <![CDATA[     Object is a label indicating the keyboard accelerators for the parent     ]]>
 * @ATSPI_ROLE_ALERT: <![CDATA[     Object is used to alert the user about something    ]]>
 * @ATSPI_ROLE_ANIMATION: <![CDATA[     Object contains a dynamic or moving image of some kind    ]]>
 * @ATSPI_ROLE_ARROW: <![CDATA[     Object is a 2d directional indicator    ]]>
 * @ATSPI_ROLE_CALENDAR: <![CDATA[     Object contains one or more dates, usually arranged into a 2d list    ]]>
 * @ATSPI_ROLE_CANVAS: <![CDATA[     Object that can be drawn into and is used to trap events    ]]>
 * @ATSPI_ROLE_CHECK_BOX: <![CDATA[     A choice that can be checked or unchecked and provides a separate       indicator for the current state.   ]]>
 * @ATSPI_ROLE_CHECK_MENU_ITEM: <![CDATA[     A menu item that behaves like a check box (see ROLE_CHECK_BOX)    ]]>
 * @ATSPI_ROLE_COLOR_CHOOSER: <![CDATA[     A specialized dialog that lets the user choose a color.    ]]>
 * @ATSPI_ROLE_COLUMN_HEADER: <![CDATA[     The header for a column of data    ]]>
 * @ATSPI_ROLE_COMBO_BOX: <![CDATA[     A list of choices the user can select from    ]]>
 * @ATSPI_ROLE_DATE_EDITOR: <![CDATA[     An object which allows entry of a date    ]]>
 * @ATSPI_ROLE_DESKTOP_ICON: <![CDATA[     An inconifed internal frame within a DESKTOP_PANE    ]]>
 * @ATSPI_ROLE_DESKTOP_FRAME: <![CDATA[     A pane that supports internal frames and iconified versions of those       internal frames.   ]]>
 * @ATSPI_ROLE_DIAL: <![CDATA[     An object that allows a value to be changed via rotating a visual element,       or which displays a value via such a rotating element.    ]]>
 * @ATSPI_ROLE_DIALOG: <![CDATA[     A top level window with title bar and a border    ]]>
 * @ATSPI_ROLE_DIRECTORY_PANE: <![CDATA[     A pane that allows the user to navigate through and select the contents       of a directory   ]]>
 * @ATSPI_ROLE_DRAWING_AREA: <![CDATA[     A specialized dialog that displays the files in the directory and lets       the user select a file, browse a different directory, or specify a       filename.   ]]>
 * @ATSPI_ROLE_FILE_CHOOSER: <![CDATA[     An object used for drawing custom user interface elements.   ]]>
 * @ATSPI_ROLE_FILLER: <![CDATA[     A object that fills up space in a user interface   ]]>
 * @ATSPI_ROLE_FOCUS_TRAVERSABLE: <![CDATA[     Don't use, reserved for future use.    ]]>
 * @ATSPI_ROLE_FONT_CHOOSER: <![CDATA[     Allows selection of a display font    ]]>
 * @ATSPI_ROLE_FRAME: <![CDATA[     A top level window with a title bar, border, menubar, etc.    ]]>
 * @ATSPI_ROLE_GLASS_PANE: <![CDATA[     A pane that is guaranteed to be painted on top of all panes beneath it    ]]>
 * @ATSPI_ROLE_HTML_CONTAINER: <![CDATA[     A document container for HTML, whose children       represent the document content.   ]]>
 * @ATSPI_ROLE_ICON: <![CDATA[     A small fixed size picture, typically used to decorate components    ]]>
 * @ATSPI_ROLE_IMAGE: <![CDATA[     An image, typically static.    ]]>
 * @ATSPI_ROLE_INTERNAL_FRAME: <![CDATA[     A frame-like object that is clipped by a desktop pane.    ]]>
 * @ATSPI_ROLE_LABEL: <![CDATA[     An object used to present an icon or short string in an interface    ]]>
 * @ATSPI_ROLE_LAYERED_PANE: <![CDATA[     A specialized pane that allows its children to be drawn in layers,       providing a form of stacking order.   ]]>
 * @ATSPI_ROLE_LIST: <![CDATA[     An object that presents a list of objects to the user and allows the       user to select one or more of them.   ]]>
 * @ATSPI_ROLE_LIST_ITEM: <![CDATA[     An object that represents an element of a list.    ]]>
 * @ATSPI_ROLE_MENU: <![CDATA[     An object usually found inside a menu bar that contains a list of       actions the user can choose from.   ]]>
 * @ATSPI_ROLE_MENU_BAR: <![CDATA[     An object usually drawn at the top of the primary dialog box of an       application that contains a list of menus the user can choose from.   ]]>
 * @ATSPI_ROLE_MENU_ITEM: <![CDATA[     An object usually contained in a menu that presents an action the       user can choose.   ]]>
 * @ATSPI_ROLE_OPTION_PANE: <![CDATA[     A specialized pane whose primary use is inside a DIALOG    ]]>
 * @ATSPI_ROLE_PAGE_TAB: <![CDATA[     An object that is a child of a page tab list    ]]>
 * @ATSPI_ROLE_PAGE_TAB_LIST: <![CDATA[     An object that presents a series of panels (or page tabs), one at a time,       through some mechanism provided by the object.   ]]>
 * @ATSPI_ROLE_PANEL: <![CDATA[     A generic container that is often used to group objects.    ]]>
 * @ATSPI_ROLE_PASSWORD_TEXT: <![CDATA[     A text object uses for passwords, or other places where the text       content is not shown visibly to the user.   ]]>
 * @ATSPI_ROLE_POPUP_MENU: <![CDATA[     A temporary window that is usually used to offer the user a list of       choices, and then hides when the user selects one of those choices.   ]]>
 * @ATSPI_ROLE_PROGRESS_BAR: <![CDATA[     An object used to indicate how much of a task has been completed.    ]]>
 * @ATSPI_ROLE_PUSH_BUTTON: <![CDATA[     An object the user can manipulate to tell the application to do       something.   ]]>
 * @ATSPI_ROLE_RADIO_BUTTON: <![CDATA[     A specialized check box that will cause other radio buttons in the       same group to become uncghecked when this one is checked.   ]]>
 * @ATSPI_ROLE_RADIO_MENU_ITEM: <![CDATA[     Object is both a menu item and a "radio button" (see ROLE_RADIO_BUTTON)    ]]>
 * @ATSPI_ROLE_ROOT_PANE: <![CDATA[     A specialized pane that has a glass pane and a layered pane as its       children.   ]]>
 * @ATSPI_ROLE_ROW_HEADER: <![CDATA[     The header for a row of data    ]]>
 * @ATSPI_ROLE_SCROLL_BAR: <![CDATA[     An object usually used to allow a user to incrementally view a large       amount of data by moving the bounds of a viewport along a one-dimensional axis.   ]]>
 * @ATSPI_ROLE_SCROLL_PANE: <![CDATA[     An object that allows a user to incrementally view a large amount       of information.  ROLE_SCROLL_PANE objects are usually accompanied by       ROLE_SCROLL_BAR controllers, on which the RELATION_CONTROLLER_FOR and       RELATION_CONTROLLED_BY reciprocal relations are set; \see        Accessibility::RelationSet.   ]]>
 * @ATSPI_ROLE_SEPARATOR: <![CDATA[     An object usually contained in a menu to provide a visible and       logical separation of the contents in a menu.   ]]>
 * @ATSPI_ROLE_SLIDER: <![CDATA[     An object that allows the user to select from a bounded range    ]]>
 * @ATSPI_ROLE_SPIN_BUTTON: <![CDATA[     An object which allows one of a set of choices to be selected,        and which displays the current choice.  Unlike ROLE_SCROLL_BAR,       ROLE_SLIDER objects need not control 'viewport'-like objects.   ]]>
 * @ATSPI_ROLE_SPLIT_PANE: <![CDATA[     A specialized panel that presents two other panels at the same time.    ]]>
 * @ATSPI_ROLE_STATUS_BAR: <![CDATA[     Object displays non-quantitative status information (c.f. ROLE_PROGRESS_BAR)    ]]>
 * @ATSPI_ROLE_TABLE: <![CDATA[     An object used to repesent information in terms of rows and columns.    ]]>
 * @ATSPI_ROLE_TABLE_CELL: <![CDATA[     A 'cell' or discrete child within a Table. \note Table cells need not have ROLE_TABLE_CELL,        other RoleType values are valid as well.     ]]>
 * @ATSPI_ROLE_TABLE_COLUMN_HEADER: <![CDATA[     An object which labels a particular column in a Table.    ]]>
 * @ATSPI_ROLE_TABLE_ROW_HEADER: <![CDATA[     An object which labels a particular row in a Table. Table rows and columns may also be        labelled via the RELATION_LABEL_FOR/RELATION_LABELLED_BY relationships;        see Accessibility.RelationSet.    ]]>
 * @ATSPI_ROLE_TEAROFF_MENU_ITEM: <![CDATA[     Object allows menu to be removed from menubar and shown in its own window.    ]]>
 * @ATSPI_ROLE_TERMINAL: <![CDATA[     An object that emulates a terminal    ]]>
 * @ATSPI_ROLE_TEXT: <![CDATA[     An object that presents text to the user, of nonspecific type.    ]]>
 * @ATSPI_ROLE_TOGGLE_BUTTON: <![CDATA[     A specialized push button that can be checked or unchecked, but does       not procide a separate indicator for the current state.   ]]>
 * @ATSPI_ROLE_TOOL_BAR: <![CDATA[     A bar or palette usually composed of push buttons or toggle buttons   ]]>
 * @ATSPI_ROLE_TOOL_TIP: <![CDATA[     An object that provides information about another object   ]]>
 * @ATSPI_ROLE_TREE: <![CDATA[     An object used to repsent hierarchical information to the user.    ]]>
 * @ATSPI_ROLE_TREE_TABLE: <![CDATA[     An object that presents both tabular and hierarchical info to the user    ]]>
 * @ATSPI_ROLE_UNKNOWN: <![CDATA[     The object contains some Accessible information, but its role is       not known.     ]]>
 * @ATSPI_ROLE_VIEWPORT: <![CDATA[     An object usually used in a scroll pane, or to otherwise clip a larger object or        content renderer to a specific onscreen viewport.    ]]>
 * @ATSPI_ROLE_WINDOW: <![CDATA[     A &#168;top level window&#168; with no title or border.    ]]>
 * @ATSPI_ROLE_EXTENDED: <![CDATA[     means that the role for this item is known, but not included in the        core enumeration    ]]>
 * @ATSPI_ROLE_HEADER: <![CDATA[     An object that serves as a document header.    ]]>
 * @ATSPI_ROLE_FOOTER: <![CDATA[     An object that serves as a document footer.    ]]>
 * @ATSPI_ROLE_PARAGRAPH: <![CDATA[     An object which is contains a single paragraph of text content. See also ROLE_TEXT.    ]]>
 * @ATSPI_ROLE_RULER: <![CDATA[     An object which describes margins and tab stops, etc.        for text objects which it controls        (should have CONTROLLER_FOR relation to such).    ]]>
 * @ATSPI_ROLE_APPLICATION: <![CDATA[     An object corresponding to the toplevel accessible of an        application, which may contain ROLE_FRAME objects or other       accessible objects.  Children of AccessibleDesktop objects        are generally ROLE_APPLICATION objects.   ]]>
 * @ATSPI_ROLE_AUTOCOMPLETE: <![CDATA[     The object is a dialog or list containing items for insertion        into an entry widget, for instance a list of words for completion        of a text entry.   ]]>
 * @ATSPI_ROLE_EDITBAR: <![CDATA[     The object is an editable text object in a toolbar.   ]]>
 * @ATSPI_ROLE_EMBEDDED: <![CDATA[     The object is an embedded component container.  This role is a        "grouping" hint that the contained objects share a context which is        different from the container in which this accessible is embedded.       In particular, it is used for some kinds of document embedding, and       for embedding of out-of-process component, "panel applets", etc.   ]]>
 * @ATSPI_ROLE_ENTRY: <![CDATA[     The object is a component whose textual content may be entered or modified by the user, 	    provided STATE_EDITABLE is present.       A readonly ROLE_ENTRY object (i.e. where STATE_EDITABLE is not present) implies a       read-only 'text field' in a form, as opposed to a title, label, or caption.     ]]>
 * @ATSPI_ROLE_CHART: <![CDATA[     The object is a graphical depiction of quantitative data.  It may contain multiple       subelements whose attributes and/or description may be queried to obtain both the       quantitative data and information about how the data is being presented.       The LABELLED_BY relation is particularly important in interpreting objects of this type,       as is the accessible-description property.         See ROLE_CAPTION     ]]>
 * @ATSPI_ROLE_CAPTION: <![CDATA[     The object contains descriptive information, usually textual, about another user interface       element such as a table, chart, or image.     ]]>
 * @ATSPI_ROLE_DOCUMENT_FRAME: <![CDATA[     The object is a visual frame or container which contains a view of document content.         Document frames may occur within another Document instance, in which case the second        document may be said to be embedded in the containing instance.  HTML frames are       often ROLE_DOCUMENT_FRAME.  Either this object, or a singleton descendant, should implement       the Document interface.     ]]>
 * @ATSPI_ROLE_HEADING: <![CDATA[     The object serves as a heading for content which follows it in a document.       The 'heading level' of the heading, if availabe,  may be obtained by       querying the object's attributes.      ]]>
 * @ATSPI_ROLE_PAGE: <![CDATA[     The object is a containing instance which encapsulates a page of        information.  ROLE_PAGE is used in documents and content which support       a paginated navigation model.     ]]>
 * @ATSPI_ROLE_SECTION: <![CDATA[     The object is a containing instance of document content which constitutes       a particular 'logical' section of the document.  The type of content within       a section, and the nature of the section division itself, may be obtained       by querying the object's attributes.  Sections may be nested.     ]]>
 * @ATSPI_ROLE_REDUNDANT_OBJECT: <![CDATA[     The object is redundant with another object in the hierarchy,        and is exposed for purely technical reasons.  Objects of this role        should be ignored by clients, if they are encountered at all.      ]]>
 * @ATSPI_ROLE_FORM: <![CDATA[     The object is a containing instance of document content which       has within it components with which the user can interact in order to       input information; i.e. the object is a container for pushbuttons,       comboboxes, text input fields, and other 'GUI' components.       ROLE_FORM should not, in general, be used for toplevel GUI containers       or dialogs, but should be reserved for 'GUI' containers which occur within       document content, for instance within Web documents, presentations, or        text documents.  Unlike other GUI containers and dialogs which occur inside       application instances, ROLE_FORM containers' components are associated with       the current document, rather than the current foreground application or       viewer instance.     ]]>
 * @ATSPI_ROLE_LINK: <![CDATA[     The object is a hypertext anchor, i.e. a "link" in a       hypertext document.  Such objects are distinct from 'inline'       content which may also use the Hypertext/Hyperlink interfaces       to indicate the range/location within a text object where       an inline or embedded object lies.     ]]>
 * @ATSPI_ROLE_INPUT_METHOD_WINDOW: <![CDATA[     The object is a window or similar viewport which is used       to allow composition or input of a 'complex character',       in other words it is an "input method window."     ]]>
 * @ATSPI_ROLE_LAST_DEFINED: <![CDATA[     Not a valid role, used for finding end of enumeration.    ]]>
 *
 * Bitfield/set of flags generated from the AT-SPI specification.
 */
typedef enum {
    ATSPI_ROLE_INVALID,
    ATSPI_ROLE_ACCELERATOR_LABEL,
    ATSPI_ROLE_ALERT,
    ATSPI_ROLE_ANIMATION,
    ATSPI_ROLE_ARROW,
    ATSPI_ROLE_CALENDAR,
    ATSPI_ROLE_CANVAS,
    ATSPI_ROLE_CHECK_BOX,
    ATSPI_ROLE_CHECK_MENU_ITEM,
    ATSPI_ROLE_COLOR_CHOOSER,
    ATSPI_ROLE_COLUMN_HEADER,
    ATSPI_ROLE_COMBO_BOX,
    ATSPI_ROLE_DATE_EDITOR,
    ATSPI_ROLE_DESKTOP_ICON,
    ATSPI_ROLE_DESKTOP_FRAME,
    ATSPI_ROLE_DIAL,
    ATSPI_ROLE_DIALOG,
    ATSPI_ROLE_DIRECTORY_PANE,
    ATSPI_ROLE_DRAWING_AREA,
    ATSPI_ROLE_FILE_CHOOSER,
    ATSPI_ROLE_FILLER,
    ATSPI_ROLE_FOCUS_TRAVERSABLE,
    ATSPI_ROLE_FONT_CHOOSER,
    ATSPI_ROLE_FRAME,
    ATSPI_ROLE_GLASS_PANE,
    ATSPI_ROLE_HTML_CONTAINER,
    ATSPI_ROLE_ICON,
    ATSPI_ROLE_IMAGE,
    ATSPI_ROLE_INTERNAL_FRAME,
    ATSPI_ROLE_LABEL,
    ATSPI_ROLE_LAYERED_PANE,
    ATSPI_ROLE_LIST,
    ATSPI_ROLE_LIST_ITEM,
    ATSPI_ROLE_MENU,
    ATSPI_ROLE_MENU_BAR,
    ATSPI_ROLE_MENU_ITEM,
    ATSPI_ROLE_OPTION_PANE,
    ATSPI_ROLE_PAGE_TAB,
    ATSPI_ROLE_PAGE_TAB_LIST,
    ATSPI_ROLE_PANEL,
    ATSPI_ROLE_PASSWORD_TEXT,
    ATSPI_ROLE_POPUP_MENU,
    ATSPI_ROLE_PROGRESS_BAR,
    ATSPI_ROLE_PUSH_BUTTON,
    ATSPI_ROLE_RADIO_BUTTON,
    ATSPI_ROLE_RADIO_MENU_ITEM,
    ATSPI_ROLE_ROOT_PANE,
    ATSPI_ROLE_ROW_HEADER,
    ATSPI_ROLE_SCROLL_BAR,
    ATSPI_ROLE_SCROLL_PANE,
    ATSPI_ROLE_SEPARATOR,
    ATSPI_ROLE_SLIDER,
    ATSPI_ROLE_SPIN_BUTTON,
    ATSPI_ROLE_SPLIT_PANE,
    ATSPI_ROLE_STATUS_BAR,
    ATSPI_ROLE_TABLE,
    ATSPI_ROLE_TABLE_CELL,
    ATSPI_ROLE_TABLE_COLUMN_HEADER,
    ATSPI_ROLE_TABLE_ROW_HEADER,
    ATSPI_ROLE_TEAROFF_MENU_ITEM,
    ATSPI_ROLE_TERMINAL,
    ATSPI_ROLE_TEXT,
    ATSPI_ROLE_TOGGLE_BUTTON,
    ATSPI_ROLE_TOOL_BAR,
    ATSPI_ROLE_TOOL_TIP,
    ATSPI_ROLE_TREE,
    ATSPI_ROLE_TREE_TABLE,
    ATSPI_ROLE_UNKNOWN,
    ATSPI_ROLE_VIEWPORT,
    ATSPI_ROLE_WINDOW,
    ATSPI_ROLE_EXTENDED,
    ATSPI_ROLE_HEADER,
    ATSPI_ROLE_FOOTER,
    ATSPI_ROLE_PARAGRAPH,
    ATSPI_ROLE_RULER,
    ATSPI_ROLE_APPLICATION,
    ATSPI_ROLE_AUTOCOMPLETE,
    ATSPI_ROLE_EDITBAR,
    ATSPI_ROLE_EMBEDDED,
    ATSPI_ROLE_ENTRY,
    ATSPI_ROLE_CHART,
    ATSPI_ROLE_CAPTION,
    ATSPI_ROLE_DOCUMENT_FRAME,
    ATSPI_ROLE_HEADING,
    ATSPI_ROLE_PAGE,
    ATSPI_ROLE_SECTION,
    ATSPI_ROLE_REDUNDANT_OBJECT,
    ATSPI_ROLE_FORM,
    ATSPI_ROLE_LINK,
    ATSPI_ROLE_INPUT_METHOD_WINDOW,
    ATSPI_ROLE_LAST_DEFINED,
} AtspiRole;

/**
 * ATSPI_ROLE_COUNT:
 *
 * 1 higher than the highest valid value of #AtspiRole.
 */
#define ATSPI_ROLE_COUNT (90+1)


#ifdef __cplusplus
}
#endif
#endif	/* _ATSPI_CONSTANTS_H_ */
