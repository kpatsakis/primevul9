e_get_user_cache_dir (void)
{
	static gchar *dirname = NULL;

	if (G_UNLIKELY (dirname == NULL)) {
		const gchar *cache_dir = g_get_user_cache_dir ();
		dirname = g_build_filename (cache_dir, "evolution", NULL);
		g_mkdir_with_parents (dirname, 0700);
	}

	return dirname;
}