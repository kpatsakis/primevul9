static gboolean handle_remove(MonoInternalThread *thread)
{
	gboolean ret;
	gsize tid = thread->tid;

	THREAD_DEBUG (g_message ("%s: thread ID %"G_GSIZE_FORMAT, __func__, tid));

	mono_threads_lock ();

	if (threads) {
		/* We have to check whether the thread object for the
		 * tid is still the same in the table because the
		 * thread might have been destroyed and the tid reused
		 * in the meantime, in which case the tid would be in
		 * the table, but with another thread object.
		 */
		if (mono_g_hash_table_lookup (threads, (gpointer)tid) == thread) {
			mono_g_hash_table_remove (threads, (gpointer)tid);
			ret = TRUE;
		} else {
			ret = FALSE;
		}
	}
	else
		ret = FALSE;
	
	mono_threads_unlock ();

	/* Don't close the handle here, wait for the object finalizer
	 * to do it. Otherwise, the following race condition applies:
	 *
	 * 1) Thread exits (and handle_remove() closes the handle)
	 *
	 * 2) Some other handle is reassigned the same slot
	 *
	 * 3) Another thread tries to join the first thread, and
	 * blocks waiting for the reassigned handle to be signalled
	 * (which might never happen).  This is possible, because the
	 * thread calling Join() still has a reference to the first
	 * thread's object.
	 */
	return ret;
}