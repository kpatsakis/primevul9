mono_threads_install_cleanup (MonoThreadCleanupFunc func)
{
	mono_thread_cleanup_fn = func;
}