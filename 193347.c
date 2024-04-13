mono_thread_push_appdomain_ref (MonoDomain *domain)
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	if (thread) {
		/* printf ("PUSH REF: %"G_GSIZE_FORMAT" -> %s.\n", (gsize)thread->tid, domain->friendly_name); */
		SPIN_LOCK (thread->lock_thread_id);
		if (thread->appdomain_refs == NULL)
			thread->appdomain_refs = ref_stack_new (16);
		ref_stack_push (thread->appdomain_refs, domain);
		SPIN_UNLOCK (thread->lock_thread_id);
	}
}