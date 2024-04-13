cgiSetSize(const char *name,		/* I - Name of variable */
           int        size)		/* I - Number of elements (0 to N) */
{
  int		i;			/* Looping var */
  _cgi_var_t	*var;			/* Returned variable */


  if (name == NULL || size < 0 || size > 100000)
    return;

  if ((var = cgi_find_variable(name)) == NULL)
    return;

  if (size >= var->avalues)
  {
    const char **temp;			/* Temporary pointer */

    temp = (const char **)realloc((void *)(var->values),
				  sizeof(char *) * (size_t)(size + 16));
    if (!temp)
      return;

    var->avalues = size + 16;
    var->values  = temp;
  }

  if (size > var->nvalues)
  {
    for (i = var->nvalues; i < size; i ++)
      var->values[i] = NULL;
  }
  else if (size < var->nvalues)
  {
    for (i = size; i < var->nvalues; i ++)
      if (var->values[i])
        _cupsStrFree((void *)(var->values[i]));
  }

  var->nvalues = size;
}