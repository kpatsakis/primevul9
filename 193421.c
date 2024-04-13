void mono_thread_abort_all_other_threads (void)
{
	gsize self = GetCurrentThreadId ();

	mono_threads_lock ();
	THREAD_DEBUG (g_message ("%s: There are %d threads to abort", __func__,
				 mono_g_hash_table_size (threads));
		      mono_g_hash_table_foreach (threads, print_tids, NULL));

	mono_g_hash_table_foreach (threads, terminate_thread, (gpointer)self);
	
	mono_threads_unlock ();
}