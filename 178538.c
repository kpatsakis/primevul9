cgi_sort_variables(void)
{
  if (form_count < 2)
    return;

  qsort(form_vars, (size_t)form_count, sizeof(_cgi_var_t),
        (int (*)(const void *, const void *))cgi_compare_variables);
}