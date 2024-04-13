ves_icall_System_Environment_get_HasShutdownStarted (void)
{
	if (mono_runtime_is_shutting_down ())
		return TRUE;

	if (mono_domain_is_unloading (mono_domain_get ()))
		return TRUE;

	return FALSE;
}