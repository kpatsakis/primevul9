ves_icall_System_Environment_get_UserName (void)
{
	MONO_ARCH_SAVE_REGS;

	/* using glib is more portable */
	return mono_string_new (mono_domain_get (), g_get_user_name ());
}