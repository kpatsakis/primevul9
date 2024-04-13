byte_array_to_domain (MonoArray *arr, MonoDomain *domain)
{
	MonoArray *copy;

	if (!arr)
		return NULL;

	if (mono_object_domain (arr) == domain)
		return arr;

	copy = mono_array_new (domain, mono_defaults.byte_class, arr->max_length);
	memcpy (mono_array_addr (copy, guint8, 0), mono_array_addr (arr, guint8, 0), arr->max_length);
	return copy;
}