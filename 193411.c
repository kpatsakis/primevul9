gboolean ves_icall_System_Threading_Thread_Join_internal(MonoInternalThread *this,
							 int ms, HANDLE thread)
{
	MonoInternalThread *cur_thread = mono_thread_internal_current ();
	gboolean ret;

	mono_thread_current_check_pending_interrupt ();

	ensure_synch_cs_set (this);
	
	EnterCriticalSection (this->synch_cs);
	
	if ((this->state & ThreadState_Unstarted) != 0) {
		LeaveCriticalSection (this->synch_cs);
		
		mono_raise_exception (mono_get_exception_thread_state ("Thread has not been started."));
		return FALSE;
	}

	LeaveCriticalSection (this->synch_cs);

	if(ms== -1) {
		ms=INFINITE;
	}
	THREAD_DEBUG (g_message ("%s: joining thread handle %p, %d ms", __func__, thread, ms));
	
	mono_thread_set_state (cur_thread, ThreadState_WaitSleepJoin);

	ret=WaitForSingleObjectEx (thread, ms, TRUE);

	mono_thread_clr_state (cur_thread, ThreadState_WaitSleepJoin);
	
	if(ret==WAIT_OBJECT_0) {
		THREAD_DEBUG (g_message ("%s: join successful", __func__));

		return(TRUE);
	}
	
	THREAD_DEBUG (g_message ("%s: join failed", __func__));

	return(FALSE);
}