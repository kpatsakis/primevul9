ves_icall_System_Threading_Thread_GetAbortExceptionState (MonoThread *this)
{
	MonoInternalThread *thread = this->internal_thread;
	MonoObject *state, *deserialized = NULL, *exc;
	MonoDomain *domain;

	if (!thread->abort_state_handle)
		return NULL;

	state = mono_gchandle_get_target (thread->abort_state_handle);
	g_assert (state);

	domain = mono_domain_get ();
	if (mono_object_domain (state) == domain)
		return state;

	deserialized = mono_object_xdomain_representation (state, domain, &exc);

	if (!deserialized) {
		MonoException *invalid_op_exc = mono_get_exception_invalid_operation ("Thread.ExceptionState cannot access an ExceptionState from a different AppDomain");
		if (exc)
			MONO_OBJECT_SETREF (invalid_op_exc, inner_ex, exc);
		mono_raise_exception (invalid_op_exc);
	}

	return deserialized;
}