ves_icall_System_Environment_Exit (int result)
{
	MONO_ARCH_SAVE_REGS;

	mono_threads_set_shutting_down ();

	mono_runtime_set_shutting_down ();

	/* This will kill the tp threads which cannot be suspended */
	mono_thread_pool_cleanup ();

	/* Suspend all managed threads since the runtime is going away */
	mono_thread_suspend_all_other_threads ();

	mono_runtime_quit ();

	/* we may need to do some cleanup here... */
	exit (result);
}