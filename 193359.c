mono_thread_internal_reset_abort (MonoInternalThread *thread)
{
	ensure_synch_cs_set (thread);

	EnterCriticalSection (thread->synch_cs);

	thread->state &= ~ThreadState_AbortRequested;

	if (thread->abort_exc) {
		thread->abort_exc = NULL;
		if (thread->abort_state_handle) {
			mono_gchandle_free (thread->abort_state_handle);
			/* This is actually not necessary - the handle
			   only counts if the exception is set */
			thread->abort_state_handle = 0;
		}
	}

	LeaveCriticalSection (thread->synch_cs);
}