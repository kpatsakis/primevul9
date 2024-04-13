e_named_parameters_assign (ENamedParameters *parameters,
                           const ENamedParameters *from)
{
	g_return_if_fail (parameters != NULL);

	e_named_parameters_clear (parameters);

	if (from) {
		gint ii;
		GPtrArray *from_array = (GPtrArray *) from;

		for (ii = 0; ii < from_array->len; ii++) {
			g_ptr_array_add (
				(GPtrArray *) parameters,
				g_strdup (from_array->pdata[ii]));
		}
	}
}