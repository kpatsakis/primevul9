MonoInternalThread* mono_thread_create_internal (MonoDomain *domain, gpointer func, gpointer arg, gboolean threadpool_thread, guint32 stack_size)
{
	MonoThread *thread;
	MonoInternalThread *internal;
	HANDLE thread_handle;
	struct StartInfo *start_info;
	gsize tid;

	thread = create_thread_object (domain);
	internal = create_internal_thread_object ();
	MONO_OBJECT_SETREF (thread, internal_thread, internal);

	start_info=g_new0 (struct StartInfo, 1);
	start_info->func = func;
	start_info->obj = thread;
	start_info->start_arg = arg;

	mono_threads_lock ();
	if (shutting_down) {
		mono_threads_unlock ();
		g_free (start_info);
		return NULL;
	}
	if (threads_starting_up == NULL) {
		MONO_GC_REGISTER_ROOT_FIXED (threads_starting_up);
		threads_starting_up = mono_g_hash_table_new_type (NULL, NULL, MONO_HASH_KEY_VALUE_GC);
	}

	register_thread_start_argument (thread, start_info);
 	mono_g_hash_table_insert (threads_starting_up, thread, thread);
	mono_threads_unlock ();	

	if (stack_size == 0)
		stack_size = default_stacksize_for_thread (internal);

	/* Create suspended, so we can do some housekeeping before the thread
	 * starts
	 */
	thread_handle = mono_create_thread (NULL, stack_size, (LPTHREAD_START_ROUTINE)start_wrapper, start_info,
				     CREATE_SUSPENDED, &tid);
	THREAD_DEBUG (g_message ("%s: Started thread ID %"G_GSIZE_FORMAT" (handle %p)", __func__, tid, thread_handle));
	if (thread_handle == NULL) {
		/* The thread couldn't be created, so throw an exception */
		mono_threads_lock ();
		mono_g_hash_table_remove (threads_starting_up, thread);
		mono_threads_unlock ();
		g_free (start_info);
		mono_raise_exception (mono_get_exception_execution_engine ("Couldn't create thread"));
		return NULL;
	}

	internal->handle=thread_handle;
	internal->tid=tid;
	internal->apartment_state=ThreadApartmentState_Unknown;
	internal->small_id = mono_thread_small_id_alloc ();
	internal->thread_pinning_ref = internal;
	MONO_GC_REGISTER_ROOT (internal->thread_pinning_ref);

	internal->synch_cs = g_new0 (CRITICAL_SECTION, 1);
	InitializeCriticalSection (internal->synch_cs);

	internal->threadpool_thread = threadpool_thread;
	if (threadpool_thread)
		mono_thread_set_state (internal, ThreadState_Background);

	if (handle_store (thread))
		ResumeThread (thread_handle);

	return internal;
}