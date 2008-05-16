/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2007 IBM Corp.
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

/* matchrule.c: implements the Matchrule interface */


#include <config.h>
#include <stdio.h>
#include <bonobo/bonobo-exception.h>
#include <libspi/matchrule.h> 
#include <libspi/spi-private.h>

SpiMatchrule *
spi_matchrule_interface_new ()
{

     SpiMatchrule *new_matchrule = g_object_new (SPI_MATCHRULE_TYPE, NULL);

     spi_base_construct_default (SPI_BASE (new_matchrule));

     return new_matchrule;

}

static void
spi_matchrule_class_init (SpiMatchruleClass *klass)
{
}

static void
spi_matchrule_init (SpiMatchrule *matchrule)
{
}

BONOBO_TYPE_FUNC_FULL (SpiMatchrule,
		      Accessibility_MatchRule,
		      SPI_TYPE_BASE,
		      spi_matchrule)


