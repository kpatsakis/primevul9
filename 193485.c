register_thread_start_argument (MonoThread *thread, struct StartInfo *start_info)
{
	if (thread_start_args == NULL) {
		MONO_GC_REGISTER_ROOT_FIXED (thread_start_args);
		thread_start_args = mono_g_hash_table_new (NULL, NULL);
	}
	mono_g_hash_table_insert (thread_start_args, thread, start_info->start_arg);
}