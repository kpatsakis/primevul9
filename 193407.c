remove_and_abort_threads (gpointer key, gpointer value, gpointer user)
{
	struct wait_data *wait=(struct wait_data *)user;
	gsize self = GetCurrentThreadId ();
	MonoInternalThread *thread = value;
	HANDLE handle;

	if (wait->num >= MAXIMUM_WAIT_OBJECTS)
		return FALSE;

	/* The finalizer thread is not a background thread */
	if (thread->tid != self && (thread->state & ThreadState_Background) != 0 &&
		!(thread->flags & MONO_THREAD_FLAG_DONT_MANAGE)) {
	
		handle = OpenThread (THREAD_ALL_ACCESS, TRUE, thread->tid);
		if (handle == NULL)
			return FALSE;

		/* printf ("A: %d\n", wait->num); */
		wait->handles[wait->num]=thread->handle;
		wait->threads[wait->num]=thread;
		wait->num++;

		THREAD_DEBUG (g_print ("%s: Aborting id: %"G_GSIZE_FORMAT"\n", __func__, (gsize)thread->tid));
		mono_thread_internal_stop (thread);
		return TRUE;
	}

	return (thread->tid != self && !mono_gc_is_finalizer_internal_thread (thread)); 
}