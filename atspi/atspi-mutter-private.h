/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2019 SUSE LLC.
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

#ifndef _ATSPI_MUTTER_H_
#define _ATSPI_MUTTER_H_

G_BEGIN_DECLS

gboolean _atspi_mutter_generate_keyboard_event (glong keyval, const gchar *keystring, AtspiKeySynthType synth_type, GError **error);

gboolean _atspi_mutter_generate_mouse_event (glong x, glong y, const gchar *name, GError **error);

void _atspi_mutter_set_reference_window (AtspiAccessible *accessible);
G_END_DECLS

#endif /* _ATSPI_MUTTER_H_ */
