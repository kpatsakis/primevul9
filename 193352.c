static void thread_cleanup (MonoInternalThread *thread)
{
	g_assert (thread != NULL);

	if (thread->abort_state_handle) {
		mono_gchandle_free (thread->abort_state_handle);
		thread->abort_state_handle = 0;
	}
	thread->abort_exc = NULL;
	thread->current_appcontext = NULL;

	/*
	 * This is necessary because otherwise we might have
	 * cross-domain references which will not get cleaned up when
	 * the target domain is unloaded.
	 */
	if (thread->cached_culture_info) {
		int i;
		for (i = 0; i < NUM_CACHED_CULTURES * 2; ++i)
			mono_array_set (thread->cached_culture_info, MonoObject*, i, NULL);
	}

	/* if the thread is not in the hash it has been removed already */
	if (!handle_remove (thread)) {
		/* This needs to be called even if handle_remove () fails */
		if (mono_thread_cleanup_fn)
			mono_thread_cleanup_fn (thread);
		return;
	}
	mono_release_type_locks (thread);

	EnterCriticalSection (thread->synch_cs);

	thread->state |= ThreadState_Stopped;
	thread->state &= ~ThreadState_Background;

	LeaveCriticalSection (thread->synch_cs);
	
	mono_profiler_thread_end (thread->tid);

	if (thread == mono_thread_internal_current ())
		mono_thread_pop_appdomain_ref ();

	thread->cached_culture_info = NULL;

	mono_free_static_data (thread->static_data, TRUE);
	thread->static_data = NULL;
	ref_stack_destroy (thread->appdomain_refs);
	thread->appdomain_refs = NULL;

	if (mono_thread_cleanup_fn)
		mono_thread_cleanup_fn (thread);

	mono_thread_small_id_free (thread->small_id);
	MONO_GC_UNREGISTER_ROOT (thread->thread_pinning_ref);
	thread->small_id = -2;
}