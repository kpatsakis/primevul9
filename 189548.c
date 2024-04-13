e_named_parameters_exists (const ENamedParameters *parameters,
			   const gchar *name)
{
	g_return_val_if_fail (parameters != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	return get_parameter_index (parameters, name) != -1;
}