/* AT-SPI : Assistive Technology Service Provider Interface
 * Copyright 2001 Sun Microsystems Inc.
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

#include "remoteobject.h"

static Accessibility_Accessible
impl_spi_remote_object_get_accessible (SpiRemoteObject *o) /* default impl */
{
  return CORBA_OBJECT_NIL;
}

Accessibility_Accessible
spi_remote_object_get_accessible (SpiRemoteObject *o)
{
  SpiRemoteObjectClass *klass;	
  g_assert (SPI_IS_REMOTE_OBJECT (o));
  
  klass = SPI_REMOTE_OBJECT_GET_CLASS (o);
  if (klass->get_accessible)
    return (klass->get_accessible) (o);
  else
    return CORBA_OBJECT_NIL;
}

static void
spi_remote_object_class_init (AtkObjectClass *klass)
{
  SpiRemoteObjectClass *parent_class = SPI_REMOTE_OBJECT_CLASS (klass);
  parent_class->get_accessible = impl_spi_remote_object_get_accessible;
} 

GType
spi_remote_object_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (SpiRemoteObjectClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) spi_remote_object_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (SpiRemoteObject),
        0,
        (GInstanceInitFunc) NULL,
      } ;
      type = g_type_register_static (ATK_TYPE_OBJECT, "SpiRemoteObject", &typeInfo, 0) ;
    }
  return type;
}

