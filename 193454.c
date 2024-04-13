get_execution_context_addr (void)
{
	MonoDomain *domain = mono_domain_get ();
	guint32 offset;

	if (!execution_context_field) {
		execution_context_field = mono_class_get_field_from_name (mono_defaults.thread_class,
				"_ec");
		g_assert (execution_context_field);
	}

	g_assert (mono_class_try_get_vtable (domain, mono_defaults.appdomain_class));

	mono_domain_lock (domain);
	offset = GPOINTER_TO_UINT (g_hash_table_lookup (domain->special_static_fields, execution_context_field));
	mono_domain_unlock (domain);
	g_assert (offset);

	return (MonoObject**) mono_get_special_static_data (offset);
}