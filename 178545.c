cgi_compare_variables(
    const _cgi_var_t *v1,		/* I - First variable */
    const _cgi_var_t *v2)		/* I - Second variable */
{
  return (_cups_strcasecmp(v1->name, v2->name));
}