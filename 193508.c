mono_set_pending_exception (MonoException *exc)
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	/* The thread may already be stopping */
	if (thread == NULL)
		return;

	if (mono_thread_notify_pending_exc_fn) {
		MONO_OBJECT_SETREF (thread, pending_exception, exc);

		mono_thread_notify_pending_exc_fn ();
	} else {
		/* No way to notify the JIT about the exception, have to throw it now */
		mono_raise_exception (exc);
	}
}