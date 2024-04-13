e_get_user_config_dir (void)
{
	static gchar *dirname = NULL;

	if (G_UNLIKELY (dirname == NULL)) {
		const gchar *config_dir = g_get_user_config_dir ();
		dirname = g_build_filename (config_dir, "evolution", NULL);
		g_mkdir_with_parents (dirname, 0700);
	}

	return dirname;
}