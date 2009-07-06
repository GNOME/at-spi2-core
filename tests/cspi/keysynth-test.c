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

#include <unistd.h>
#include <stdlib.h>
#include "../../cspi/spi-private.h" /* A hack for now */
#include <glib-object.h>
#include <gtk/gtk.h>
#include <atk/atk.h>
#include <atk/atknoopobject.h>
#include "common/spi-dbus.h"

typedef struct {
	long int val;
	char *string;
	AccessibleKeySynthType type;
} TextTest;

static TextTest text[] = {
	{65, NULL, SPI_KEY_PRESSRELEASE},
	{64, NULL, SPI_KEY_SYM},
	{0,  "--hello!", SPI_KEY_STRING},
	{0, "StudlyCaps!", SPI_KEY_STRING}
};

static void
test_key_synthesis (void)
{
	int i;
	for (i = 0; i < G_N_ELEMENTS (text); ++i) {
		SPI_generateKeyboardEvent (text[i].val, text[i].string, text[i].type);
	}
}

int
main (int argc, char **argv)
{
	gtk_init (&argc, &argv);
	SPI_init ();

	test_key_synthesis ();

	return SPI_exit ();
}

