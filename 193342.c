void ves_icall_System_Threading_Thread_Sleep_internal(gint32 ms)
{
	guint32 res;
	MonoInternalThread *thread = mono_thread_internal_current ();

	THREAD_DEBUG (g_message ("%s: Sleeping for %d ms", __func__, ms));

	mono_thread_current_check_pending_interrupt ();
	
	while (TRUE) {
		mono_thread_set_state (thread, ThreadState_WaitSleepJoin);
	
		res = SleepEx(ms,TRUE);
	
		mono_thread_clr_state (thread, ThreadState_WaitSleepJoin);

		if (res == WAIT_IO_COMPLETION) { /* we might have been interrupted */
			MonoException* exc = mono_thread_execute_interruption (thread);
			if (exc) {
				mono_raise_exception (exc);
			} else {
				// FIXME: !INFINITE
				if (ms != INFINITE)
					break;
			}
		} else {
			break;
		}
	}
}