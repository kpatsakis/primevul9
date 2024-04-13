e_named_parameters_new_strv (const gchar * const *strv)
{
	ENamedParameters *parameters;
	gint ii;

	g_return_val_if_fail (strv != NULL, NULL);

	parameters = e_named_parameters_new ();
	for (ii = 0; strv[ii]; ii++) {
		g_ptr_array_add ((GPtrArray *) parameters, g_strdup (strv[ii]));
	}

	return parameters;
}