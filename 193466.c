mono_thread_suspend (MonoInternalThread *thread)
{
	ensure_synch_cs_set (thread);
	
	EnterCriticalSection (thread->synch_cs);

	if ((thread->state & ThreadState_Unstarted) != 0 || 
		(thread->state & ThreadState_Aborted) != 0 || 
		(thread->state & ThreadState_Stopped) != 0)
	{
		LeaveCriticalSection (thread->synch_cs);
		return FALSE;
	}

	if ((thread->state & ThreadState_Suspended) != 0 || 
		(thread->state & ThreadState_SuspendRequested) != 0 ||
		(thread->state & ThreadState_StopRequested) != 0) 
	{
		LeaveCriticalSection (thread->synch_cs);
		return TRUE;
	}
	
	thread->state |= ThreadState_SuspendRequested;

	LeaveCriticalSection (thread->synch_cs);

	signal_thread_state_change (thread);
	return TRUE;
}