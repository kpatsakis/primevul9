e_named_parameters_set (ENamedParameters *parameters,
                        const gchar *name,
                        const gchar *value)
{
	GPtrArray *array;
	gint index;
	gchar *name_and_value;

	g_return_if_fail (parameters != NULL);
	g_return_if_fail (name != NULL);
	g_return_if_fail (strchr (name, ':') == NULL);
	g_return_if_fail (*name != '\0');

	array = (GPtrArray *) parameters;

	index = get_parameter_index (parameters, name);
	if (!value) {
		if (index != -1)
			g_ptr_array_remove_index (array, index);
		return;
	}

	name_and_value = g_strconcat (name, ":", value, NULL);
	if (index != -1) {
		g_free (array->pdata[index]);
		array->pdata[index] = name_and_value;
	} else {
		g_ptr_array_add (array, name_and_value);
	}
}