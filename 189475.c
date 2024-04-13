e_named_parameters_new_clone (const ENamedParameters *parameters)
{
	ENamedParameters *clone;

	clone = e_named_parameters_new ();
	if (parameters)
		e_named_parameters_assign (clone, parameters);

	return clone;
}