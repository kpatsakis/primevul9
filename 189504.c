e_named_parameters_get_name (const ENamedParameters *parameters,
			     gint index)
{
	const gchar *name_and_value, *colon;

	g_return_val_if_fail (parameters != NULL, NULL);
	g_return_val_if_fail (index >= 0 && index < e_named_parameters_count (parameters), NULL);

	name_and_value = g_ptr_array_index ((GPtrArray *) parameters, index);
	colon = name_and_value ? strchr (name_and_value, ':') : NULL;

	if (!colon || colon == name_and_value)
		return NULL;

	return g_strndup (name_and_value, colon - name_and_value);
}