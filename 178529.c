cgiSetVariable(const char *name,	/* I - Name of variable */
               const char *value)	/* I - Value of variable */
{
  int		i;			/* Looping var */
  _cgi_var_t	*var;			/* Returned variable */


  if (name == NULL || value == NULL)
    return;

  fprintf(stderr, "cgiSetVariable: %s=\"%s\"\n", name, value);

  if ((var = cgi_find_variable(name)) == NULL)
  {
    cgi_add_variable(name, 0, value);
    cgi_sort_variables();
  }
  else
  {
    for (i = 0; i < var->nvalues; i ++)
      if (var->values[i])
        _cupsStrFree((char *)var->values[i]);

    var->values[0] = _cupsStrAlloc(value);
    var->nvalues   = 1;
  }
}