#include <stdlib.h>
#include <cspi/spi-private.h>

/**
 * SPI_freeString:
 * @s: a character string returned from another at-spi call.
 *
 * Free a character string returned from an at-spi call.  Clients of
 * at-spi should use this function instead of free () or g_free().
 * This API should not be used to free strings
 * from other libraries or allocated by the client.
 **/
void
SPI_freeString (char *s)
{
  CORBA_free (s);
}

boolean
cspi_warn_ev (CORBA_Environment *ev, const char *error_string)
{
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

void
cspi_check_ev (CORBA_Environment *ev, const char *error_string)
{
  if (ev->_major != CORBA_NO_EXCEPTION)
    {
      char *err;

      err = bonobo_exception_get_text (ev);

      fprintf (stderr, "AT-SPI error: %s: %s\n",
	       error_string, err);

      g_free (err);

      CORBA_exception_free (ev);

      exit (-1);
    }
}
