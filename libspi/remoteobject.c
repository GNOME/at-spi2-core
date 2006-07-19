/* AT-SPI : Assistive Technology Service Provider Interface
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

#include <config.h>
#include "remoteobject.h"

Accessibility_Accessible
spi_remote_object_get_accessible (SpiRemoteObject *remote)
{
  SpiRemoteObjectIface *iface;	

  g_return_val_if_fail (SPI_IS_REMOTE_OBJECT (remote), CORBA_OBJECT_NIL);
  
  iface = SPI_REMOTE_OBJECT_GET_IFACE (remote);

  if (!iface->get_accessible)
    return CORBA_OBJECT_NIL;

  return iface->get_accessible (remote);
}

GType
spi_remote_object_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (SpiRemoteObjectIface),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) NULL,
        (GClassFinalizeFunc) NULL,
        NULL, 0, 0,
        (GInstanceInitFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_INTERFACE, "SpiRemoteObject", &typeInfo, 0) ;
    }

  return type;
}
