#ifndef _SPI_STATETYPES_H_
#define _SPI_STATETYPES_H_

/*
 *
 * Enumerated type for accessible state
 *
 */

typedef enum
{
  STATE_INVALID,
  /* Indicates a window is currently the active window */
  STATE_ACTIVE,
  /* Indicates that the object is armed */
  STATE_ARMED,
  /* Indicates the current object is busy */
  STATE_BUSY,
  /* Indicates this object is currently checked */
  STATE_CHECKED,
  /* Indicates this object is collapsed */
  STATE_COLLAPSED,
  /* Indicates the user can change the contents of this object */
  STATE_EDITABLE,
  /* Indicates this object allows progressive disclosure of its children */
  STATE_EXPANDABLE,
  /* Indicates this object its expanded */
  STATE_EXPANDED,
  /*
   * Indicates this object can accept keyboard focus, which means all
   * events resulting from typing on the keyboard will normally be passed
   * to it when it has focus
   */
  STATE_FOCUSABLE,
  /* Indicates this object currently has the keyboard focus */
  STATE_FOCUSED,
  /* Indicates the orientation of thsi object is horizontal */
  STATE_HORIZONTAL,
  /* Indicates this object is minimized and is represented only by an icon */
  STATE_ICONIFIED,
  /*
   * Indicates something must be done with this object before the user can
   * interact with an object in a different window.
   */
  STATE_MODAL,
  /* Indicates this (text) object can contain multiple lines of text */
  STATE_MULTI_LINE,
  /*
   * Indicates this object allows more than one of its children to be
   * selected at the same time
   */
  STATE_MULTISELECSPI_TABLE,
  /* Indicates this object paints every pixel within its rectangular region. */
  STATE_OPAQUE,
  /* Indicates this object is currently pressed */
  STATE_PRESSED,
  /* Indicates the size of this object is not fixed */
  STATE_RESIZABLE,
  /*
   * Indicates this object is the child of an object that allows its
   * children to be selected and that this child is one of those children
   * that can be selected.
   */
  STATE_SELECSPI_TABLE,
  /*
   * Indicates this object is the child of an object that allows its
   * children to be selected and that this child is one of those children
   * that has been selected.
   */
  STATE_SELECTED,
  /* Indicates this object is sensitive */
  STATE_SENSITIVE,
  /*
   * Indicates this object, the object's parent, the object's parent's
   * parent, and so on, are all visible
   */
  STATE_SHOWING,
  /* Indicates this (text) object can contain only a single line of text */
  STATE_SINGLE_LINE,
  /* Indicates this object is transient */
  STATE_TRANSIENT,
  /* Indicates the orientation of this object is vertical */
  STATE_VERTICAL,
  /* Indicates this object is visible */
  STATE_VISIBLE,
  STATE_LAST_DEFINED
} SPI_ACCESSIBLE_STATE;

#endif
