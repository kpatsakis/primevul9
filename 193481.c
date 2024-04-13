mono_thread_internal_has_appdomain_ref (MonoInternalThread *thread, MonoDomain *domain)
{
	gboolean res;
	SPIN_LOCK (thread->lock_thread_id);
	res = ref_stack_find (thread->appdomain_refs, domain);
	SPIN_UNLOCK (thread->lock_thread_id);
	return res;
}