mono_thread_pop_appdomain_ref (void)
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	if (thread) {
		/* printf ("POP REF: %"G_GSIZE_FORMAT" -> %s.\n", (gsize)thread->tid, ((MonoDomain*)(thread->appdomain_refs->data))->friendly_name); */
		SPIN_LOCK (thread->lock_thread_id);
		ref_stack_pop (thread->appdomain_refs);
		SPIN_UNLOCK (thread->lock_thread_id);
	}
}