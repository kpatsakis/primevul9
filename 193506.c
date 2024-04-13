mono_thread_request_interruption (gboolean running_managed)
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	/* The thread may already be stopping */
	if (thread == NULL) 
		return NULL;

#ifdef HOST_WIN32
	if (thread->interrupt_on_stop && 
		thread->state & ThreadState_StopRequested && 
		thread->state & ThreadState_Background)
		ExitThread (1);
#endif
	
	if (InterlockedCompareExchange (&thread->interruption_requested, 1, 0) == 1)
		return NULL;

	if (!running_managed || is_running_protected_wrapper ()) {
		/* Can't stop while in unmanaged code. Increase the global interruption
		   request count. When exiting the unmanaged method the count will be
		   checked and the thread will be interrupted. */
		
		InterlockedIncrement (&thread_interruption_requested);

		if (mono_thread_notify_pending_exc_fn && !running_managed)
			/* The JIT will notify the thread about the interruption */
			/* This shouldn't take any locks */
			mono_thread_notify_pending_exc_fn ();

		/* this will awake the thread if it is in WaitForSingleObject 
		   or similar */
		/* Our implementation of this function ignores the func argument */
		QueueUserAPC ((PAPCFUNC)dummy_apc, thread->handle, NULL);
		return NULL;
	}
	else {
		return mono_thread_execute_interruption (thread);
	}
}