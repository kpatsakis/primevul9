cgiGetVariable(const char *name)	/* I - Name of variable */
{
  const _cgi_var_t	*var;		/* Returned variable */


  var = cgi_find_variable(name);

  return ((var == NULL) ? NULL : _cupsStrRetain(var->values[var->nvalues - 1]));
}