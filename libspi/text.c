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

/* text.c : implements the Text interface */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <atk/atktext.h>
#include <libspi/text.h>
#include <libspi/spi-private.h>

/* Our parent Gtk object type */
#define PARENT_TYPE SPI_TYPE_BASE

typedef struct {
  gint x;
  gint y;
  gint w;
  gint h;
} SpiTextRect;

static SpiTextRect *
_spi_text_rect_union (SpiTextRect *aggregate, SpiTextRect *subrect)
{
  if (subrect != NULL)
    {
      /* 'normalize' subrect */
      if (subrect->w < 0)
	{
	  subrect->x += subrect->w;
	  subrect->w *= -1;
	}
      if (subrect->h < 0)
	{
	  subrect->y += subrect->h;
	  subrect->h *= -1;
	}
      if (aggregate == NULL)
	{
	  aggregate = g_new (SpiTextRect, 1);
	  memcpy (aggregate, subrect, sizeof (SpiTextRect));
	}
      else
	{
	  gint ax2 = aggregate->x + aggregate->w;
	  gint ay2 = aggregate->y + aggregate->h; 
	  gint sx2 = subrect->x + subrect->w; 
	  gint sy2 = subrect->y + subrect->h;
	  if (subrect->x < aggregate->x)
	    {
	      aggregate->w += (aggregate->x - subrect->x);
	      aggregate->x = subrect->x;
	    }
	  if (sx2 > ax2)
	    {
	      aggregate->w += (sx2 - ax2);
	    }
	  if (subrect->y < aggregate->y)
	    {
	      aggregate->h += (aggregate->y - subrect->y);
	      aggregate->y = subrect->y;
	    }
	  if (sy2 > ay2)
	    {
	      aggregate->h += (sy2 - ay2);
	    }
	}
    }
  return aggregate;
}

static AtkText *
get_text_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT(object->gobj), NULL);
  return ATK_TEXT (object->gobj);
}

static CORBA_string
impl_getText (PortableServer_Servant servant,
	      const CORBA_long       startOffset,
	      const CORBA_long       endOffset,
	      CORBA_Environment     *ev)
{
  gchar *txt;
  CORBA_string rv;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));
  
  txt = atk_text_get_text (text, startOffset, endOffset);
  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


CORBA_string
impl_getTextAfterOffset (PortableServer_Servant servant,
			 const CORBA_long offset,
			 const
			 Accessibility_TEXT_BOUNDARY_TYPE
			 type, CORBA_long * startOffset,
			 CORBA_long * endOffset,
			 CORBA_Environment *ev)
{
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  txt = atk_text_get_text_after_offset (text,
					offset, (AtkTextBoundary) type,
					&intStartOffset, &intEndOffset);
  *startOffset = intStartOffset;
  *endOffset = intEndOffset;

  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


static CORBA_string
impl_getTextAtOffset (PortableServer_Servant servant,
		      const CORBA_long offset,
		      const Accessibility_TEXT_BOUNDARY_TYPE type,
		      CORBA_long * startOffset,
		      CORBA_long * endOffset,
		      CORBA_Environment *ev)
{
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  txt = atk_text_get_text_at_offset (
	  text,
	  offset, (AtkTextBoundary) type,
	  &intStartOffset, &intEndOffset);

  *startOffset = intStartOffset;
  *endOffset = intEndOffset;

  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


static CORBA_unsigned_long
impl_getCharacterAtOffset (PortableServer_Servant servant,
			   const CORBA_long offset,
			   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, 0);

  return atk_text_get_character_at_offset (text, offset);
}


static CORBA_string
impl_getTextBeforeOffset (PortableServer_Servant servant,
			  const CORBA_long offset,
			  const
			  Accessibility_TEXT_BOUNDARY_TYPE
			  type, CORBA_long * startOffset,
			  CORBA_long * endOffset,
			  CORBA_Environment *ev)
{
  gchar *txt;
  CORBA_char *rv;
  gint intStartOffset, intEndOffset;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  txt = atk_text_get_text_before_offset (text,
					 offset, (AtkTextBoundary) type,
					 &intStartOffset, &intEndOffset);

  *startOffset = intStartOffset;
  *endOffset = intEndOffset;

  if (txt)
    {
      rv = CORBA_string_dup (txt);
      g_free (txt);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
}


static CORBA_long
impl__get_caretOffset (PortableServer_Servant servant,
		     CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, -1);

  return atk_text_get_caret_offset (text);
}


static CORBA_char *
_string_from_attribute_set (AtkAttributeSet *set)
{
  gchar *attributes, *tmp, *tmp2;
  CORBA_char *rv;
  GSList *cur_attr;
  AtkAttribute *at;
  
  attributes = g_strdup ("");
  cur_attr = (GSList *) set;
  while (cur_attr)
    {
      at = (AtkAttribute *) cur_attr->data;
      tmp = g_strdup_printf ("%s%s:%s%s",
			     ((GSList *)(set) == cur_attr) ? "" : " ",
			     at->name, at->value,
			     (cur_attr->next) ? ";" : "");
      tmp2 = g_strconcat (attributes, tmp, NULL);
      g_free (tmp);
      g_free (attributes);
      attributes = tmp2;
      cur_attr = cur_attr->next;
    }
  rv = CORBA_string_dup (attributes);
  g_free (attributes);
  return rv;
}



static CORBA_string
impl_getAttributes (PortableServer_Servant servant,
		    const CORBA_long offset,
		    CORBA_long * startOffset,
		    CORBA_long * endOffset,
		    CORBA_Environment *ev)
{
  AtkAttributeSet *set;
  gint intstart_offset, intend_offset;
  CORBA_char *rv;
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, CORBA_string_dup (""));

  set = atk_text_get_run_attributes (text, offset,
				     &intstart_offset, &intend_offset);
  *startOffset = intstart_offset;
  *endOffset = intend_offset;
  rv = _string_from_attribute_set (set);
  atk_attribute_set_free (set);
  return rv;  
}


static void 
impl_getCharacterExtents (PortableServer_Servant servant,
			  const CORBA_long offset, CORBA_long * x,
			  CORBA_long * y, CORBA_long * width,
			  CORBA_long * height,
			  const CORBA_short coordType,
			  CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);
  gint ix, iy, iw, ih;

  g_return_if_fail (text != NULL);

  atk_text_get_character_extents (
	  text, offset,
	  &ix, &iy, &iw, &ih,
	  (AtkCoordType) coordType);
  *x = ix;
  *y = iy;
  *width = iw;
  *height = ih;
}


static CORBA_long
impl__get_characterCount (PortableServer_Servant servant,
			  CORBA_Environment    *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, 0);

  return atk_text_get_character_count (text);
}


static CORBA_long
impl_getOffsetAtPoint (PortableServer_Servant servant,
		       const CORBA_long x, const CORBA_long y,
		       const CORBA_short coordType,
		       CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, -1);

  return atk_text_get_offset_at_point (text,
				  x, y,
				  (AtkCoordType) coordType);
}


static CORBA_long
impl_getNSelections (PortableServer_Servant servant,
		     CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, 0);

  return atk_text_get_n_selections (text);
}


static void 
impl_getSelection (PortableServer_Servant servant,
		   const CORBA_long selectionNum,
		   CORBA_long * startOffset, CORBA_long * endOffset,
		   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);
  gint intStartOffset, intEndOffset;
  
  g_return_if_fail (text != NULL);

  /* atk_text_get_selection returns gchar* which we discard */
  g_free (atk_text_get_selection (text, selectionNum,
				  &intStartOffset, &intEndOffset));
  
  *startOffset = intStartOffset;
  *endOffset = intEndOffset;
}


static CORBA_boolean
impl_addSelection (PortableServer_Servant servant,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_add_selection (text,
			    startOffset, endOffset);
}


static CORBA_boolean
impl_removeSelection (PortableServer_Servant servant,
		      const CORBA_long selectionNum,
		      CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_remove_selection (text, selectionNum);
}


static CORBA_boolean
impl_setSelection (PortableServer_Servant servant,
		   const CORBA_long selectionNum,
		   const CORBA_long startOffset,
		   const CORBA_long endOffset,
		   CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_set_selection (text,
			    selectionNum, startOffset, endOffset);
}


static CORBA_boolean
impl_setCaretOffset (PortableServer_Servant servant,
		     const CORBA_long value,
		     CORBA_Environment *ev)
{
  AtkText *text = get_text_from_servant (servant);

  g_return_val_if_fail (text != NULL, FALSE);

  return atk_text_set_caret_offset (text, value);
}

#define SPI_TEXT_MIN_RANGE_FOR_LINE_CHECK 6

static void
impl_getRangeExtents(PortableServer_Servant servant,
		     const CORBA_long startOffset,
		     const CORBA_long endOffset,
		     CORBA_long * x, CORBA_long * y,
		     CORBA_long * width,
		     CORBA_long * height,
		     const CORBA_short coordType,
		     CORBA_Environment * ev)
{
  AtkText *text = get_text_from_servant (servant);
  SpiTextRect cbounds, bounds;
  int i;

  g_return_if_fail (text != NULL);
  
  atk_text_get_character_extents (text, startOffset,
			          &bounds.x, &bounds.y, &bounds.w, &bounds.h,
				  (AtkCoordType) coordType);
  /* no equivalent ATK API yet, must do the hard way. :-( */
  for (i = startOffset + 1; i < endOffset; i++) 
    {
      atk_text_get_character_extents (text, i,
				      &cbounds.x, &cbounds.y, &cbounds.w, &cbounds.h,
				      (AtkCoordType) coordType);
      _spi_text_rect_union (&bounds, &cbounds);
    }

  *x = bounds.x;
  *y = bounds.y;
  *width = bounds.w;
  *height = bounds.h;
}

static Accessibility_Text_RangeList *
_spi_text_range_seq_from_gslist (GSList *range_list) 
{ 
  Accessibility_Text_RangeList *rangeList = 
    Accessibility_Text_RangeList__alloc ();
  int i, len = g_slist_length (range_list);
  GSList *list = range_list;

  rangeList->_length = len;
  rangeList->_buffer = Accessibility_Text_RangeList_allocbuf (len);
  for (i = 0; i < len; ++i) 
    {
      memcpy (&rangeList->_buffer[i], list->data, sizeof (Accessibility_Text_Range));
      spi_init_any_nil (&rangeList->_buffer[i].data);
      g_free (list->data);
      list = g_slist_next (range_list);
    }
  g_slist_free (range_list);

  return rangeList;
}

static gboolean
_spi_bounds_contain (SpiTextRect *clip, SpiTextRect *cbounds, 
		     Accessibility_TEXT_CLIP_TYPE xClipType, 
		     Accessibility_TEXT_CLIP_TYPE yClipType)
{
  gint clipx2 = clip->x + clip->w;
  gint clipy2 = clip->y + clip->h;
  gint charx2 = cbounds->x + cbounds->w;
  gint chary2 = cbounds->y + cbounds->h;
  gboolean x_min_ok, y_min_ok, x_max_ok, y_max_ok;

  x_min_ok = (cbounds->x >= clip->x) || 
    ((charx2 >= clip->x) && 
     ((xClipType == Accessibility_TEXT_CLIP_NONE) || 
      (xClipType == Accessibility_TEXT_CLIP_MAX)));
  x_max_ok = (charx2 <= clipx2) || 
    ((cbounds->x <= clipx2) && 
     ((xClipType == Accessibility_TEXT_CLIP_NONE) || 
      (xClipType == Accessibility_TEXT_CLIP_MIN)));
  y_min_ok = (cbounds->y >= clip->y) || 
    ((chary2 >= clip->y) && 
     ((yClipType == Accessibility_TEXT_CLIP_NONE) || 
      (yClipType == Accessibility_TEXT_CLIP_MAX)));
  y_max_ok = (chary2 <= clipy2) || 
    ((cbounds->y <= clipy2) && 
     ((yClipType == Accessibility_TEXT_CLIP_NONE) || 
      (yClipType == Accessibility_TEXT_CLIP_MIN)));
  
  if (x_min_ok && y_min_ok && x_max_ok && y_max_ok)
    return TRUE;
  else 
    return FALSE;
}

Accessibility_Text_RangeList *
impl_getBoundedRanges(PortableServer_Servant servant,
		      const CORBA_long x,
		      const CORBA_long y,
		      const CORBA_long width,
		      const CORBA_long height,
		      const CORBA_short coordType,
		      const Accessibility_TEXT_CLIP_TYPE xClipType,
		      const Accessibility_TEXT_CLIP_TYPE yClipType, 
		      CORBA_Environment * ev)
{
  AtkText *text = get_text_from_servant (servant);
  GSList *range_list = NULL;
  SpiTextRect clip;
  int startOffset = 0, endOffset = atk_text_get_character_count (text);
  int curr_offset;
  gint minLineStart, minLineEnd, maxLineStart, maxLineEnd;
  long bounds_min_offset;
  long bounds_max_offset;

  clip.x = x;
  clip.y = y;
  clip.w = width;
  clip.h = height;

  /* for horizontal text layouts, at least, the following check helps. */
  bounds_min_offset =  atk_text_get_offset_at_point (text, x, y, 
						     (AtkCoordType) coordType);
  bounds_max_offset =  atk_text_get_offset_at_point (text, x + width, y + height, 
						     (AtkCoordType) coordType);
  atk_text_get_text_at_offset (text, bounds_min_offset, 
			       ATK_TEXT_BOUNDARY_LINE_START,
			       &minLineStart, &minLineEnd);
  atk_text_get_text_at_offset (text, bounds_max_offset, 
			       ATK_TEXT_BOUNDARY_LINE_START,
			       &maxLineStart, &maxLineEnd);
  startOffset = MIN (minLineStart, maxLineStart);
  endOffset  = MAX (minLineEnd, maxLineEnd);

  curr_offset = startOffset;

  while (curr_offset < endOffset) 
    {
      int offset = startOffset;
      SpiTextRect cbounds;
      while (curr_offset < endOffset) 
	{
	  atk_text_get_character_extents (text, curr_offset, 
					  &cbounds.x, &cbounds.y, 
					  &cbounds.w, &cbounds.h, 
					  (AtkCoordType) coordType);
	  if (!_spi_bounds_contain (&clip, &cbounds, xClipType, yClipType))
	    break;
	  curr_offset++;
	}
      /* add the range to our list */
      if (curr_offset > offset) 
	{
	  Accessibility_Text_Range *range = g_malloc (sizeof (Accessibility_Text_Range));
	  char *s;
	  range->startOffset = offset;
	  range->endOffset = curr_offset;
	  s = atk_text_get_text (text, offset, curr_offset);
	  range->content = CORBA_string_dup (s ? s : "");
	  range_list = g_slist_append (range_list, range);
	  offset = curr_offset;
	}
      offset++;
    }  
  return _spi_text_range_seq_from_gslist (range_list); /* frees the GSList too */
}


static void
spi_text_class_init (SpiTextClass *klass)
{
  POA_Accessibility_Text__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->getText = impl_getText;
  epv->getTextAfterOffset = impl_getTextAfterOffset;
  epv->getCharacterAtOffset = impl_getCharacterAtOffset;
  epv->getTextAtOffset = impl_getTextAtOffset;
  epv->getTextBeforeOffset = impl_getTextBeforeOffset;
  epv->_get_caretOffset = impl__get_caretOffset;
  epv->getAttributes = impl_getAttributes;
  epv->getCharacterExtents = impl_getCharacterExtents;
  epv->_get_characterCount = impl__get_characterCount;
  epv->getOffsetAtPoint = impl_getOffsetAtPoint;
  epv->getNSelections = impl_getNSelections;
  epv->getSelection = impl_getSelection;
  epv->addSelection = impl_addSelection;
  epv->removeSelection = impl_removeSelection;
  epv->setSelection = impl_setSelection;
  epv->setCaretOffset = impl_setCaretOffset;
  epv->getRangeExtents = impl_getRangeExtents;
  epv->getBoundedRanges = impl_getBoundedRanges;
}

static void
spi_text_init (SpiText *text)
{
}

BONOBO_TYPE_FUNC_FULL (SpiText,
		       Accessibility_Text,
		       PARENT_TYPE,
		       spi_text)

void
spi_text_construct (SpiText *text, AtkObject *obj)
{
  spi_base_construct (SPI_BASE (text), G_OBJECT(obj));
}


SpiText *
spi_text_interface_new (AtkObject *obj)
{
  SpiText *retval;

  g_return_val_if_fail (ATK_IS_TEXT (obj), NULL);

  retval = g_object_new (SPI_TEXT_TYPE, NULL);

  spi_text_construct (retval, obj);

  return retval;
}
