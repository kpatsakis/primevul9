cgi_add_variable(const char *name,	/* I - Variable name */
		 int        element,	/* I - Array element number */
                 const char *value)	/* I - Variable value */
{
  _cgi_var_t	*var;			/* New variable */


  if (name == NULL || value == NULL || element < 0 || element > 100000)
    return;

  if (form_count >= form_alloc)
  {
    _cgi_var_t	*temp_vars;		/* Temporary form pointer */


    if (form_alloc == 0)
      temp_vars = malloc(sizeof(_cgi_var_t) * 16);
    else
      temp_vars = realloc(form_vars, (size_t)(form_alloc + 16) * sizeof(_cgi_var_t));

    if (!temp_vars)
      return;

    form_vars  = temp_vars;
    form_alloc += 16;
  }

  var = form_vars + form_count;

  if ((var->values = calloc((size_t)element + 1, sizeof(char *))) == NULL)
    return;

  var->name            = _cupsStrAlloc(name);
  var->nvalues         = element + 1;
  var->avalues         = element + 1;
  var->values[element] = _cupsStrAlloc(value);

  form_count ++;
}