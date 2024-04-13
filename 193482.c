mono_thread_has_appdomain_ref (MonoThread *thread, MonoDomain *domain)
{
	return mono_thread_internal_has_appdomain_ref (thread->internal_thread, domain);
}