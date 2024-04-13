mono_thread_attach (MonoDomain *domain)
{
	MonoInternalThread *thread;
	MonoThread *current_thread;
	HANDLE thread_handle;
	gsize tid;

	if ((thread = mono_thread_internal_current ())) {
		if (domain != mono_domain_get ())
			mono_domain_set (domain, TRUE);
		/* Already attached */
		return mono_thread_current ();
	}

	if (!mono_gc_register_thread (&domain)) {
		g_error ("Thread %"G_GSIZE_FORMAT" calling into managed code is not registered with the GC. On UNIX, this can be fixed by #include-ing <gc.h> before <pthread.h> in the file containing the thread creation code.", GetCurrentThreadId ());
	}

	thread = create_internal_thread_object ();

	thread_handle = GetCurrentThread ();
	g_assert (thread_handle);

	tid=GetCurrentThreadId ();

	/* 
	 * The handle returned by GetCurrentThread () is a pseudo handle, so it can't be used to
	 * refer to the thread from other threads for things like aborting.
	 */
	DuplicateHandle (GetCurrentProcess (), thread_handle, GetCurrentProcess (), &thread_handle, 
					 THREAD_ALL_ACCESS, TRUE, 0);

	thread->handle=thread_handle;
	thread->tid=tid;
#ifdef PLATFORM_ANDROID
	thread->android_tid = (gpointer) gettid ();
#endif
	thread->apartment_state=ThreadApartmentState_Unknown;
	thread->small_id = mono_thread_small_id_alloc ();
	thread->thread_pinning_ref = thread;
	MONO_GC_REGISTER_ROOT (thread->thread_pinning_ref);

	thread->stack_ptr = &tid;

	thread->synch_cs = g_new0 (CRITICAL_SECTION, 1);
	InitializeCriticalSection (thread->synch_cs);

	THREAD_DEBUG (g_message ("%s: Attached thread ID %"G_GSIZE_FORMAT" (handle %p)", __func__, tid, thread_handle));

	current_thread = new_thread_with_internal (domain, thread);

	if (!handle_store (current_thread)) {
		/* Mono is shutting down, so just wait for the end */
		for (;;)
			Sleep (10000);
	}

	THREAD_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Setting current_object_key to %p", __func__, GetCurrentThreadId (), thread));

	SET_CURRENT_OBJECT (thread);
	mono_domain_set (domain, TRUE);

	mono_monitor_init_tls ();

	thread_adjust_static_data (thread);

	init_root_domain_thread (thread, current_thread);
	if (domain != mono_get_root_domain ())
		set_current_thread_for_domain (domain, thread, current_thread);


	if (mono_thread_attach_cb) {
		guint8 *staddr;
		size_t stsize;

		mono_thread_get_stack_bounds (&staddr, &stsize);

		if (staddr == NULL)
			mono_thread_attach_cb (tid, &tid);
		else
			mono_thread_attach_cb (tid, staddr + stsize);
	}

	// FIXME: Need a separate callback
	mono_profiler_thread_start (tid);

	return current_thread;
}