cgiGetArray(const char *name,		/* I - Name of array variable */
            int        element)		/* I - Element number (0 to N) */
{
  _cgi_var_t	*var;			/* Pointer to variable */


  if ((var = cgi_find_variable(name)) == NULL)
    return (NULL);

  if (element < 0 || element >= var->nvalues)
    return (NULL);

  return (_cupsStrRetain(var->values[element]));
}