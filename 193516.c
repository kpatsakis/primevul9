static void signal_thread_state_change (MonoInternalThread *thread)
{
	if (thread == mono_thread_internal_current ()) {
		/* Do it synchronously */
		MonoException *exc = mono_thread_request_interruption (FALSE); 
		if (exc)
			mono_raise_exception (exc);
	}

#ifdef HOST_WIN32
	QueueUserAPC ((PAPCFUNC)interruption_request_apc, thread->handle, NULL);
#else
	/* fixme: store the state somewhere */
	mono_thread_kill (thread, mono_thread_get_abort_signal ());

	/* 
	 * This will cause waits to be broken.
	 * It will also prevent the thread from entering a wait, so if the thread returns
	 * from the wait before it receives the abort signal, it will just spin in the wait
	 * functions in the io-layer until the signal handler calls QueueUserAPC which will
	 * make it return.
	 */
	wapi_interrupt_thread (thread->handle);
#endif /* HOST_WIN32 */
}