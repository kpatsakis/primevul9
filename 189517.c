e_named_parameters_new (void)
{
	return (ENamedParameters *) g_ptr_array_new_with_free_func ((GDestroyNotify) e_util_safe_free_string);
}