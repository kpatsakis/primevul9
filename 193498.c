mono_thread_test_state (MonoInternalThread *thread, MonoThreadState test)
{
	gboolean ret = FALSE;

	ensure_synch_cs_set (thread);
	
	EnterCriticalSection (thread->synch_cs);

	if ((thread->state & test) != 0) {
		ret = TRUE;
	}
	
	LeaveCriticalSection (thread->synch_cs);
	
	return ret;
}