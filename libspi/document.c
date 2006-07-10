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

/* document.c: implements the Document interface */


#include <config.h>
#include <stdio.h>
#include <bonobo/bonobo-exception.h>
#include <libspi/document.h>
#include <libspi/spi-private.h>

SpiDocument *
spi_document_interface_new (AtkObject *obj)
{
  SpiDocument *new_document = g_object_new (SPI_DOCUMENT_TYPE, NULL);

  spi_base_construct (SPI_BASE (new_document), G_OBJECT(obj));

  return new_document;

}

static AtkDocument *
get_document_from_servant (PortableServer_Servant servant)
{

  SpiBase *object = SPI_BASE (bonobo_object_from_servant (servant));

  g_return_val_if_fail (object, NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (object->gobj), NULL);

  return ATK_DOCUMENT (object->gobj);

}

static CORBA_string
impl_getLocale (PortableServer_Servant servant,
		CORBA_Environment *ev)
{
  const gchar *lc;
  AtkDocument *document = get_document_from_servant (servant);

  g_return_val_if_fail (document != NULL, "");

  lc = atk_document_get_locale (document);

  if (lc)
    return CORBA_string_dup (lc);
  else
      return CORBA_string_dup (""); /* Should we return 'C' by default? */
}

static CORBA_string 
impl_getAttributeValue (PortableServer_Servant servant,
			const CORBA_char *attributename,
			CORBA_Environment *ev){

  const gchar *atr;
  
  AtkDocument *document = get_document_from_servant (servant);
   
  g_return_val_if_fail (document != NULL, "");

  atr = atk_document_get_attribute_value (document, attributename);

  if (atr)
    return CORBA_string_dup (atr);
  else
    return CORBA_string_dup ("");
}


static CORBA_string
impl_getAttributes (PortableServer_Servant servant,
		    CORBA_Environment *ev){
  
  AtkDocument *document = get_document_from_servant (servant);
  AtkAttributeSet *attributes = NULL;
  Accessibility_AttributeSet *retval;
  gint n_attributes = 0;
  gint i;
  
  g_return_val_if_fail (document != NULL, CORBA_string_dup (""));
  
  attributes = atk_document_get_attributes (document);
  
  bonobo_return_val_if_fail (attributes != NULL, NULL, ev);

  bonobo_return_val_if_fail (attributes != NULL, NULL, ev);

  /* according to atkobject.h, AtkAttributeSet is a GSList */
  n_attributes = g_slist_length (attributes);
    
  retval = CORBA_sequence_CORBA_string__alloc ();
  retval->_length = retval->_maximum = n_attributes;
  retval->_buffer = CORBA_sequence_CORBA_string_allocbuf (n_attributes);
  CORBA_sequence_set_release (retval, CORBA_TRUE);
  
  for (i = 0; i < n_attributes; ++i)
  {
      retval->_buffer[i] = CORBA_string_dup (g_slist_nth_data (attributes, i));
  }
    
  atk_attribute_set_free (attributes);

  return retval;
  
}


static void
spi_document_class_init (SpiDocumentClass *klass)
{

 
  POA_Accessibility_Document__epv *epv = &klass->epv;

  epv->getLocale = impl_getLocale;
  epv->getAttributeValue = impl_getAttributeValue;
  epv->getAttributes = impl_getAttributes;

}

static void
spi_document_init (SpiDocument *document)
{
}
BONOBO_TYPE_FUNC_FULL (SpiDocument,
		       Accessibility_Document,
		       SPI_TYPE_BASE,
		       spi_document)
