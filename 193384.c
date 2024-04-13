void mono_thread_cleanup (void)
{
#if !defined(HOST_WIN32) && !defined(RUN_IN_SUBTHREAD)
	/* The main thread must abandon any held mutexes (particularly
	 * important for named mutexes as they are shared across
	 * processes, see bug 74680.)  This will happen when the
	 * thread exits, but if it's not running in a subthread it
	 * won't exit in time.
	 */
	/* Using non-w32 API is a nasty kludge, but I couldn't find
	 * anything in the documentation that would let me do this
	 * here yet still be safe to call on windows.
	 */
	_wapi_thread_signal_self (mono_environment_exitcode_get ());
#endif

#if 0
	/* This stuff needs more testing, it seems one of these
	 * critical sections can be locked when mono_thread_cleanup is
	 * called.
	 */
	DeleteCriticalSection (&threads_mutex);
	DeleteCriticalSection (&interlocked_mutex);
	DeleteCriticalSection (&contexts_mutex);
	DeleteCriticalSection (&delayed_free_table_mutex);
	DeleteCriticalSection (&small_id_mutex);
	CloseHandle (background_change_event);
#endif

	TlsFree (current_object_key);
}