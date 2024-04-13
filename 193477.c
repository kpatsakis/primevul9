mono_thread_set_manage_callback (MonoThread *thread, MonoThreadManageCallback func)
{
	thread->internal_thread->manage_callback = func;
}