ves_icall_System_Web_Util_ICalls_get_machine_install_dir (void)
{
	MonoString *ipath;
	gchar *path;

	MONO_ARCH_SAVE_REGS;

	path = g_path_get_dirname (mono_get_config_dir ());

#if defined (PLATFORM_WIN32)
	/* Avoid mixing '/' and '\\' */
	{
		gint i;
		for (i = strlen (path) - 1; i >= 0; i--)
			if (path [i] == '/')
				path [i] = '\\';
	}
#endif
	ipath = mono_string_new (mono_domain_get (), path);
	g_free (path);

	return ipath;
}