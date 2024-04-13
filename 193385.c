static void CALLBACK interruption_request_apc (ULONG_PTR param)
{
	MonoException* exc = mono_thread_request_interruption (FALSE);
	if (exc) mono_raise_exception (exc);
}