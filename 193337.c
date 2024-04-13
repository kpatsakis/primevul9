set_current_thread_for_domain (MonoDomain *domain, MonoInternalThread *thread, MonoThread *current)
{
	MonoThread **current_thread_ptr = get_current_thread_ptr_for_domain (domain, thread);

	g_assert (current->obj.vtable->domain == domain);

	g_assert (!*current_thread_ptr);
	*current_thread_ptr = current;
}