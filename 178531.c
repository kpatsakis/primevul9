cgiSetArray(const char *name,		/* I - Name of variable */
            int        element,		/* I - Element number (0 to N) */
            const char *value)		/* I - Value of variable */
{
  int		i;			/* Looping var */
  _cgi_var_t	*var;			/* Returned variable */


  if (name == NULL || value == NULL || element < 0 || element > 100000)
    return;

  fprintf(stderr, "DEBUG: cgiSetArray: %s[%d]=\"%s\"\n", name, element, value);

  if ((var = cgi_find_variable(name)) == NULL)
  {
    cgi_add_variable(name, element, value);
    cgi_sort_variables();
  }
  else
  {
    if (element >= var->avalues)
    {
      const char **temp;		/* Temporary pointer */

      temp = (const char **)realloc((void *)(var->values),
                                    sizeof(char *) * (size_t)(element + 16));
      if (!temp)
        return;

      var->avalues = element + 16;
      var->values  = temp;
    }

    if (element >= var->nvalues)
    {
      for (i = var->nvalues; i < element; i ++)
	var->values[i] = NULL;

      var->nvalues = element + 1;
    }
    else if (var->values[element])
      _cupsStrFree((char *)var->values[element]);

    var->values[element] = _cupsStrAlloc(value);
  }
}