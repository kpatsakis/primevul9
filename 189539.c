e_named_parameters_free (ENamedParameters *parameters)
{
	if (!parameters)
		return;

	g_ptr_array_unref ((GPtrArray *) parameters);
}