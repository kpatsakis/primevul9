static gboolean handle_store(MonoThread *thread)
{
	mono_threads_lock ();

	THREAD_DEBUG (g_message ("%s: thread %p ID %"G_GSIZE_FORMAT, __func__, thread, (gsize)thread->internal_thread->tid));

	if (threads_starting_up)
		mono_g_hash_table_remove (threads_starting_up, thread);

	if (shutting_down) {
		mono_threads_unlock ();
		return FALSE;
	}

	if(threads==NULL) {
		MONO_GC_REGISTER_ROOT_FIXED (threads);
		threads=mono_g_hash_table_new_type (NULL, NULL, MONO_HASH_VALUE_GC);
	}

	/* We don't need to duplicate thread->handle, because it is
	 * only closed when the thread object is finalized by the GC.
	 */
	g_assert (thread->internal_thread);
	mono_g_hash_table_insert(threads, (gpointer)(gsize)(thread->internal_thread->tid),
				 thread->internal_thread);

	mono_threads_unlock ();

	return TRUE;
}