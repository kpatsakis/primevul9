e_named_parameters_to_string (const ENamedParameters *parameters)
{
	gchar **strv, *str;
	gint ii;

	strv = e_named_parameters_to_strv (parameters);
	if (!strv)
		return NULL;

	for (ii = 0; strv[ii]; ii++) {
		gchar *name_and_value = strv[ii];

		strv[ii] = g_strescape (name_and_value, "");
		g_free (name_and_value);
	}

	str = g_strjoinv ("\n", strv);

	g_strfreev (strv);

	return str;
}