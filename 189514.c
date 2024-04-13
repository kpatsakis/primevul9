e_named_parameters_unref (ENamedParameters *params)
{
	g_ptr_array_unref ((GPtrArray *) params);
}