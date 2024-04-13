e_named_parameters_test (const ENamedParameters *parameters,
                         const gchar *name,
                         const gchar *value,
                         gboolean case_sensitively)
{
	const gchar *stored_value;

	g_return_val_if_fail (parameters != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	stored_value = e_named_parameters_get (parameters, name);
	if (!stored_value)
		return FALSE;

	if (case_sensitively)
		return strcmp (stored_value, value) == 0;

	return g_ascii_strcasecmp (stored_value, value) == 0;
}