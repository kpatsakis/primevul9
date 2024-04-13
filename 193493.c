get_current_thread_ptr_for_domain (MonoDomain *domain, MonoInternalThread *thread)
{
	static MonoClassField *current_thread_field = NULL;

	guint32 offset;

	if (!current_thread_field) {
		current_thread_field = mono_class_get_field_from_name (mono_defaults.thread_class, "current_thread");
		g_assert (current_thread_field);
	}

	mono_class_vtable (domain, mono_defaults.thread_class);
	mono_domain_lock (domain);
	offset = GPOINTER_TO_UINT (g_hash_table_lookup (domain->special_static_fields, current_thread_field));
	mono_domain_unlock (domain);
	g_assert (offset);

	return get_thread_static_data (thread, offset);
}