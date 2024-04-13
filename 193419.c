static void build_wait_tids (gpointer key, gpointer value, gpointer user)
{
	struct wait_data *wait=(struct wait_data *)user;

	if(wait->num<MAXIMUM_WAIT_OBJECTS) {
		HANDLE handle;
		MonoInternalThread *thread=(MonoInternalThread *)value;

		/* Ignore background threads, we abort them later */
		/* Do not lock here since it is not needed and the caller holds threads_lock */
		if (thread->state & ThreadState_Background) {
			THREAD_DEBUG (g_message ("%s: ignoring background thread %"G_GSIZE_FORMAT, __func__, (gsize)thread->tid));
			return; /* just leave, ignore */
		}
		
		if (mono_gc_is_finalizer_internal_thread (thread)) {
			THREAD_DEBUG (g_message ("%s: ignoring finalizer thread %"G_GSIZE_FORMAT, __func__, (gsize)thread->tid));
			return;
		}

		if (thread == mono_thread_internal_current ()) {
			THREAD_DEBUG (g_message ("%s: ignoring current thread %"G_GSIZE_FORMAT, __func__, (gsize)thread->tid));
			return;
		}

		if (mono_thread_get_main () && (thread == mono_thread_get_main ()->internal_thread)) {
			THREAD_DEBUG (g_message ("%s: ignoring main thread %"G_GSIZE_FORMAT, __func__, (gsize)thread->tid));
			return;
		}

		if (thread->flags & MONO_THREAD_FLAG_DONT_MANAGE) {
			THREAD_DEBUG (g_message ("%s: ignoring thread %" G_GSIZE_FORMAT "with DONT_MANAGE flag set.", __func__, (gsize)thread->tid));
			return;
		}

		handle = OpenThread (THREAD_ALL_ACCESS, TRUE, thread->tid);
		if (handle == NULL) {
			THREAD_DEBUG (g_message ("%s: ignoring unopenable thread %"G_GSIZE_FORMAT, __func__, (gsize)thread->tid));
			return;
		}
		
		THREAD_DEBUG (g_message ("%s: Invoking mono_thread_manage callback on thread %p", __func__, thread));
		if ((thread->manage_callback == NULL) || (thread->manage_callback (thread->root_domain_thread) == TRUE)) {
			wait->handles[wait->num]=handle;
			wait->threads[wait->num]=thread;
			wait->num++;

			THREAD_DEBUG (g_message ("%s: adding thread %"G_GSIZE_FORMAT, __func__, (gsize)thread->tid));
		} else {
			THREAD_DEBUG (g_message ("%s: ignoring (because of callback) thread %"G_GSIZE_FORMAT, __func__, (gsize)thread->tid));
		}
		
		
	} else {
		/* Just ignore the rest, we can't do anything with
		 * them yet
		 */
	}
}