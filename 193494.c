mono_thread_current (void)
{
	MonoDomain *domain = mono_domain_get ();
	MonoInternalThread *internal = mono_thread_internal_current ();
	MonoThread **current_thread_ptr;

	g_assert (internal);
	current_thread_ptr = get_current_thread_ptr_for_domain (domain, internal);

	if (!*current_thread_ptr) {
		g_assert (domain != mono_get_root_domain ());
		*current_thread_ptr = new_thread_with_internal (domain, internal);
	}
	return *current_thread_ptr;
}