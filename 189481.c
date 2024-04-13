get_parameter_index (const ENamedParameters *parameters,
                     const gchar *name)
{
	GPtrArray *array;
	gint ii, name_len;

	g_return_val_if_fail (parameters != NULL, -1);
	g_return_val_if_fail (name != NULL, -1);

	name_len = strlen (name);

	array = (GPtrArray *) parameters;

	for (ii = 0; ii < array->len; ii++) {
		const gchar *name_and_value = g_ptr_array_index (array, ii);

		if (name_and_value == NULL || strlen (name_and_value) <= name_len)
			continue;

		if (name_and_value[name_len] != ':')
			continue;

		if (g_ascii_strncasecmp (name_and_value, name, name_len) == 0)
			return ii;
	}

	return -1;
}