ves_icall_System_Environment_get_NewLine (void)
{
	MONO_ARCH_SAVE_REGS;

#if defined (PLATFORM_WIN32)
	return mono_string_new (mono_domain_get (), "\r\n");
#else
	return mono_string_new (mono_domain_get (), "\n");
#endif
}