mono_thread_resume_interruption (void)
{
	MonoInternalThread *thread = mono_thread_internal_current ();
	gboolean still_aborting;

	/* The thread may already be stopping */
	if (thread == NULL)
		return NULL;

	ensure_synch_cs_set (thread);
	EnterCriticalSection (thread->synch_cs);
	still_aborting = (thread->state & ThreadState_AbortRequested) != 0;
	LeaveCriticalSection (thread->synch_cs);

	/*This can happen if the protected block called Thread::ResetAbort*/
	if (!still_aborting)
		return FALSE;

	if (InterlockedCompareExchange (&thread->interruption_requested, 1, 0) == 1)
		return NULL;
	InterlockedIncrement (&thread_interruption_requested);

#ifndef HOST_WIN32
	wapi_self_interrupt ();
#endif
	return mono_thread_execute_interruption (thread);
}