void mono_thread_current_check_pending_interrupt ()
{
	MonoInternalThread *thread = mono_thread_internal_current ();
	gboolean throw = FALSE;

	mono_debugger_check_interruption ();

	ensure_synch_cs_set (thread);
	
	EnterCriticalSection (thread->synch_cs);
	
	if (thread->thread_interrupt_requested) {
		throw = TRUE;
		thread->thread_interrupt_requested = FALSE;
	}
	
	LeaveCriticalSection (thread->synch_cs);

	if (throw) {
		mono_raise_exception (mono_get_exception_thread_interrupted ());
	}
}