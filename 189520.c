e_named_parameters_to_strv (const ENamedParameters *parameters)
{
	GPtrArray *array = (GPtrArray *) parameters;
	GPtrArray *ret = g_ptr_array_new ();

	if (array) {
		guint i;
		for (i = 0; i < array->len; i++) {
			g_ptr_array_add (ret, g_strdup (array->pdata[i]));
		}
	}

	g_ptr_array_add (ret, NULL);

	return (gchar **) g_ptr_array_free (ret, FALSE);
}