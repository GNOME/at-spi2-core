#ifndef _SPI_ROLETYPES_H_
#define _SPI_ROLETYPES_H_

/*
 *
 * Enumerated type for AccessibleRole
 *
 */

typedef enum
{
  SPI_ROLE_INVALID,
  /* Object is used to alert the user about something */
  SPI_ROLE_ALERT,
  /* Object that can be drawn into and is used to trap events */
  SPI_ROLE_CANVAS,
  /*
   * A choice that can be checked or unchecked and provides a separate
   * indicator for the current state.
   */
  SPI_ROLE_CHECK_BOX,
  /* A specialized dialog that lets the user choose a color. */
  SPI_ROLE_COLOR_CHOOSER,
  /* The header for a column of data */
  SPI_ROLE_COLUMN_HEADER,
  /* A list of choices the user can select from */
  SPI_ROLE_COMBO_BOX,
  /* An inconifed internal frame within a SPI_DESKTOP_PANE */
  SPI_ROLE_SPI_DESKTOP_ICON,
  /*
   * A pane that supports internal frames and iconified versions of those
   * internal frames.
   */
  SPI_ROLE_SPI_DESKTOP_FRAME,
  /* A top level window with title bar and a border */
  SPI_ROLE_DIALOG,
  /*
   * A pane that allows the user to navigate through and select the contents
   * of a directory
   */
  SPI_ROLE_DIRECTORY_PANE,
  /*
   * A specialized dialog that displays the files in the directory and lets
   * the user select a file, browse a different directory, or specify a
   * filename.
   */
  SPI_ROLE_FILE_CHOOSER,
  /*
   * A object that fills up space in a user interface
   */
  SPI_ROLE_FILLER,
  /* XXX Don't know sure about this. */
  SPI_ROLE_FOCUS_TRAVERSABLE,
  /* A top level window with a title bar, border, menubar, etc. */
  SPI_ROLE_FRAME,
  /* A pane that is guaranteed to be painted on top of all panes beneath it */
  SPI_ROLE_GLASS_PANE,
  /*
   * A document container for HTML, whose children
   * represent the document content.
   */
  SPI_ROLE_HTML_CONTAINER,
  /* A small fixed size picture, typically used to decorate components */
  SPI_ROLE_ICON,
  /* A frame-like object that is clipped by a desktop pane. */
  SPI_ROLE_INTERNAL_FRAME,
  /* An object used to present an icon or short string in an interface */
  SPI_ROLE_LABEL,
  /*
   * A specialized pane that allows its children to be drawn in layers,
   * providing a form of stacking order.
   */
  SPI_ROLE_LAYERED_PANE,
  /*
   * An object that presents a list of objects to the user and allows the
   * user to select one or more of them.
   */
  SPI_ROLE_LIST,
   /* An object that represents an element of a list. */
  SPI_ROLE_LIST_ITEM,
  /*
   * An object usually found inside a menu bar that contains a list of
   * actions the user can choose from.
   */
  SPI_ROLE_MENU,
  /*
   * An object usually drawn at the top of the primary dialog box of an
   * application that contains a list of menus the user can choose from.
   */
  SPI_ROLE_MENU_BAR,
  /*
   * An object usually contained in a menu that presents an action the
   * user can choose.
   */
  SPI_ROLE_MENU_ITEM,
  /* A specialized pane whose primary use is inside a DIALOG */
  SPI_ROLE_OPTION_PANE,
  /* An object that is a child of a page tab list */
  SPI_ROLE_PAGE_TAB,
  /*
   * An object that presents a series of panels (or page tabs), one at a time,
   * through some mechanism provided by the object.
   */
  SPI_ROLE_PAGE_TAB_LIST,
  /* A generic container that is often used to group objects. */
  SPI_ROLE_PANEL,
  /*
   * A text object uses for passwords, or other places where the text
   * content is not shown visibly to the user.
   */
  SPI_ROLE_PASSWORD_TEXT,
  /*
   * A temporary window that is usually used to offer the user a list of
   * choices, and then hides when the user selects one of those choices.
   */
  SPI_ROLE_POPUP_MENU,
  /* An object used to indicate how much of a task has been completed. */
  SPI_ROLE_PROGRESS_BAR,
  /*
   * An object the user can manipulate to tell the application to do
   * something.
   */
  SPI_ROLE_PUSH_BUTTON,
  /*
   * A specialized check box that will cause other radio buttons in the
   * same group to become uncghecked when this one is checked.
   */
  SPI_ROLE_RADIO_BUTTON,
  /*
   * A specialized pane that has a glass pane and a layered pane as its
   * children.
   */
  SPI_ROLE_ROOT_PANE,
  /* The header for a row of data */
  SPI_ROLE_ROW_HEADER,
  /*
   * An object usually used to allow a user to incrementally view a large
   * amount of data.
   */
  SPI_ROLE_SCROLL_BAR,
  /*
   * An object that allows a user to incrementally view a large amount
   * of information.
   */
  SPI_ROLE_SCROLL_PANE,
  /*
   * An object usually contained in a menu to provide a visible and
   * logical separation of the contents in a menu.
   */
  SPI_ROLE_SEPARATOR,
  /* An object that allows the user to select from a bounded range */
  SPI_ROLE_SLIDER,
  /* A specialized panel that presents two other panels at the same time. */
  SPI_ROLE_SPLIT_PANE,
  /* An object used to rpesent information in terms of rows and columns. */
  SPI_ROLE_TABLE,
  SPI_ROLE_TABLE_CELL,
  SPI_ROLE_TABLE_COLUMN_HEADER,
  SPI_ROLE_TABLE_ROW_HEADER,
  /* An object that presents text to the user */
  SPI_ROLE_TEXT,
  /*
   * A specialized push button that can be checked or unchecked, but does
   * not procide a separate indicator for the current state.
   */
  SPI_ROLE_TOGGLE_BUTTON,
  /*
   * A bar or palette usually composed of push buttons or toggle buttons
   */
  SPI_ROLE_TOOL_BAR,
  /*
   * An object that provides information about another object
   */
  SPI_ROLE_TOOL_TIP,
  /* An object used to repsent hierarchical information to the user. */
  SPI_ROLE_TREE,
  /*
   * The object contains some SpiAccessible information, but its role is
   * not known.
   */
  SPI_ROLE_UNKNOWN,
  /* An object usually used in a scroll pane. */
  SPI_ROLE_VIEWPORT,
  /* A top level window with no title or border */
  SPI_ROLE_WINDOW,
  /* not a valid role, used for finding end of enumeration. */
  SPI_ROLE_LAST_DEFINED
} AccessibleRole;

#endif
