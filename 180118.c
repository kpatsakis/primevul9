ves_icall_GetCurrentMethod (void) 
{
	MonoMethod *m = mono_method_get_last_managed ();

	MONO_ARCH_SAVE_REGS;

	return mono_method_get_object (mono_domain_get (), m, NULL);
}