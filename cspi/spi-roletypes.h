/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _SPI_ROLETYPES_H_
#define _SPI_ROLETYPES_H_

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * AccessibleRole:
 * @SPI_ROLE_INVALID: role is not legal, something is wrong with this object
 * @SPI_ROLE_ACCEL_LABEL: object is a label indicating keyboard
 *                              accelerators for the parent
 * @SPI_ROLE_ALERT: Object is used to alert the user about something
 * @SPI_ROLE_ANIMATION: Object contains a dynamic or moving image
 * @SPI_ROLE_ARROW: Object is a 2d directional indicator
 * @SPI_ROLE_CALENDAR: Object contains one or more dates, usually arranged into
 *                     a 2d list
 * @SPI_ROLE_CANVAS: Object that can be drawn into and is used to trap events
 * @SPI_ROLE_CHECK_BOX: Object representd a choice that can be checked or unchecked
 *                    and provides a separate indicator for the current state.
 * @SPI_ROLE_CHECK_MENU_ITEM: Object is a menu item that behaves like a CHECK_BOX
 * @SPI_ROLE_COLOR_CHOOSER: A specialized dialog that lets the user choose a color.
 * @SPI_ROLE_COLUMN_HEADER: The header for a column of data 
 * @SPI_ROLE_COMBO_BOX: A list of choices the user can select from
 * @SPI_ROLE_DATE_EDITOR: Allows entry of a date
 * @SPI_ROLE_DESKTOP_ICON: An inconifed internal frame within a DESKTOP_PANE
 * @SPI_ROLE_DESKTOP_FRAME: A pane that supports internal frames and
 *              iconified versions of those internal frames.
 * @SPI_ROLE_DIAL: A rotating/rotatable valuator
 * @SPI_ROLE_DIALOG: A top level window with title bar and a border
 * @SPI_ROLE_DIRECTORY_PANE: A pane that allows the user to navigate
 *              through and select the contents of a directory.
 * @SPI_ROLE_DRAWING_AREA: An object used for drawing custom user interface elements
 * @SPI_ROLE_FILE_CHOOSER: A specialized dialog that displays the files in the
 *               directory and lets the user select a file, browse a different
 *	 	 directory, or specify a filename.
 * @SPI_ROLE_FILLER: A object that fills up space in a user interface
 * @SPI_ROLE_FONT_CHOOSER: Allows selection of a display font
 * @SPI_ROLE_FRAME: A top level window with a title bar, border, menubar, etc.
 * @SPI_ROLE_GLASS_PANE: A pane that is guaranteed to be painted on top of all
 *               panes beneath it
 * @SPI_ROLE_HTML_CONTAINER: A document container for HTML, whose children
 *               represent the document content.
 * @SPI_ROLE_ICON: A small fixed size picture, typically used to decorate components.
 * @SPI_ROLE_IMAGE: An image, typically static
 * @SPI_ROLE_INTERNAL_FRAME: A frame-like object that is clipped by a desktop pane.
 * @SPI_ROLE_LABEL: An object used to present an icon or short string in an interface
 * @SPI_ROLE_LAYERED_PANE: A specialized pane that allows its children to be drawn in layers
 *                         providing a  form of stacking order.
 * @SPI_ROLE_LIST: An object that presents a list of objects to the user and allows the
 *                         user to select one or more of them.
 * @SPI_ROLE_LIST_ITEM: An object that represents an element of a list.
 * @SPI_ROLE_MENU: An object usually found inside a menu bar that contains a list of
 *                 actions the user can choose from.
 * @SPI_ROLE_MENU_BAR: An object usually drawn at the top of the primary dialog box of an
 *                 application that contains a list of menus the user can choose from.
 * @SPI_ROLE_MENU_ITEM: An object usually contained in a menu that presents an
 *	           action the user can choose.
 * @SPI_ROLE_OPTION_PANE: A specialized pane whose primary use is inside a DIALOG 
 * @SPI_ROLE_PAGE_TAB: An object that is a child of a page tab list
 * @SPI_ROLE_PAGE_TAB_LIST: An object that presents a series of panels (or page tabs),
 *	  one at a time, through some mechanism provided by the object.
 * @SPI_ROLE_PANEL: A generic container that is often used to group objects.
 * @SPI_ROLE_PASSWORD_TEXT: A text object uses for passwords, or other places where
 *        the text content is not shown visibly to the user.
 * @SPI_ROLE_POPUP_MENU: A temporary window that is usually used to offer the user a
 *        list of choices, and then hides when the user selects one of those choices.
 * @SPI_ROLE_PROGRESS_BAR: An object used to indicate how much of a task has been completed.
 * @SPI_ROLE_PUSH_BUTTON: A specialized object which the user can interact with to tell
 *        the application to do something.
 * @SPI_ROLE_RADIO_BUTTON: A specialized check box that will cause other radio buttons in the
 *        same group to become uncghecked when this one is checked.
 * @SPI_ROLE_RADIO_MENU_ITEM: A specialized menu item that behaves like a 
 *        radio button.
 * @SPI_ROLE_ROOT_PANE: A specialized pane that has a glass pane and a layered pane as its
 *        children.
 * @SPI_ROLE_ROW_HEADER: The header for a row of data
 * @SPI_ROLE_SCROLL_BAR: An object usually used to allow a user to incrementally
 *        view a large amount of data.
 * @SPI_ROLE_SCROLL_PANE: An object that allows a user to incrementally view a large amount
 *         of information.
 * @SPI_ROLE_SEPARATOR: An object usually contained in a menu to provide a visible and
 *        logical separation of the contents in a menu.
 * @SPI_ROLE_SLIDER: An object that allows the user to select from a bounded range.
 * @SPI_ROLE_SPIN_BUTTON: An object which allows selection from a set of choices and
 *                        displays the current choice.
 * @SPI_ROLE_SPLIT_PANE: A specialized panel that presents two other panels at the same time.
 * @SPI_ROLE_STATUS_BAR: An object the display qualitative status information. c.f. PROGRESS_BAR
 * @SPI_ROLE_TABLE: An object used to rpesent information in terms of rows and columns.
 * @SPI_ROLE_TABLE_CELL: An object which is a descendant of a table,
 *        with a row/column location.  A cell may span multiple rows and columns.
 * @SPI_ROLE_TABLE_COLUMN_HEADER: An object which serves to describe a column in a table.
 * @SPI_ROLE_TABLE_ROW_HEADER: An object which serves to label or describe a row in a table.
 * @SPI_ROLE_TEAROFF_MENU_ITEM: A menu item which allows the menu to be removed from
 *                              a menubar and placed in its own window
 * @SPI_ROLE_TERMINAL: An object that emulates a teletype or terminal
 * @SPI_ROLE_TEXT: An object that presents text to the user
 * @SPI_ROLE_TOGGLE_BUTTON: A specialized push button that can be checked or unchecked,
 *	  but does not provide a separate indicator for the current state.
 * @SPI_ROLE_TOOL_BAR: A bar or palette usually composed of push buttons or toggle buttons
 * @SPI_ROLE_TOOL_TIP: An object that provides information about another object
 * @SPI_ROLE_TREE: An object used to represent hierarchical information to the user
 * @SPI_ROLE_TREE_TABLE: An object which represents both hierarchical and tabular information
 * @SPI_ROLE_UNKNOWN: The object contains some SpiAccessible information, but its role is
 *                    not known.
 * @SPI_ROLE_VIEWPORT: An object usually used in a scroll pane, which restricts the visual
 *                    area into which its contents are presented.
 * @SPI_ROLE_WINDOW: A top level window with no title or border.
 * @SPI_ROLE_EXTENDED: This object's role is not included in the standard role list and
 *                     should be queried by name.
 * @SPI_ROLE_HEADER: An object that serves as a document header.
 * @SPI_ROLE_FOOTER: An object that serves as a document footer.
 * @SPI_ROLE_PARAGRAPH: An object which is contains a paragraph of text content.
 * @SPI_ROLE_RULER: An object which describes margins and tab stops, etc.  for text objects 
 *                  which it controls (should have CONTROLLER_FOR relation to such). 
 * @SPI_ROLE_APPLICATION: An object which corresponds to a desktop application,
 *                  which may have children of @SPI_ROLE_FRAME or other type.
 * @SPI_ROLE_AUTOCOMPLETE: The object is a dialog or list containing items for 
 *                         insertion into an entry widget, for instance a list 
 *                          of words for completion of a text entry.
 * @SPI_ROLE_EDITBAR: The object is an editable text object in a toolbar
 * @SPI_ROLE_EMBEDDED: The object is an embedded component container.  
 *                     This role is a  "grouping" hint that the contained 
 *                     objects share a context which is different from the 
 *                     container in which this accessible is embedded.
 *                     Uses: document embedding, "panel applets", etc.
 * @SPI_ROLE_ENTRY: The object is a component whose textual content may be entered or modified by the user, provided @SPI_STATE_EDITABLE is present.
 * @SPI_ROLE_CHART: The object is a graphical depiction of quantitative data. It may contain multiple subelements whose attributes and/or description may be queried to obtain both the quantitative data and information about how the data is being presented. The LABELLED_BY relation is particularly important in interpreting objects of this type, as is the accessible-description property.
 * @SPI_ROLE_CAPTION: The object contains descriptive information, usually textual, about another user interface element such as a table, chart, or image.
 * @SPI_ROLE_DOCUMENT_FRAME: The object is a visual frame or container which contains a view of document content. Document frames may occur within another Document instance, in which case the second document may be said to be embedded in the containing instance. HTML frames are often ROLE_DOCUMENT_FRAME. Either this object, or a singleton descendant, should implement the Document interface.
 * @SPI_ROLE_HEADING: The object serves as a heading for content which follows it in a document. The 'heading level' of the heading, if availabe, may be obtained by querying the object's attributes.
 * @SPI_ROLE_PAGE: The object is a containing instance which encapsulates a page of information. @SPI_ROLE_PAGE is used in documents and content which support a paginated navigation model.
 * @SPI_ROLE_SECTION: The object is a containing instance of document content which constitutes a particular 'logical' section of the document. The type of content within a section, and the nature of the section division itself, may be obtained by querying the object's attributes. Sections may be nested.
 * @SPI_ROLE_REDUNDANT_OBJECT: The object is redundant with another object in the hierarchy, and is exposed for purely technical reasons.  Objects of this role should normally be ignored by clients. 
 * @SPI_ROLE_FORM: The object is a container for form controls, for instance as part of a 
 * web form or user-input form within a document.  This role is primarily a tag/convenience for 
 * clients when navigating complex documents, it is not expected that ordinary GUI containers will 
 * always have ATK_ROLE_FORM.
 * @SPI_ROLE_LINK: The object is a hypertext anchor.
 * @SPI_ROLE_INPUT_METHOD_WINDOW: The object is an input method window used
 * to input or compose a text character (typically a "complex text" character).
 * @SPI_ROLE_LAST_DEFINED: Used to determine the end of the role enumeration.
 *
 * Describes the role of an object
 *
 * These are the built-in enumerated roles that UI components can have.
 * Other roles may be added at runtime, so an AccessibleRole >=
 * SPI_ROLE_LAST_DEFINED is not necessarily an error.
 **/
typedef enum
{
  SPI_ROLE_INVALID,
  SPI_ROLE_ACCEL_LABEL,
  SPI_ROLE_ALERT,
  SPI_ROLE_ANIMATION,
  SPI_ROLE_ARROW,
  SPI_ROLE_CALENDAR,
  SPI_ROLE_CANVAS,
  SPI_ROLE_CHECK_BOX,
  SPI_ROLE_CHECK_MENU_ITEM,
  SPI_ROLE_COLOR_CHOOSER,
  SPI_ROLE_COLUMN_HEADER,
  SPI_ROLE_COMBO_BOX,
  SPI_ROLE_DATE_EDITOR,
  SPI_ROLE_DESKTOP_ICON,
  SPI_ROLE_DESKTOP_FRAME,
  SPI_ROLE_DIAL,
  SPI_ROLE_DIALOG,
  SPI_ROLE_DIRECTORY_PANE,
  SPI_ROLE_DRAWING_AREA,
  SPI_ROLE_FILE_CHOOSER,
  SPI_ROLE_FILLER,
  SPI_ROLE_FONT_CHOOSER,
  SPI_ROLE_FRAME,
  SPI_ROLE_GLASS_PANE,
  SPI_ROLE_HTML_CONTAINER,
  SPI_ROLE_ICON,
  SPI_ROLE_IMAGE,
  SPI_ROLE_INTERNAL_FRAME,
  SPI_ROLE_LABEL,
  SPI_ROLE_LAYERED_PANE,
  SPI_ROLE_LIST,
  SPI_ROLE_LIST_ITEM,
  SPI_ROLE_MENU,
  SPI_ROLE_MENU_BAR,
  SPI_ROLE_MENU_ITEM,
  SPI_ROLE_OPTION_PANE,
  SPI_ROLE_PAGE_TAB,
  SPI_ROLE_PAGE_TAB_LIST,
  SPI_ROLE_PANEL,
  SPI_ROLE_PASSWORD_TEXT,
  SPI_ROLE_POPUP_MENU,
  SPI_ROLE_PROGRESS_BAR,
  SPI_ROLE_PUSH_BUTTON,
  SPI_ROLE_RADIO_BUTTON,
  SPI_ROLE_RADIO_MENU_ITEM,
  SPI_ROLE_ROOT_PANE,
  SPI_ROLE_ROW_HEADER,
  SPI_ROLE_SCROLL_BAR,
  SPI_ROLE_SCROLL_PANE,
  SPI_ROLE_SEPARATOR,
  SPI_ROLE_SLIDER,
  SPI_ROLE_SPIN_BUTTON,
  SPI_ROLE_SPLIT_PANE,
  SPI_ROLE_STATUS_BAR,
  SPI_ROLE_TABLE,
  SPI_ROLE_TABLE_CELL,
  SPI_ROLE_TABLE_COLUMN_HEADER,
  SPI_ROLE_TABLE_ROW_HEADER,
  SPI_ROLE_TEAROFF_MENU_ITEM,
  SPI_ROLE_TERMINAL,
  SPI_ROLE_TEXT,
  SPI_ROLE_TOGGLE_BUTTON,
  SPI_ROLE_TOOL_BAR,
  SPI_ROLE_TOOL_TIP,
  SPI_ROLE_TREE,
  SPI_ROLE_TREE_TABLE,
  SPI_ROLE_UNKNOWN,
  SPI_ROLE_VIEWPORT,
  SPI_ROLE_WINDOW,
  SPI_ROLE_EXTENDED,
  SPI_ROLE_HEADER,
  SPI_ROLE_FOOTER,
  SPI_ROLE_PARAGRAPH,
  SPI_ROLE_RULER,
  SPI_ROLE_APPLICATION,
  SPI_ROLE_AUTOCOMPLETE,
  SPI_ROLE_EDITBAR,
  SPI_ROLE_EMBEDDED,
  SPI_ROLE_ENTRY,
  SPI_ROLE_CHART,
  SPI_ROLE_CAPTION,
  SPI_ROLE_DOCUMENT_FRAME,
  SPI_ROLE_HEADING,
  SPI_ROLE_PAGE,
  SPI_ROLE_SECTION,
  SPI_ROLE_FORM,
  SPI_ROLE_REDUNDANT_OBJECT,
  SPI_ROLE_LINK,
  SPI_ROLE_INPUT_METHOD_WINDOW,
  SPI_ROLE_LAST_DEFINED
} AccessibleRole;

char* AccessibleRole_getName (AccessibleRole role);

#ifdef  __cplusplus
}
#endif

#endif
