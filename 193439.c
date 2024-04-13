init_root_domain_thread (MonoInternalThread *thread, MonoThread *candidate)
{
	MonoDomain *domain = mono_get_root_domain ();

	if (!candidate || candidate->obj.vtable->domain != domain)
		candidate = new_thread_with_internal (domain, thread);
	set_current_thread_for_domain (domain, thread, candidate);
	g_assert (!thread->root_domain_thread);
	MONO_OBJECT_SETREF (thread, root_domain_thread, candidate);
}