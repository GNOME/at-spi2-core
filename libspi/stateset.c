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

/* stateset.c : implements the StateSet interface */

#include <config.h>
#include <stdio.h>
#include <libspi/stateset.h>


static Accessibility_StateType *accessible_state_types = NULL;
static AtkStateType *atk_state_types = NULL;


static gboolean
spi_init_state_type_tables (void)
{
  gint i;

  if (accessible_state_types || atk_state_types)
    return FALSE;
  if (!accessible_state_types)
    accessible_state_types = g_new (Accessibility_StateType, ATK_STATE_LAST_DEFINED);
  if (!atk_state_types)
    atk_state_types = g_new (AtkStateType, Accessibility_STATE_LAST_DEFINED);
  g_return_val_if_fail (accessible_state_types, FALSE);
  g_return_val_if_fail (atk_state_types, FALSE);
  
  for (i = 0; i < Accessibility_STATE_LAST_DEFINED; i++)
    {
      atk_state_types[i] = ATK_STATE_INVALID;
    }

  for (i=0; i < ATK_STATE_LAST_DEFINED; i++)
    {
      accessible_state_types[i] = Accessibility_STATE_INVALID;	
    }

  accessible_state_types[ATK_STATE_ACTIVE] = Accessibility_STATE_ACTIVE;
  atk_state_types[Accessibility_STATE_ACTIVE] = ATK_STATE_ACTIVE;
  accessible_state_types[ATK_STATE_ARMED] = Accessibility_STATE_ARMED;
  atk_state_types[Accessibility_STATE_ARMED] = ATK_STATE_ARMED;
  accessible_state_types[ATK_STATE_BUSY] = Accessibility_STATE_BUSY;
  atk_state_types[Accessibility_STATE_BUSY] = ATK_STATE_BUSY;
  accessible_state_types[ATK_STATE_CHECKED] = Accessibility_STATE_CHECKED;
  atk_state_types[Accessibility_STATE_CHECKED] = ATK_STATE_CHECKED;
  accessible_state_types[ATK_STATE_DEFUNCT] = Accessibility_STATE_DEFUNCT;
  atk_state_types[Accessibility_STATE_DEFUNCT] = ATK_STATE_DEFUNCT;
  accessible_state_types[ATK_STATE_EDITABLE] = Accessibility_STATE_EDITABLE;
  atk_state_types[Accessibility_STATE_EDITABLE] = ATK_STATE_EDITABLE;  
  accessible_state_types[ATK_STATE_ENABLED] = Accessibility_STATE_ENABLED;
  atk_state_types[Accessibility_STATE_ENABLED] = ATK_STATE_ENABLED;  
  accessible_state_types[ATK_STATE_EXPANDABLE] = Accessibility_STATE_EXPANDABLE;
  atk_state_types[Accessibility_STATE_EXPANDABLE] = ATK_STATE_EXPANDABLE;
  accessible_state_types[ATK_STATE_EXPANDED] = Accessibility_STATE_EXPANDED;
  atk_state_types[Accessibility_STATE_EXPANDED] = ATK_STATE_EXPANDED;
  accessible_state_types[ATK_STATE_FOCUSABLE] = Accessibility_STATE_FOCUSABLE;
  atk_state_types[Accessibility_STATE_FOCUSABLE] = ATK_STATE_FOCUSABLE;
  accessible_state_types[ATK_STATE_FOCUSED] = Accessibility_STATE_FOCUSED;
  atk_state_types[Accessibility_STATE_FOCUSED] = ATK_STATE_FOCUSED;
  accessible_state_types[ATK_STATE_HORIZONTAL] = Accessibility_STATE_HORIZONTAL;
  atk_state_types[Accessibility_STATE_HORIZONTAL] = ATK_STATE_HORIZONTAL;
  accessible_state_types[ATK_STATE_ICONIFIED] = Accessibility_STATE_ICONIFIED;
  atk_state_types[Accessibility_STATE_ICONIFIED] = ATK_STATE_ICONIFIED;
  accessible_state_types[ATK_STATE_MODAL] = Accessibility_STATE_MODAL;
  atk_state_types[Accessibility_STATE_MODAL] = ATK_STATE_MODAL;
  accessible_state_types[ATK_STATE_MULTI_LINE] = Accessibility_STATE_MULTI_LINE;
  atk_state_types[Accessibility_STATE_MULTI_LINE] = ATK_STATE_MULTI_LINE;
  accessible_state_types[ATK_STATE_MULTISELECTABLE] = Accessibility_STATE_MULTISELECTABLE;
  atk_state_types[Accessibility_STATE_MULTISELECTABLE] = ATK_STATE_MULTISELECTABLE;
  accessible_state_types[ATK_STATE_OPAQUE] = Accessibility_STATE_OPAQUE;
  atk_state_types[Accessibility_STATE_OPAQUE] = ATK_STATE_OPAQUE;
  accessible_state_types[ATK_STATE_PRESSED] = Accessibility_STATE_PRESSED;
  atk_state_types[Accessibility_STATE_PRESSED] = ATK_STATE_PRESSED;
  accessible_state_types[ATK_STATE_RESIZABLE] = Accessibility_STATE_RESIZABLE;
  atk_state_types[Accessibility_STATE_RESIZABLE] = ATK_STATE_RESIZABLE;
  accessible_state_types[ATK_STATE_SELECTABLE] = Accessibility_STATE_SELECTABLE;
  atk_state_types[Accessibility_STATE_SELECTABLE] = ATK_STATE_SELECTABLE;
  accessible_state_types[ATK_STATE_SELECTED] = Accessibility_STATE_SELECTED;
  atk_state_types[Accessibility_STATE_SELECTED] = ATK_STATE_SELECTED;
  accessible_state_types[ATK_STATE_SENSITIVE] = Accessibility_STATE_SENSITIVE;
  atk_state_types[Accessibility_STATE_SENSITIVE] = ATK_STATE_SENSITIVE;
  accessible_state_types[ATK_STATE_SHOWING] = Accessibility_STATE_SHOWING;
  atk_state_types[Accessibility_STATE_SHOWING] = ATK_STATE_SHOWING;
  accessible_state_types[ATK_STATE_SINGLE_LINE] = Accessibility_STATE_SINGLE_LINE;
  atk_state_types[Accessibility_STATE_SINGLE_LINE] = ATK_STATE_SINGLE_LINE;
  accessible_state_types[ATK_STATE_STALE] = Accessibility_STATE_STALE;
  atk_state_types[Accessibility_STATE_STALE] = ATK_STATE_STALE;
  accessible_state_types[ATK_STATE_TRANSIENT] = Accessibility_STATE_TRANSIENT;
  atk_state_types[Accessibility_STATE_TRANSIENT] = ATK_STATE_TRANSIENT;
  accessible_state_types[ATK_STATE_VERTICAL] = Accessibility_STATE_VERTICAL;
  atk_state_types[Accessibility_STATE_VERTICAL] = ATK_STATE_VERTICAL;
  accessible_state_types[ATK_STATE_VISIBLE] = Accessibility_STATE_VISIBLE;
  atk_state_types[Accessibility_STATE_VISIBLE] = ATK_STATE_VISIBLE;
  accessible_state_types[ATK_STATE_MANAGES_DESCENDANTS] = Accessibility_STATE_MANAGES_DESCENDANTS;
  atk_state_types[Accessibility_STATE_MANAGES_DESCENDANTS] = ATK_STATE_MANAGES_DESCENDANTS;
  accessible_state_types[ATK_STATE_INDETERMINATE] = Accessibility_STATE_INDETERMINATE;
  atk_state_types[Accessibility_STATE_INDETERMINATE] = ATK_STATE_INDETERMINATE;
  accessible_state_types[ATK_STATE_TRUNCATED] = Accessibility_STATE_TRUNCATED;
  atk_state_types[Accessibility_STATE_TRUNCATED] = ATK_STATE_TRUNCATED;
  accessible_state_types[ATK_STATE_REQUIRED] = Accessibility_STATE_REQUIRED;
  atk_state_types[Accessibility_STATE_REQUIRED] = ATK_STATE_REQUIRED;
  accessible_state_types[ATK_STATE_INVALID_ENTRY] = Accessibility_STATE_INVALID_ENTRY;
  atk_state_types[Accessibility_STATE_INVALID_ENTRY] = ATK_STATE_INVALID_ENTRY;
  accessible_state_types[ATK_STATE_SUPPORTS_AUTOCOMPLETION] = Accessibility_STATE_SUPPORTS_AUTOCOMPLETION;
  atk_state_types[Accessibility_STATE_SUPPORTS_AUTOCOMPLETION] = ATK_STATE_SUPPORTS_AUTOCOMPLETION;
  accessible_state_types[ATK_STATE_SELECTABLE_TEXT] = Accessibility_STATE_SELECTABLE_TEXT;
  atk_state_types[Accessibility_STATE_SELECTABLE_TEXT] = ATK_STATE_SELECTABLE_TEXT;
  accessible_state_types[ATK_STATE_DEFAULT] = Accessibility_STATE_IS_DEFAULT;
  atk_state_types[Accessibility_STATE_IS_DEFAULT] = ATK_STATE_DEFAULT;
  accessible_state_types[ATK_STATE_VISITED] = Accessibility_STATE_VISITED;
  atk_state_types[Accessibility_STATE_VISITED] = ATK_STATE_VISITED;


  return TRUE;
}

static inline AtkState
state_spi_to_atk (Accessibility_StateType state)
{
  guint idx = state;
  if (idx < Accessibility_STATE_LAST_DEFINED)
    return atk_state_types [idx];
  else
    return ATK_STATE_INVALID;
}

AtkState
spi_atk_state_from_spi_state (Accessibility_StateType state)
{
  spi_init_state_type_tables ();
  return state_spi_to_atk (state);
}

static AtkStateSet *
atk_state_set_from_servant (PortableServer_Servant servant)
{
  SpiBase *base = SPI_BASE (bonobo_object_from_servant(servant));

  g_return_val_if_fail (base, NULL);
  return  ATK_STATE_SET(base->gobj);
}

AtkStateSet *
spi_state_set_cache_from_sequence (const Accessibility_StateSeq *seq)
{
  int i;
  AtkStateSet *set;
  AtkStateType *states;

  spi_init_state_type_tables ();
  
  states = g_newa (AtkStateType, seq->_length);
  for (i = 0; i < seq->_length; i++)
    states [i] = state_spi_to_atk (seq->_buffer [i]);

  set = atk_state_set_new ();
  atk_state_set_add_states (set, states, seq->_length);

  return set;
}

static AtkStateSet *
atk_state_set_from_accessibility_state_set (Accessibility_StateSet set, CORBA_Environment *ev)
{
  AtkStateSet *rv;
  Accessibility_StateSeq *seq;
  
  seq = Accessibility_StateSet_getStates (set, ev);
  rv = spi_state_set_cache_from_sequence (seq);
  CORBA_free (seq);

  return rv;
}


SpiStateSet *
spi_state_set_new (AtkStateSet *obj)
{
  SpiStateSet *new_set = g_object_new (SPI_STATE_SET_TYPE, NULL);
  spi_base_construct (SPI_BASE (new_set), G_OBJECT (obj));
  return new_set;
}


static CORBA_boolean
impl_contains (PortableServer_Servant servant,
	       const Accessibility_StateType state,
	       CORBA_Environment * ev)
{
  AtkStateSet *set = atk_state_set_from_servant (servant);

  g_return_val_if_fail (set, FALSE);
  return atk_state_set_contains_state (set, state_spi_to_atk (state));
}


static void 
impl_add (PortableServer_Servant servant,
	  const Accessibility_StateType state,
	  CORBA_Environment * ev)
{
  AtkStateSet *set = atk_state_set_from_servant (servant);

  g_return_if_fail (set);
  atk_state_set_add_state (set, state_spi_to_atk (state));
}


static void 
impl_remove (PortableServer_Servant servant,
	     const Accessibility_StateType state,
	     CORBA_Environment * ev)
{
  AtkStateSet *set = atk_state_set_from_servant (servant);

  g_return_if_fail (set);
  atk_state_set_remove_state (set, state_spi_to_atk (state));
}


static CORBA_boolean
impl_equals (PortableServer_Servant servant,
	     const Accessibility_StateSet stateSet,
	     CORBA_Environment * ev)
{
  AtkStateSet *set = atk_state_set_from_servant (servant);
  AtkStateSet *set2, *return_set;
  CORBA_boolean rv;
  
  g_return_val_if_fail (set, FALSE);

  set2 = atk_state_set_from_accessibility_state_set (stateSet, ev);
  g_return_val_if_fail (set2, FALSE);

  return_set = atk_state_set_xor_sets (set, set2);
  g_object_unref (G_OBJECT(set2));
  if (return_set)
    {
      rv = atk_state_set_is_empty (return_set);
      g_object_unref (G_OBJECT(return_set));
    }
  else
    rv = FALSE;
  return rv;
}


static Accessibility_StateSet
impl_compare (PortableServer_Servant servant,
	      const Accessibility_StateSet compareState,
	      CORBA_Environment * ev)
{
  AtkStateSet *set = atk_state_set_from_servant (servant);
  AtkStateSet *set2, *return_set;
  SpiStateSet *spi_set;
  
  g_return_val_if_fail (set, NULL);

  set2 = atk_state_set_from_accessibility_state_set (compareState, ev);
  g_return_val_if_fail (set2, CORBA_OBJECT_NIL);

  return_set = atk_state_set_xor_sets (set, set2);
  g_object_unref (G_OBJECT(set2));
  spi_set = spi_state_set_new (return_set);
  return bonobo_object_corba_objref (BONOBO_OBJECT(spi_set));
}


static CORBA_boolean
impl_isEmpty (PortableServer_Servant servant,
	      CORBA_Environment * ev)
{
  AtkStateSet *set = atk_state_set_from_servant (servant);

  g_return_val_if_fail (set, TRUE);
  return atk_state_set_is_empty (set);
}


static Accessibility_StateSeq *
impl_getStates (PortableServer_Servant servant,
		CORBA_Environment * ev)
{
  AtkStateSet *set = atk_state_set_from_servant (servant);
  GSList *states = NULL;
  GSList *tmp;
  gint i = 0;
  Accessibility_StateSeq *rv;
  
  g_return_val_if_fail (set, CORBA_OBJECT_NIL);

  /* Argh-- this is bad!!! */

  if (atk_state_set_contains_state (set, ATK_STATE_ACTIVE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_ACTIVE);
  if (atk_state_set_contains_state (set, ATK_STATE_ARMED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_ARMED);
  if (atk_state_set_contains_state (set, ATK_STATE_BUSY))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_BUSY);
  if (atk_state_set_contains_state (set, ATK_STATE_CHECKED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_CHECKED);
  if (atk_state_set_contains_state (set, ATK_STATE_DEFUNCT))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_DEFUNCT);
  if (atk_state_set_contains_state (set, ATK_STATE_EDITABLE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_EDITABLE);
  if (atk_state_set_contains_state (set, ATK_STATE_ENABLED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_ENABLED);
  if (atk_state_set_contains_state (set, ATK_STATE_EXPANDABLE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_EXPANDABLE);
  if (atk_state_set_contains_state (set, ATK_STATE_EXPANDED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_EXPANDED);
  if (atk_state_set_contains_state (set, ATK_STATE_FOCUSABLE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_FOCUSABLE);
  if (atk_state_set_contains_state (set, ATK_STATE_FOCUSED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_FOCUSED);
  if (atk_state_set_contains_state (set, ATK_STATE_HORIZONTAL))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_HORIZONTAL);
  if (atk_state_set_contains_state (set, ATK_STATE_ICONIFIED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_ICONIFIED);
  if (atk_state_set_contains_state (set, ATK_STATE_MODAL))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_MODAL);
  if (atk_state_set_contains_state (set, ATK_STATE_MULTI_LINE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_MULTI_LINE);
  if (atk_state_set_contains_state (set, ATK_STATE_MULTISELECTABLE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_MULTISELECTABLE);
  if (atk_state_set_contains_state (set, ATK_STATE_OPAQUE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_OPAQUE);
  if (atk_state_set_contains_state (set, ATK_STATE_PRESSED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_PRESSED);
  if (atk_state_set_contains_state (set, ATK_STATE_RESIZABLE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_RESIZABLE);
  if (atk_state_set_contains_state (set, ATK_STATE_SELECTABLE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_SELECTABLE);
  if (atk_state_set_contains_state (set, ATK_STATE_SELECTED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_SELECTED);
  if (atk_state_set_contains_state (set, ATK_STATE_SENSITIVE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_SENSITIVE);
  if (atk_state_set_contains_state (set, ATK_STATE_SHOWING))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_SHOWING);
  if (atk_state_set_contains_state (set, ATK_STATE_SINGLE_LINE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_SINGLE_LINE);
  if (atk_state_set_contains_state (set, ATK_STATE_STALE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_STALE);
  if (atk_state_set_contains_state (set, ATK_STATE_TRANSIENT))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_TRANSIENT);
  if (atk_state_set_contains_state (set, ATK_STATE_VERTICAL))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_VERTICAL);
  if (atk_state_set_contains_state (set, ATK_STATE_VISIBLE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_VISIBLE);
  if (atk_state_set_contains_state (set, ATK_STATE_MANAGES_DESCENDANTS))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_MANAGES_DESCENDANTS);
  if (atk_state_set_contains_state (set, ATK_STATE_INDETERMINATE))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_INDETERMINATE);
  if (atk_state_set_contains_state (set, ATK_STATE_REQUIRED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_REQUIRED);
  if (atk_state_set_contains_state (set, ATK_STATE_TRUNCATED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_TRUNCATED);
  if (atk_state_set_contains_state (set, ATK_STATE_ANIMATED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_ANIMATED);
  if (atk_state_set_contains_state (set, ATK_STATE_INVALID_ENTRY))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_INVALID_ENTRY);
  if (atk_state_set_contains_state (set, ATK_STATE_SUPPORTS_AUTOCOMPLETION))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_SUPPORTS_AUTOCOMPLETION);
  if (atk_state_set_contains_state (set, ATK_STATE_DEFAULT))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_IS_DEFAULT);
  if (atk_state_set_contains_state (set, ATK_STATE_VISITED))
    states = g_slist_append (states, (gpointer) Accessibility_STATE_VISITED);

  rv = Accessibility_StateSeq__alloc ();
  rv->_length = rv->_maximum = g_slist_length (states);
  rv->_buffer = Accessibility_StateSeq_allocbuf (rv->_length);
  tmp = states;
  while (tmp)
    {
      rv->_buffer[i++] = (Accessibility_StateType) tmp->data;
      tmp = tmp->next;
    }
  g_slist_free (states);
  return rv;
}


static void
spi_state_set_class_init (SpiStateSetClass *klass)
{
  POA_Accessibility_StateSet__epv *epv = &klass->epv;

  spi_init_state_type_tables ();
  epv->contains = impl_contains;
  epv->add = impl_add;
  epv->remove = impl_remove;
  epv->equals = impl_equals;
  epv->compare = impl_compare;
  epv->isEmpty = impl_isEmpty;
  epv->getStates = impl_getStates;  
}


static void
spi_state_set_init (SpiStateSet *set)
{
}


BONOBO_TYPE_FUNC_FULL (SpiStateSet,
		       Accessibility_StateSet,
		       SPI_TYPE_BASE,
		       spi_state_set)
