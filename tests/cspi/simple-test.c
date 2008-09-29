/*
 * Copyright (C) 2008 Codethink Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <glib.h>
#include <cspi/spi.h>

#include <stdio.h>

int
main (int argc, char **argv)
{
	gint i;
	gint n_desktops;
	gchar *s;

	SPI_init ();

	n_desktops = SPI_getDesktopCount ();

	for (i=0; i<n_desktops; ++i) {
		Accessible *desktop;

		desktop = SPI_getDesktop (i);
		s = Accessible_getName (desktop);
		fprintf (stderr, "desktop %d name: %s\n", i, s);
		SPI_freeString (s);
		Accessible_unref (desktop);
	}
	return SPI_exit ();
}
