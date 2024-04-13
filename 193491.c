mono_thread_get_undeniable_exception (void)
{
	MonoInternalThread *thread = mono_thread_internal_current ();

	if (thread && thread->abort_exc && !is_running_protected_wrapper ()) {
		/*
		 * FIXME: Clear the abort exception and return an AppDomainUnloaded 
		 * exception if the thread no longer references a dying appdomain.
		 */
		thread->abort_exc->trace_ips = NULL;
		thread->abort_exc->stack_trace = NULL;
		return thread->abort_exc;
	}

	return NULL;
}