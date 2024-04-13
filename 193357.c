static void wait_for_tids_or_state_change (struct wait_data *wait, guint32 timeout)
{
	guint32 i, ret, count;
	
	THREAD_DEBUG (g_message("%s: %d threads to wait for in this batch", __func__, wait->num));

	/* Add the thread state change event, so it wakes up if a thread changes
	 * to background mode.
	 */
	count = wait->num;
	if (count < MAXIMUM_WAIT_OBJECTS) {
		wait->handles [count] = background_change_event;
		count++;
	}

	ret=WaitForMultipleObjectsEx (count, wait->handles, FALSE, timeout, TRUE);

	if(ret==WAIT_FAILED) {
		/* See the comment in build_wait_tids() */
		THREAD_DEBUG (g_message ("%s: Wait failed", __func__));
		return;
	}
	
	for(i=0; i<wait->num; i++)
		CloseHandle (wait->handles[i]);

	if (ret == WAIT_TIMEOUT)
		return;
	
	if (ret < wait->num) {
		gsize tid = wait->threads[ret]->tid;
		mono_threads_lock ();
		if (mono_g_hash_table_lookup (threads, (gpointer)tid)!=NULL) {
			/* See comment in wait_for_tids about thread cleanup */
			mono_threads_unlock ();
			THREAD_DEBUG (g_message ("%s: cleaning up after thread %"G_GSIZE_FORMAT, __func__, tid));
			thread_cleanup (wait->threads [ret]);
		} else
			mono_threads_unlock ();
	}
}