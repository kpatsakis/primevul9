e_named_parameters_new_string (const gchar *str)
{
	ENamedParameters *parameters;
	gchar **split;
	gint ii;

	g_return_val_if_fail (str != NULL, NULL);

	split = g_strsplit (str, "\n", -1);

	parameters = e_named_parameters_new ();
	for (ii = 0; split && split[ii]; ii++) {
		g_ptr_array_add ((GPtrArray *) parameters, g_strcompress (split[ii]));
	}

	g_strfreev (split);

	return parameters;
}