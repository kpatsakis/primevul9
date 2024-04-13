ves_icall_System_Threading_Thread_Resume (MonoThread *thread)
{
	if (!thread->internal_thread || !mono_thread_resume (thread->internal_thread))
		mono_raise_exception (mono_get_exception_thread_state ("Thread has not been started, or is dead."));
}