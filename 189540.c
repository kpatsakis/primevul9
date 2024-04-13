e_get_user_data_dir (void)
{
	static gchar *dirname = NULL;

	if (G_UNLIKELY (dirname == NULL)) {
		const gchar *data_dir = g_get_user_data_dir ();
		dirname = g_build_filename (data_dir, "evolution", NULL);
		g_mkdir_with_parents (dirname, 0700);
	}

	return dirname;
}