/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 *           
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

#ifndef _SPI_PRIVATE_H_
#define _SPI_PRIVATE_H_

/* Private internal implementation details of at-spi. */

#include <spi-common/spi-dbus.h>
#include <cspi/spi.h>
#include <string.h>
#include "cspi/cspi-lowlevel.h"
#include "cspi/spi-listener.h"
#include "dbind/dbind.h"
#include <glib-object.h>

typedef struct _CSpiApplication CSpiApplication;
struct _CSpiApplication
{
  GHashTable *hash;
  char *bus_name;
};

struct _Accessible {
  gint ref_count;
  Accessible *parent;
  GList *children;
	CSpiApplication *app;
	union
	{
	  gint id;
	  char *path;
	} v;
  gint role : 8;
  gint interfaces : 24;
  char *name;
  char *description;
};

struct _AccessibleStateSet {
	guint   ref_count;
	GArray *states;
};

#define SPI_INTERNAL_EVENT_MAGIC 0xc3
/* 
 * For internal use by CSPI implementation only
 */
typedef struct {
  AccessibleEvent event;
  guint           id;
  guchar          magic;
  guchar          type;
  guint16         ref_count;
} InternalEvent;

struct _SPIException {
  SPIExceptionType type;
  Accessible *source;
  DBusError *error;
  SPIExceptionCode code;
  char * desc;
};

DBusConnection *SPI_bus (void);
SPIBoolean             cspi_exception         (void);
Accessible            *cspi_object_add (Accessible  *accessible);
void                   cspi_object_ref        (Accessible  *accessible);
void                   cspi_object_unref      (Accessible  *accessible);
SPIBoolean             cspi_accessible_is_a   (Accessible  *accessible,
					       const char  *interface_name);
AccessibleRole         cspi_role_from_spi_role (Accessibility_Role role);
void                   cspi_streams_close_all (void);
gboolean               cspi_exception_throw (DBusError *error, const char *desc_prefix);

AccessibleAttributeSet 
                     *_cspi_attribute_set_from_sequence (const GArray *seq);
#define cspi_return_if_fail(val)		\
	if (!(val))				\
		return
#define cspi_return_val_if_fail(val, ret)	\
	if (!(val))				\
		return (ret)

#define cspi_return_if_ev(err)			\
	if (cspi_exception ()) \
		return;
#define cspi_return_val_if_ev(err, ret)	\
	if (cspi_exception ()) \
		return (ret);

typedef struct _Accessibility_BoundingBox Accessibility_BoundingBox;
struct _Accessibility_BoundingBox
{
  dbus_int32_t x;
  dbus_int32_t y;
  dbus_int32_t width;
  dbus_int32_t height;
};

#endif /* _SPI_PRIVATE_H_ */
