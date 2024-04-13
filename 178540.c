cgiClearVariables(void)
{
  int		i, j;			/* Looping vars */
  _cgi_var_t	*v;			/* Current variable */


  fputs("DEBUG: cgiClearVariables called.\n", stderr);

  for (v = form_vars, i = form_count; i > 0; v ++, i --)
  {
    _cupsStrFree(v->name);
    for (j = 0; j < v->nvalues; j ++)
      if (v->values[j])
        _cupsStrFree(v->values[j]);
  }

  form_count = 0;

  cgi_unlink_file();
}