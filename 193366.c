thread_adjust_static_data (MonoInternalThread *thread)
{
	guint32 offset;

	mono_threads_lock ();
	if (thread_static_info.offset || thread_static_info.idx > 0) {
		/* get the current allocated size */
		offset = thread_static_info.offset | ((thread_static_info.idx + 1) << 24);
		mono_alloc_static_data (&(thread->static_data), offset, TRUE);
	}
	mono_threads_unlock ();
}