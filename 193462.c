gboolean ves_icall_System_Threading_WaitHandle_WaitOne_internal(MonoObject *this, HANDLE handle, gint32 ms, gboolean exitContext)
{
	guint32 ret;
	MonoInternalThread *thread = mono_thread_internal_current ();

	THREAD_WAIT_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") waiting for %p, %d ms", __func__, GetCurrentThreadId (), handle, ms));
	
	if(ms== -1) {
		ms=INFINITE;
	}
	
	mono_thread_current_check_pending_interrupt ();

	mono_thread_set_state (thread, ThreadState_WaitSleepJoin);
	
	ret=WaitForSingleObjectEx (handle, ms, TRUE);
	
	mono_thread_clr_state (thread, ThreadState_WaitSleepJoin);
	
	if(ret==WAIT_FAILED) {
		THREAD_WAIT_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Wait failed", __func__, GetCurrentThreadId ()));
		return(FALSE);
	} else if(ret==WAIT_TIMEOUT || ret == WAIT_IO_COMPLETION) {
		/* Do we want to try again if we get
		 * WAIT_IO_COMPLETION? The documentation for
		 * WaitHandle doesn't give any clues.  (We'd have to
		 * fiddle with the timeout if we retry.)
		 */
		THREAD_WAIT_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Wait timed out", __func__, GetCurrentThreadId ()));
		return(FALSE);
	}
	
	return(TRUE);
}