mono_thread_kill (MonoInternalThread *thread, int signal)
{
#ifdef HOST_WIN32
	/* Win32 uses QueueUserAPC and callers of this are guarded */
	g_assert_not_reached ();
#else
#  ifdef PTHREAD_POINTER_ID
	return pthread_kill ((gpointer)(gsize)(thread->tid), mono_thread_get_abort_signal ());
#  else
#    ifdef PLATFORM_ANDROID
	if (thread->android_tid != 0) {
		int  ret;
		int  old_errno = errno;

		ret = tkill ((pid_t) thread->android_tid, signal);
		if (ret < 0) {
			ret = errno;
			errno = old_errno;
		}

		return ret;
	}
	else
		return pthread_kill (thread->tid, mono_thread_get_abort_signal ());
#    else
	return pthread_kill (thread->tid, mono_thread_get_abort_signal ());
#    endif
#  endif
#endif
}