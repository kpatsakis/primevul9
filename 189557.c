e_named_parameters_ref (ENamedParameters *params)
{
	return (ENamedParameters *) g_ptr_array_ref ((GPtrArray *) params);
}