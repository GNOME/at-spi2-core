#include <string.h>
#include "spi-util.h"


char *
string_from_corba_string (CORBA_char *string)
{
  char *newstring;
  int length;

  length = strlen (string);
  newstring = (char *) malloc (length+1);
  strcpy (newstring, string);
  corba_free (string);
  return newstring;
}

