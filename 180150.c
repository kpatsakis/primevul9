ves_icall_System_IO_get_temp_path (void)
{
	MONO_ARCH_SAVE_REGS;

	return mono_string_new (mono_domain_get (), g_get_tmp_dir ());
}