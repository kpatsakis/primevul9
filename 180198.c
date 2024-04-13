ves_icall_System_Configuration_DefaultConfig_get_machine_config_path (void)
{
	MonoString *mcpath;
	gchar *path;

	MONO_ARCH_SAVE_REGS;

	path = g_build_path (G_DIR_SEPARATOR_S, mono_get_config_dir (), "mono", mono_get_runtime_info ()->framework_version, "machine.config", NULL);

#if defined (PLATFORM_WIN32)
	/* Avoid mixing '/' and '\\' */
	{
		gint i;
		for (i = strlen (path) - 1; i >= 0; i--)
			if (path [i] == '/')
				path [i] = '\\';
	}
#endif
	mcpath = mono_string_new (mono_domain_get (), path);
	g_free (path);

	return mcpath;
}