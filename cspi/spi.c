#include <libbonobo.h>
#include <stdio.h>
#include "spi.h"

static CORBA_Environment ev;
static AccessibilityRegistry registry;

static Accessible *
Obj_Add (Accessible object)
{
  /* TODO: keep list of live object refs */
  Accessible *oref = g_malloc (sizeof (Accessible));
  *oref = object;
  return oref;
}

/* temporary hack until we restructure these sources */

#include "spi_main.c"
#include "spi_event.c"
#include "spi_registry.c"
#include "spi_application.c"
#include "spi_accessible.c"
#include "spi_component.c"
