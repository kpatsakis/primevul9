static void mono_thread_interruption_checkpoint_request (gboolean bypass_abort_protection)
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	/* The thread may already be stopping */
	if (thread == NULL)
		return;

	mono_debugger_check_interruption ();

	if (thread->interruption_requested && (bypass_abort_protection || !is_running_protected_wrapper ())) {
		MonoException* exc = mono_thread_execute_interruption (thread);
		if (exc) mono_raise_exception (exc);
	}
}