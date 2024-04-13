ves_icall_System_Threading_Thread_ResetAbort (void)
{
	MonoInternalThread *thread = mono_thread_internal_current ();
	gboolean was_aborting;

	ensure_synch_cs_set (thread);
	
	EnterCriticalSection (thread->synch_cs);
	was_aborting = thread->state & ThreadState_AbortRequested;
	thread->state &= ~ThreadState_AbortRequested;
	LeaveCriticalSection (thread->synch_cs);

	if (!was_aborting) {
		const char *msg = "Unable to reset abort because no abort was requested";
		mono_raise_exception (mono_get_exception_thread_state (msg));
	}
	thread->abort_exc = NULL;
	if (thread->abort_state_handle) {
		mono_gchandle_free (thread->abort_state_handle);
		/* This is actually not necessary - the handle
		   only counts if the exception is set */
		thread->abort_state_handle = 0;
	}
}