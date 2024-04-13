void ves_icall_System_Threading_Thread_Interrupt_internal (MonoInternalThread *this)
{
	gboolean throw = FALSE;
	
	ensure_synch_cs_set (this);

	if (this == mono_thread_internal_current ())
		return;
	
	EnterCriticalSection (this->synch_cs);
	
	this->thread_interrupt_requested = TRUE;
	
	if (this->state & ThreadState_WaitSleepJoin) {
		throw = TRUE;
	}
	
	LeaveCriticalSection (this->synch_cs);
	
	if (throw) {
		signal_thread_state_change (this);
	}
}