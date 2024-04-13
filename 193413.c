mono_thread_set_state (MonoInternalThread *thread, MonoThreadState state)
{
	ensure_synch_cs_set (thread);
	
	EnterCriticalSection (thread->synch_cs);
	thread->state |= state;
	LeaveCriticalSection (thread->synch_cs);
}