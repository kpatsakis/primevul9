get_schema (const gchar *name)
{
  const char * const *schemas = NULL;
  gint i;

  schemas = g_settings_list_schemas ();
  for (i = 0; schemas[i]; i++)
  {
    if (!strcmp (schemas[i], name))
      return g_settings_new (schemas[i]);
  }

  return NULL;
}