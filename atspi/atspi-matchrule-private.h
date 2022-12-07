/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 * Copyright 2010, 2011 Novell, Inc.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _ATSPI_MATCH_RULE_PRIVATE_H_
#define _ATSPI_MATCH_RULE_PRIVATE_H_

#include "glib-object.h"

#include "atspi-matchrule.h"
#include "dbus/dbus.h"

G_BEGIN_DECLS

gboolean
_atspi_match_rule_marshal (AtspiMatchRule *rule, DBusMessageIter *iter);

G_END_DECLS

#endif /* _ATSPI_MATCH_RULE_PRIVATE_H_ */
