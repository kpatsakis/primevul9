e_named_parameters_get (const ENamedParameters *parameters,
                        const gchar *name)
{
	gint index;
	const gchar *name_and_value;

	g_return_val_if_fail (parameters != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);

	index = get_parameter_index (parameters, name);
	if (index == -1)
		return NULL;

	name_and_value = g_ptr_array_index ((GPtrArray *) parameters, index);

	return name_and_value + strlen (name) + 1;
}