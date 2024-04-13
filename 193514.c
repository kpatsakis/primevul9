ves_icall_System_Threading_WaitHandle_SignalAndWait_Internal (HANDLE toSignal, HANDLE toWait, gint32 ms, gboolean exitContext)
{
	guint32 ret;
	MonoInternalThread *thread = mono_thread_internal_current ();

	MONO_ARCH_SAVE_REGS;

	if (ms == -1)
		ms = INFINITE;

	mono_thread_current_check_pending_interrupt ();

	mono_thread_set_state (thread, ThreadState_WaitSleepJoin);
	
	ret = SignalObjectAndWait (toSignal, toWait, ms, TRUE);
	
	mono_thread_clr_state (thread, ThreadState_WaitSleepJoin);

	return  (!(ret == WAIT_TIMEOUT || ret == WAIT_IO_COMPLETION || ret == WAIT_FAILED));
}