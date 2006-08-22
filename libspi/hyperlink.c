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

/* hyperlink.c : implements the Hyperlink interface */

#include <config.h>
#include <stdio.h>
#include <libspi/hyperlink.h>
#include <libspi/action.h>
#include <libspi/accessible.h>

/* Static function declarations */

static void
spi_hyperlink_class_init (SpiHyperlinkClass *klass);
static void
spi_hyperlink_init (SpiHyperlink *hyperlink);
static CORBA_string
impl_getURI (PortableServer_Servant _servant,
	     const CORBA_long i, CORBA_Environment * ev);
static CORBA_short
impl__get_n_anchors (PortableServer_Servant _servant,
		     CORBA_Environment * ev);
static CORBA_long
impl__get_startIndex (PortableServer_Servant _servant,
		      CORBA_Environment * ev);
static CORBA_long
impl__get_endIndex (PortableServer_Servant _servant,
		    CORBA_Environment * ev);
static Accessibility_Accessible
impl_getObject (PortableServer_Servant _servant,
		const CORBA_long i,
		CORBA_Environment * ev);
static CORBA_boolean
impl_isValid (PortableServer_Servant _servant,
	      CORBA_Environment * ev);


BONOBO_TYPE_FUNC_FULL (SpiHyperlink,
		       Accessibility_Hyperlink,
		       SPI_TYPE_BASE,
		       spi_hyperlink)


static void
spi_hyperlink_class_init (SpiHyperlinkClass *klass)
{
  POA_Accessibility_Hyperlink__epv *epv = &klass->epv;

  /* Initialize epv table */

  epv->_get_nAnchors = impl__get_n_anchors;
  epv->getURI = impl_getURI;
  epv->_get_startIndex = impl__get_startIndex;
  epv->_get_endIndex = impl__get_endIndex;
  epv->getObject = impl_getObject;
  epv->isValid = impl_isValid;
}


static void
spi_hyperlink_init (SpiHyperlink *hyperlink)
{
}


SpiHyperlink *
spi_hyperlink_new (AtkHyperlink *object)
{
  SpiHyperlink *new_hyperlink = g_object_new (
	  SPI_HYPERLINK_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_hyperlink), G_OBJECT(object));

  /* 
   * some hyperlinks are actionable... this is an ATK convention 
   * that seems convenient though possibly poorly documented or unintended.
   */
  if (ATK_IS_ACTION (object))
    {
      /* 
       * NOTE: we don't cast 'object' to ATK_OBJECT in the call to 
       * spi_action_interface_new(), because of the above convention, 
       * even though it means we may be violating the func prototype.
       * See discussion in bugzilla bug #120659.
       * !!!
       * IMPORTANT! The 'AtkObject' typecast, instead of the cast macro,
       * is used below, because 'object' may NOT really be an AtkObject;
       * it will be cast back to a G_OBJECT inside spi_action_interface_new
       * before use, so this is OK though very ropey coding style.
       */

	/* Don't aggregate action twice... if this is from AtkHyperlinkImpl */
	if (!bonobo_object_query_interface (bonobo_object (new_hyperlink), "IDL:Accessibility/Action:1.0",
					    NULL))
	    
	    bonobo_object_add_interface (bonobo_object (new_hyperlink),
					 BONOBO_OBJECT (spi_action_interface_new ((AtkObject *) object)));
    }
  return new_hyperlink;
}

static AtkHyperlink *
get_hyperlink_from_servant (PortableServer_Servant servant)
{
  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object != NULL, NULL);
  if (ATK_IS_HYPERLINK(object->gobj)) 
  {
      return ATK_HYPERLINK (object->gobj);
  }
  else if (ATK_IS_HYPERLINK_IMPL(object->gobj))
  {
      return atk_hyperlink_impl_get_hyperlink (ATK_HYPERLINK_IMPL (object->gobj));
  }
  else
      return NULL;
}


static CORBA_short
impl__get_n_anchors (PortableServer_Servant servant,
		     CORBA_Environment     *ev)
{
  AtkHyperlink *link = get_hyperlink_from_servant (servant);

  g_return_val_if_fail (link != NULL, 0);

  return atk_hyperlink_get_n_anchors (link);
}


static CORBA_long
impl__get_startIndex (PortableServer_Servant servant,
		      CORBA_Environment     *ev)
{
  AtkHyperlink *link = get_hyperlink_from_servant (servant);

  g_return_val_if_fail (link != NULL, -1);

  return atk_hyperlink_get_start_index (link);
}


static CORBA_long
impl__get_endIndex (PortableServer_Servant servant,
		    CORBA_Environment     *ev)
{
  AtkHyperlink *link = get_hyperlink_from_servant (servant);

  g_return_val_if_fail (link != NULL, -1);

  return atk_hyperlink_get_end_index (link);
}


static CORBA_string
impl_getURI (PortableServer_Servant servant,
	     const CORBA_long i, CORBA_Environment *ev)
{
  gchar *uri;
  CORBA_char *rv;
  AtkHyperlink *link = get_hyperlink_from_servant (servant);

  g_return_val_if_fail (link != NULL, CORBA_string_dup (""));

  uri = atk_hyperlink_get_uri (link, i);
  if (uri)
    {
      rv = CORBA_string_dup (uri);
      g_free (uri);
    }
  else
    rv = CORBA_string_dup ("");

  return rv;
} 


static Accessibility_Accessible
impl_getObject (PortableServer_Servant servant,
		const CORBA_long       i,
		CORBA_Environment     *ev)
{
  AtkObject    *atk_object;
  AtkHyperlink *link = get_hyperlink_from_servant (servant);

  g_return_val_if_fail (link != NULL, CORBA_OBJECT_NIL);

  atk_object = atk_hyperlink_get_object (link, i);

  return spi_accessible_new_return (atk_object, FALSE, ev);
}


static CORBA_boolean
impl_isValid (PortableServer_Servant servant,
	      CORBA_Environment     *ev)
{
  AtkHyperlink *link = get_hyperlink_from_servant (servant);

  g_return_val_if_fail (link != NULL, TRUE);

  return atk_hyperlink_is_valid (link);
}

