ves_icall_System_Environment_InternalGetHome (void)
{
	MONO_ARCH_SAVE_REGS;

	return mono_string_new (mono_domain_get (), g_get_home_dir ());
}