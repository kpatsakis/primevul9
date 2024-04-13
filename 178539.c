cgiGetSize(const char *name)		/* I - Name of variable */
{
  _cgi_var_t	*var;			/* Pointer to variable */


  if ((var = cgi_find_variable(name)) == NULL)
    return (0);

  return (var->nvalues);
}