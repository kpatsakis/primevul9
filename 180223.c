get_bundled_machine_config (void)
{
	const gchar *machine_config;

	MONO_ARCH_SAVE_REGS;

	machine_config = mono_get_machine_config ();

	if (!machine_config)
		return NULL;

	return mono_string_new (mono_domain_get (), machine_config);
}