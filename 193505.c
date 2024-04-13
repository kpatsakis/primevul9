collect_threads (gpointer key, gpointer value, gpointer user_data)
{
	MonoInternalThread *thread = (MonoInternalThread*)value;
	struct wait_data *wait = (struct wait_data*)user_data;
	HANDLE handle;

	if (wait->num<MAXIMUM_WAIT_OBJECTS) {
		handle = OpenThread (THREAD_ALL_ACCESS, TRUE, thread->tid);
		if (handle == NULL)
			return;

		wait->handles [wait->num] = handle;
		wait->threads [wait->num] = thread;
		wait->num++;
	}
}