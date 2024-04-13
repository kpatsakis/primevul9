ves_icall_System_Threading_Thread_Suspend (MonoInternalThread *thread)
{
	if (!mono_thread_suspend (thread))
		mono_raise_exception (mono_get_exception_thread_state ("Thread has not been started, or is dead."));
}