static void mono_thread_start (MonoThread *thread)
{
	MonoInternalThread *internal = thread->internal_thread;

	THREAD_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Launching thread %p (%"G_GSIZE_FORMAT")", __func__, GetCurrentThreadId (), internal, (gsize)internal->tid));

	/* Only store the handle when the thread is about to be
	 * launched, to avoid the main thread deadlocking while trying
	 * to clean up a thread that will never be signalled.
	 */
	if (!handle_store (thread))
		return;

	ResumeThread (internal->handle);

	if(internal->start_notify!=NULL) {
		/* Wait for the thread to set up its TLS data etc, so
		 * theres no potential race condition if someone tries
		 * to look up the data believing the thread has
		 * started
		 */

		THREAD_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") waiting for thread %p (%"G_GSIZE_FORMAT") to start", __func__, GetCurrentThreadId (), internal, (gsize)internal->tid));

		WaitForSingleObjectEx (internal->start_notify, INFINITE, FALSE);
		CloseHandle (internal->start_notify);
		internal->start_notify = NULL;
	}

	THREAD_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Done launching thread %p (%"G_GSIZE_FORMAT")", __func__, GetCurrentThreadId (), internal, (gsize)internal->tid));
}