gint32 ves_icall_System_Threading_WaitHandle_WaitAny_internal(MonoArray *mono_handles, gint32 ms, gboolean exitContext)
{
	HANDLE handles [MAXIMUM_WAIT_OBJECTS];
	guint32 numhandles;
	guint32 ret;
	guint32 i;
	MonoObject *waitHandle;
	MonoInternalThread *thread = mono_thread_internal_current ();
	guint32 start;

	/* Do this WaitSleepJoin check before creating objects */
	mono_thread_current_check_pending_interrupt ();

	numhandles = mono_array_length(mono_handles);
	if (numhandles > MAXIMUM_WAIT_OBJECTS)
		return WAIT_FAILED;

	for(i = 0; i < numhandles; i++) {	
		waitHandle = mono_array_get(mono_handles, MonoObject*, i);
		handles [i] = mono_wait_handle_get_handle ((MonoWaitHandle *) waitHandle);
	}
	
	if(ms== -1) {
		ms=INFINITE;
	}

	mono_thread_set_state (thread, ThreadState_WaitSleepJoin);

	start = (ms == -1) ? 0 : mono_msec_ticks ();
	do {
		ret = WaitForMultipleObjectsEx (numhandles, handles, FALSE, ms, TRUE);
		if (ret != WAIT_IO_COMPLETION)
			break;
		if (ms != -1) {
			guint32 diff;

			diff = mono_msec_ticks () - start;
			ms -= diff;
			if (ms <= 0)
				break;
		}
	} while (ms == -1 || ms > 0);

	mono_thread_clr_state (thread, ThreadState_WaitSleepJoin);

	THREAD_WAIT_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") returning %d", __func__, GetCurrentThreadId (), ret));

	/*
	 * These need to be here.  See MSDN dos on WaitForMultipleObjects.
	 */
	if (ret >= WAIT_OBJECT_0 && ret <= WAIT_OBJECT_0 + numhandles - 1) {
		return ret - WAIT_OBJECT_0;
	}
	else if (ret >= WAIT_ABANDONED_0 && ret <= WAIT_ABANDONED_0 + numhandles - 1) {
		return ret - WAIT_ABANDONED_0;
	}
	else {
		return ret;
	}
}