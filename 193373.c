mono_thread_get_and_clear_pending_exception (void)
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	/* The thread may already be stopping */
	if (thread == NULL)
		return NULL;

	if (thread->interruption_requested && !is_running_protected_wrapper ()) {
		return mono_thread_execute_interruption (thread);
	}
	
	if (thread->pending_exception) {
		MonoException *exc = thread->pending_exception;

		thread->pending_exception = NULL;
		return exc;
	}

	return NULL;
}