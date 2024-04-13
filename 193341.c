void mono_thread_init (MonoThreadStartCB start_cb,
		       MonoThreadAttachCB attach_cb)
{
	mono_thread_smr_init ();

	InitializeCriticalSection(&threads_mutex);
	InitializeCriticalSection(&interlocked_mutex);
	InitializeCriticalSection(&contexts_mutex);
	
	background_change_event = CreateEvent (NULL, TRUE, FALSE, NULL);
	g_assert(background_change_event != NULL);
	
	mono_init_static_data_info (&thread_static_info);
	mono_init_static_data_info (&context_static_info);

	MONO_FAST_TLS_INIT (tls_current_object);
	current_object_key=TlsAlloc();
	THREAD_DEBUG (g_message ("%s: Allocated current_object_key %d", __func__, current_object_key));

	mono_thread_start_cb = start_cb;
	mono_thread_attach_cb = attach_cb;

	/* Get a pseudo handle to the current process.  This is just a
	 * kludge so that wapi can build a process handle if needed.
	 * As a pseudo handle is returned, we don't need to clean
	 * anything up.
	 */
	GetCurrentProcess ();
}