setup (void)
{
	gchar *full_pfx;
	gchar *cp_pfx;

	G_LOCK (mutex);
	if (prefix != NULL) {
		G_UNLOCK (mutex);
		return;
	}

	/* This requires that the libedataserver DLL is installed in $bindir */
	full_pfx = g_win32_get_package_installation_directory_of_module (hmodule);
	cp_pfx = g_win32_locale_filename_from_utf8 (full_pfx);

	prefix = g_strdup (full_pfx);
	cp_prefix = g_strdup (cp_pfx);

	g_free (full_pfx);
	g_free (cp_pfx);

	localedir = replace_prefix (cp_prefix, E_DATA_SERVER_LOCALEDIR);
	imagesdir = replace_prefix (prefix, E_DATA_SERVER_IMAGESDIR);
	credentialmoduledir = replace_prefix (prefix, E_DATA_SERVER_CREDENTIALMODULEDIR);
	uimoduledir = replace_prefix (prefix, E_DATA_SERVER_UIMODULEDIR);

	G_UNLOCK (mutex);
}