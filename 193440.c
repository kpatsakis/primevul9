mono_thread_resume (MonoInternalThread *thread)
{
	ensure_synch_cs_set (thread);
	
	EnterCriticalSection (thread->synch_cs);

	if ((thread->state & ThreadState_SuspendRequested) != 0) {
		thread->state &= ~ThreadState_SuspendRequested;
		LeaveCriticalSection (thread->synch_cs);
		return TRUE;
	}

	if ((thread->state & ThreadState_Suspended) == 0 ||
		(thread->state & ThreadState_Unstarted) != 0 || 
		(thread->state & ThreadState_Aborted) != 0 || 
		(thread->state & ThreadState_Stopped) != 0)
	{
		LeaveCriticalSection (thread->synch_cs);
		return FALSE;
	}
	
	thread->resume_event = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (thread->resume_event == NULL) {
		LeaveCriticalSection (thread->synch_cs);
		return(FALSE);
	}
	
	/* Awake the thread */
	SetEvent (thread->suspend_event);

	LeaveCriticalSection (thread->synch_cs);

	/* Wait for the thread to awake */
	WaitForSingleObject (thread->resume_event, INFINITE);
	CloseHandle (thread->resume_event);
	thread->resume_event = NULL;

	return TRUE;
}