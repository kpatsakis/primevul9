static guint32 WINAPI start_wrapper_internal(void *data)
{
	struct StartInfo *start_info=(struct StartInfo *)data;
	guint32 (*start_func)(void *);
	void *start_arg;
	gsize tid;
	/* 
	 * We don't create a local to hold start_info->obj, so hopefully it won't get pinned during a
	 * GC stack walk.
	 */
	MonoInternalThread *internal = start_info->obj->internal_thread;
	MonoObject *start_delegate = start_info->delegate;
	MonoDomain *domain = start_info->obj->obj.vtable->domain;

	THREAD_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Start wrapper", __func__, GetCurrentThreadId ()));

	/* We can be sure start_info->obj->tid and
	 * start_info->obj->handle have been set, because the thread
	 * was created suspended, and these values were set before the
	 * thread resumed
	 */

	tid=internal->tid;

	SET_CURRENT_OBJECT (internal);

	mono_monitor_init_tls ();

	/* Every thread references the appdomain which created it */
	mono_thread_push_appdomain_ref (domain);
	
	if (!mono_domain_set (domain, FALSE)) {
		/* No point in raising an appdomain_unloaded exception here */
		/* FIXME: Cleanup here */
		mono_thread_pop_appdomain_ref ();
		return 0;
	}

	start_func = start_info->func;
	start_arg = start_info->start_arg;

	/* We have to do this here because mono_thread_new_init()
	   requires that root_domain_thread is set up. */
	thread_adjust_static_data (internal);
	init_root_domain_thread (internal, start_info->obj);

	/* This MUST be called before any managed code can be
	 * executed, as it calls the callback function that (for the
	 * jit) sets the lmf marker.
	 */
	mono_thread_new_init (tid, &tid, start_func);
	internal->stack_ptr = &tid;

	LIBGC_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT",%d) Setting thread stack to %p", __func__, GetCurrentThreadId (), getpid (), thread->stack_ptr));

	THREAD_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Setting current_object_key to %p", __func__, GetCurrentThreadId (), internal));

	/* On 2.0 profile (and higher), set explicitly since state might have been
	   Unknown */
	if (internal->apartment_state == ThreadApartmentState_Unknown)
		internal->apartment_state = ThreadApartmentState_MTA;

	mono_thread_init_apartment_state ();

	if(internal->start_notify!=NULL) {
		/* Let the thread that called Start() know we're
		 * ready
		 */
		ReleaseSemaphore (internal->start_notify, 1, NULL);
	}

	mono_threads_lock ();
	mono_g_hash_table_remove (thread_start_args, start_info->obj);
	mono_threads_unlock ();

	mono_thread_set_execution_context (start_info->obj->ec_to_set);
	start_info->obj->ec_to_set = NULL;

	g_free (start_info);
	THREAD_DEBUG (g_message ("%s: start_wrapper for %"G_GSIZE_FORMAT, __func__,
							 internal->tid));

	/* 
	 * Call this after calling start_notify, since the profiler callback might want
	 * to lock the thread, and the lock is held by thread_start () which waits for
	 * start_notify.
	 */
	mono_profiler_thread_start (tid);

	/* start_func is set only for unmanaged start functions */
	if (start_func) {
		start_func (start_arg);
	} else {
		void *args [1];
		g_assert (start_delegate != NULL);
		args [0] = start_arg;
		/* we may want to handle the exception here. See comment below on unhandled exceptions */
		mono_runtime_delegate_invoke (start_delegate, args, NULL);
	}

	/* If the thread calls ExitThread at all, this remaining code
	 * will not be executed, but the main thread will eventually
	 * call thread_cleanup() on this thread's behalf.
	 */

	THREAD_DEBUG (g_message ("%s: (%"G_GSIZE_FORMAT") Start wrapper terminating", __func__, GetCurrentThreadId ()));

	thread_cleanup (internal);

	/* Do any cleanup needed for apartment state. This
	 * cannot be done in thread_cleanup since thread_cleanup could be 
	 * called for a thread other than the current thread.
	 * mono_thread_cleanup_apartment_state cleans up apartment
	 * for the current thead */
	mono_thread_cleanup_apartment_state ();

	/* Remove the reference to the thread object in the TLS data,
	 * so the thread object can be finalized.  This won't be
	 * reached if the thread threw an uncaught exception, so those
	 * thread handles will stay referenced :-( (This is due to
	 * missing support for scanning thread-specific data in the
	 * Boehm GC - the io-layer keeps a GC-visible hash of pointers
	 * to TLS data.)
	 */
	SET_CURRENT_OBJECT (NULL);
	mono_domain_unset ();

	return(0);
}