mono_thread_get_execution_context (void)
{
	return *get_execution_context_addr ();
}