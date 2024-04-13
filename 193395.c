void mono_thread_suspend_all_other_threads (void)
{
	struct wait_data wait_data;
	struct wait_data *wait = &wait_data;
	int i;
	gsize self = GetCurrentThreadId ();
	gpointer *events;
	guint32 eventidx = 0;
	gboolean starting, finished;

	memset (wait, 0, sizeof (struct wait_data));
	/*
	 * The other threads could be in an arbitrary state at this point, i.e.
	 * they could be starting up, shutting down etc. This means that there could be
	 * threads which are not even in the threads hash table yet.
	 */

	/* 
	 * First we set a barrier which will be checked by all threads before they
	 * are added to the threads hash table, and they will exit if the flag is set.
	 * This ensures that no threads could be added to the hash later.
	 * We will use shutting_down as the barrier for now.
	 */
	g_assert (shutting_down);

	/*
	 * We make multiple calls to WaitForMultipleObjects since:
	 * - we can only wait for MAXIMUM_WAIT_OBJECTS threads
	 * - some threads could exit without becoming suspended
	 */
	finished = FALSE;
	while (!finished) {
		/*
		 * Make a copy of the hashtable since we can't do anything with
		 * threads while threads_mutex is held.
		 */
		wait->num = 0;
		/*We must zero all InternalThread pointers to avoid making the GC unhappy.*/
		memset (wait->threads, 0, MAXIMUM_WAIT_OBJECTS * SIZEOF_VOID_P);
		mono_threads_lock ();
		mono_g_hash_table_foreach (threads, collect_threads_for_suspend, wait);
		mono_threads_unlock ();

		events = g_new0 (gpointer, wait->num);
		eventidx = 0;
		/* Get the suspended events that we'll be waiting for */
		for (i = 0; i < wait->num; ++i) {
			MonoInternalThread *thread = wait->threads [i];
			gboolean signal_suspend = FALSE;

			if ((thread->tid == self) || mono_gc_is_finalizer_internal_thread (thread) || (thread->flags & MONO_THREAD_FLAG_DONT_MANAGE)) {
				//CloseHandle (wait->handles [i]);
				wait->threads [i] = NULL; /* ignore this thread in next loop */
				continue;
			}

			ensure_synch_cs_set (thread);
		
			EnterCriticalSection (thread->synch_cs);

			if (thread->suspended_event == NULL) {
				thread->suspended_event = CreateEvent (NULL, TRUE, FALSE, NULL);
				if (thread->suspended_event == NULL) {
					/* Forget this one and go on to the next */
					LeaveCriticalSection (thread->synch_cs);
					continue;
				}
			}

			if ((thread->state & ThreadState_Suspended) != 0 || 
				(thread->state & ThreadState_StopRequested) != 0 ||
				(thread->state & ThreadState_Stopped) != 0) {
				LeaveCriticalSection (thread->synch_cs);
				CloseHandle (wait->handles [i]);
				wait->threads [i] = NULL; /* ignore this thread in next loop */
				continue;
			}

			if ((thread->state & ThreadState_SuspendRequested) == 0)
				signal_suspend = TRUE;

			events [eventidx++] = thread->suspended_event;

			/* Convert abort requests into suspend requests */
			if ((thread->state & ThreadState_AbortRequested) != 0)
				thread->state &= ~ThreadState_AbortRequested;
			
			thread->state |= ThreadState_SuspendRequested;

			LeaveCriticalSection (thread->synch_cs);

			/* Signal the thread to suspend */
			if (signal_suspend)
				signal_thread_state_change (thread);
		}

		if (eventidx > 0) {
			WaitForMultipleObjectsEx (eventidx, events, TRUE, 100, FALSE);
			for (i = 0; i < wait->num; ++i) {
				MonoInternalThread *thread = wait->threads [i];

				if (thread == NULL)
					continue;

				ensure_synch_cs_set (thread);
			
				EnterCriticalSection (thread->synch_cs);
				if ((thread->state & ThreadState_Suspended) != 0) {
					CloseHandle (thread->suspended_event);
					thread->suspended_event = NULL;
				}
				LeaveCriticalSection (thread->synch_cs);
			}
		} else {
			/* 
			 * If there are threads which are starting up, we wait until they
			 * are suspended when they try to register in the threads hash.
			 * This is guaranteed to finish, since the threads which can create new
			 * threads get suspended after a while.
			 * FIXME: The finalizer thread can still create new threads.
			 */
			mono_threads_lock ();
			if (threads_starting_up)
				starting = mono_g_hash_table_size (threads_starting_up) > 0;
			else
				starting = FALSE;
			mono_threads_unlock ();
			if (starting)
				Sleep (100);
			else
				finished = TRUE;
		}

		g_free (events);
	}
}