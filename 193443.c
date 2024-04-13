ves_icall_System_Threading_Thread_GetState (MonoInternalThread* this)
{
	guint32 state;

	ensure_synch_cs_set (this);
	
	EnterCriticalSection (this->synch_cs);
	
	state = this->state;

	LeaveCriticalSection (this->synch_cs);
	
	return state;
}