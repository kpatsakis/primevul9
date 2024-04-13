HANDLE ves_icall_System_Threading_Thread_Thread_internal(MonoThread *this,
							 MonoObject *start)
{
	guint32 (*start_func)(void *);
	struct StartInfo *start_info;
	HANDLE thread;
	gsize tid;
	MonoInternalThread *internal;

	THREAD_DEBUG (g_message("%s: Trying to start a new thread: this (%p) start (%p)", __func__, this, start));

	if (!this->internal_thread)
		ves_icall_System_Threading_Thread_ConstructInternalThread (this);
	internal = this->internal_thread;

	ensure_synch_cs_set (internal);

	EnterCriticalSection (internal->synch_cs);

	if ((internal->state & ThreadState_Unstarted) == 0) {
		LeaveCriticalSection (internal->synch_cs);
		mono_raise_exception (mono_get_exception_thread_state ("Thread has already been started."));
		return NULL;
	}

	internal->small_id = -1;

	if ((internal->state & ThreadState_Aborted) != 0) {
		LeaveCriticalSection (internal->synch_cs);
		return this;
	}
	start_func = NULL;
	{
		/* This is freed in start_wrapper */
		start_info = g_new0 (struct StartInfo, 1);
		start_info->func = start_func;
		start_info->start_arg = this->start_obj; /* FIXME: GC object stored in unmanaged memory */
		start_info->delegate = start;
		start_info->obj = this;
		g_assert (this->obj.vtable->domain == mono_domain_get ());

		internal->start_notify=CreateSemaphore (NULL, 0, 0x7fffffff, NULL);
		if (internal->start_notify==NULL) {
			LeaveCriticalSection (internal->synch_cs);
			g_warning ("%s: CreateSemaphore error 0x%x", __func__, GetLastError ());
			g_free (start_info);
			return(NULL);
		}

		mono_threads_lock ();
		register_thread_start_argument (this, start_info);
		if (threads_starting_up == NULL) {
			MONO_GC_REGISTER_ROOT_FIXED (threads_starting_up);
			threads_starting_up = mono_g_hash_table_new_type (NULL, NULL, MONO_HASH_KEY_VALUE_GC);
		}
		mono_g_hash_table_insert (threads_starting_up, this, this);
		mono_threads_unlock ();	

		thread=mono_create_thread(NULL, default_stacksize_for_thread (internal), (LPTHREAD_START_ROUTINE)start_wrapper, start_info,
				    CREATE_SUSPENDED, &tid);
		if(thread==NULL) {
			LeaveCriticalSection (internal->synch_cs);
			mono_threads_lock ();
			mono_g_hash_table_remove (threads_starting_up, this);
			mono_threads_unlock ();
			g_warning("%s: CreateThread error 0x%x", __func__, GetLastError());
			return(NULL);
		}
		
		internal->handle=thread;
		internal->tid=tid;
		internal->small_id = mono_thread_small_id_alloc ();
		internal->thread_pinning_ref = internal;
		MONO_GC_REGISTER_ROOT (internal->thread_pinning_ref);
		

		/* Don't call handle_store() here, delay it to Start.
		 * We can't join a thread (trying to will just block
		 * forever) until it actually starts running, so don't
		 * store the handle till then.
		 */

		mono_thread_start (this);
		
		internal->state &= ~ThreadState_Unstarted;

		THREAD_DEBUG (g_message ("%s: Started thread ID %"G_GSIZE_FORMAT" (handle %p)", __func__, tid, thread));

		LeaveCriticalSection (internal->synch_cs);
		return(thread);
	}
}