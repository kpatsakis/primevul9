void mono_thread_stop (MonoThread *thread)
{
	mono_thread_internal_stop (thread->internal_thread);
}