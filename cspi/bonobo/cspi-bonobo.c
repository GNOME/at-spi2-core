#include "../cspi-lowlevel.h"

#include <libbonobo.h>

void
cspi_dup_ref (CORBA_Object object)
{
  bonobo_object_dup_ref (object, NULL);
}

void
cspi_release_unref (CORBA_Object object)
{
  bonobo_object_release_unref (object, NULL);
}

SPIBoolean
cspi_check_ev (const char *error_string)
{
  CORBA_Environment *ev = cspi_ev ();

  if (ev->_major != CORBA_NO_EXCEPTION)
    {
      char *err;

      err = bonobo_exception_get_text (ev);

      fprintf (stderr, "Warning: AT-SPI error: %s: %s\n",
	       error_string, err);

      g_free (err);

      CORBA_exception_free (ev);

      return FALSE;
    }
  else
    {
      return TRUE;
    }
}

