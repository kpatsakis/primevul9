mono_thread_detach (MonoThread *thread)
{
	g_return_if_fail (thread != NULL);

	THREAD_DEBUG (g_message ("%s: mono_thread_detach for %p (%"G_GSIZE_FORMAT")", __func__, thread, (gsize)thread->internal_thread->tid));
	
	thread_cleanup (thread->internal_thread);

	SET_CURRENT_OBJECT (NULL);
	mono_domain_unset ();

	/* Don't need to CloseHandle this thread, even though we took a
	 * reference in mono_thread_attach (), because the GC will do it
	 * when the Thread object is finalised.
	 */
}