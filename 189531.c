e_named_parameters_clear (ENamedParameters *parameters)
{
	GPtrArray *array;
	g_return_if_fail (parameters != NULL);

	array = (GPtrArray *) parameters;

	if (array->len)
		g_ptr_array_remove_range (array, 0, array->len);
}