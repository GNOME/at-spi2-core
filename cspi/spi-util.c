void
spi_freeString (char *s)
{
  CORBA_free (s);
}

boolean
spi_warn_ev (CORBA_Environment *c_ev, char *error_string) {

  if (c_ev->_major != CORBA_NO_EXCEPTION) {
    fprintf (stderr,
            "Warning: AT-SPI error: %s: %s\n",
	     error_string,
             CORBA_exception_id(c_ev));
        CORBA_exception_init (c_ev);
        return FALSE;
  }
  else
  {
        return TRUE;
  }
}

void
spi_check_ev (CORBA_Environment *c_ev, char *error_string) {

  if (c_ev->_major != CORBA_NO_EXCEPTION) {
    fprintf (stderr,
            ("AT-SPI error: %s: %s\n"),
	     error_string,
            CORBA_exception_id(c_ev));
    CORBA_exception_free(c_ev);
    exit(-1);
  }
}
