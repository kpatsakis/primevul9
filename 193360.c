mono_threads_request_thread_dump (void)
{
	struct wait_data wait_data;
	struct wait_data *wait = &wait_data;
	int i;

	memset (wait, 0, sizeof (struct wait_data));

	/* 
	 * Make a copy of the hashtable since we can't do anything with
	 * threads while threads_mutex is held.
	 */
	mono_threads_lock ();
	mono_g_hash_table_foreach (threads, collect_threads, wait);
	mono_threads_unlock ();

	for (i = 0; i < wait->num; ++i) {
		MonoInternalThread *thread = wait->threads [i];

		if (!mono_gc_is_finalizer_internal_thread (thread) &&
				(thread != mono_thread_internal_current ()) &&
				!thread->thread_dump_requested) {
			thread->thread_dump_requested = TRUE;

			signal_thread_state_change (thread);
		}

		CloseHandle (wait->handles [i]);
	}
}