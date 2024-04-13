void mono_thread_internal_stop (MonoInternalThread *thread)
{
	ensure_synch_cs_set (thread);
	
	EnterCriticalSection (thread->synch_cs);

	if ((thread->state & ThreadState_StopRequested) != 0 ||
		(thread->state & ThreadState_Stopped) != 0)
	{
		LeaveCriticalSection (thread->synch_cs);
		return;
	}
	
	/* Make sure the thread is awake */
	mono_thread_resume (thread);

	thread->state |= ThreadState_StopRequested;
	thread->state &= ~ThreadState_AbortRequested;
	
	LeaveCriticalSection (thread->synch_cs);
	
	signal_thread_state_change (thread);
}