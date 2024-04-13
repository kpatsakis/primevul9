void mono_thread_manage (void)
{
	struct wait_data wait_data;
	struct wait_data *wait = &wait_data;

	memset (wait, 0, sizeof (struct wait_data));
	/* join each thread that's still running */
	THREAD_DEBUG (g_message ("%s: Joining each running thread...", __func__));
	
	mono_threads_lock ();
	if(threads==NULL) {
		THREAD_DEBUG (g_message("%s: No threads", __func__));
		mono_threads_unlock ();
		return;
	}
	mono_threads_unlock ();
	
	do {
		mono_threads_lock ();
		if (shutting_down) {
			/* somebody else is shutting down */
			mono_threads_unlock ();
			break;
		}
		THREAD_DEBUG (g_message ("%s: There are %d threads to join", __func__, mono_g_hash_table_size (threads));
			mono_g_hash_table_foreach (threads, print_tids, NULL));
	
		ResetEvent (background_change_event);
		wait->num=0;
		/*We must zero all InternalThread pointers to avoid making the GC unhappy.*/
		memset (wait->threads, 0, MAXIMUM_WAIT_OBJECTS * SIZEOF_VOID_P);
		mono_g_hash_table_foreach (threads, build_wait_tids, wait);
		mono_threads_unlock ();
		if(wait->num>0) {
			/* Something to wait for */
			wait_for_tids_or_state_change (wait, INFINITE);
		}
		THREAD_DEBUG (g_message ("%s: I have %d threads after waiting.", __func__, wait->num));
	} while(wait->num>0);

	mono_threads_set_shutting_down ();

	/* No new threads will be created after this point */

	mono_runtime_set_shutting_down ();

	THREAD_DEBUG (g_message ("%s: threadpool cleanup", __func__));
	mono_thread_pool_cleanup ();

	/* 
	 * Remove everything but the finalizer thread and self.
	 * Also abort all the background threads
	 * */
	do {
		mono_threads_lock ();

		wait->num = 0;
		/*We must zero all InternalThread pointers to avoid making the GC unhappy.*/
		memset (wait->threads, 0, MAXIMUM_WAIT_OBJECTS * SIZEOF_VOID_P);
		mono_g_hash_table_foreach_remove (threads, remove_and_abort_threads, wait);

		mono_threads_unlock ();

		THREAD_DEBUG (g_message ("%s: wait->num is now %d", __func__, wait->num));
		if(wait->num>0) {
			/* Something to wait for */
			wait_for_tids (wait, INFINITE);
		}
	} while (wait->num > 0);
	
	/* 
	 * give the subthreads a chance to really quit (this is mainly needed
	 * to get correct user and system times from getrusage/wait/time(1)).
	 * This could be removed if we avoid pthread_detach() and use pthread_join().
	 */
#ifndef HOST_WIN32
	sched_yield ();
#endif
}