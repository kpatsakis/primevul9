e_named_parameters_count (const ENamedParameters *parameters)
{
	g_return_val_if_fail (parameters != NULL, 0);

	return ((GPtrArray *) parameters)->len;
}