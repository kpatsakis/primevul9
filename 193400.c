gboolean ves_icall_System_Threading_WaitHandle_WaitAll_internal(MonoArray *mono_handles, gint32 ms, gboolean exitContext)
{
	HANDLE *handles;
	guint32 numhandles;
	guint32 ret;
	guint32 i;
	MonoObject *waitHandle;
	MonoInternalThread *thread = mono_thread_internal_current ();

	/* Do this WaitSleepJoin check before creating objects */
	mono_thread_current_check_pending_interrupt ();

	numhandles = mono_array_length(mono_handles);
	handles = g_new0(HANDLE, numhandles);

	for(i = 0; i < numhandles; i++) {	
		waitHandle = mono_array_get(mono_handles, MonoObject*, i);
		handles [i] = mono_wait_handle_get_handle ((MonoWaitHandle *) waitHandle);
	}
	
	if(ms== -1) {
		ms=INFINITE;
	}

	mono_thread_set_state (thread, ThreadState_WaitSleepJoin);
	
	ret=WaitForMultipleObjectsEx(numhandles, handles, TRUE, ms, TRUE);

	mono_thread_clr_state (thread, ThreadState_WaitSleepJoin);

	g_free(handles);

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