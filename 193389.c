mono_thread_set_execution_context (MonoObject *ec)
{
	*get_execution_context_addr () = ec;
}