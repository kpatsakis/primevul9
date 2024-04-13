ves_icall_System_Reflection_Assembly_get_code_base (MonoReflectionAssembly *assembly, MonoBoolean escaped)
{
	MonoDomain *domain = mono_object_domain (assembly); 
	MonoAssembly *mass = assembly->assembly;
	MonoString *res = NULL;
	gchar *uri;
	gchar *absolute;
	gchar *dirname;
	
	MONO_ARCH_SAVE_REGS;

	if (g_path_is_absolute (mass->image->name)) {
		absolute = g_strdup (mass->image->name);
		dirname = g_path_get_dirname (absolute);
	} else {
		absolute = g_build_filename (mass->basedir, mass->image->name, NULL);
		dirname = g_strdup (mass->basedir);
	}

	replace_shadow_path (domain, dirname, &absolute);
	g_free (dirname);
#if PLATFORM_WIN32
	{
		gint i;
		for (i = strlen (absolute) - 1; i >= 0; i--)
			if (absolute [i] == '\\')
				absolute [i] = '/';
	}
#endif
	if (escaped) {
		uri = g_filename_to_uri (absolute, NULL, NULL);
	} else {
		const char *prepend = "file://";
#if PLATFORM_WIN32
		if (*absolute == '/' && *(absolute + 1) == '/') {
			prepend = "file:";
		} else {
			prepend = "file:///";
		}
#endif
		uri = g_strconcat (prepend, absolute, NULL);
	}

	if (uri) {
		res = mono_string_new (domain, uri);
		g_free (uri);
	}
	g_free (absolute);
	return res;
}