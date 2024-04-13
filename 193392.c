mono_thread_exit ()
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	THREAD_DEBUG (g_message ("%s: mono_thread_exit for %p (%"G_GSIZE_FORMAT")", __func__, thread, (gsize)thread->tid));

	thread_cleanup (thread);
	SET_CURRENT_OBJECT (NULL);
	mono_domain_unset ();

	/* we could add a callback here for embedders to use. */
	if (mono_thread_get_main () && (thread == mono_thread_get_main ()->internal_thread))
		exit (mono_environment_exitcode_get ());
	ExitThread (-1);
}