replace_shadow_path (MonoDomain *domain, gchar *dirname, gchar **filename)
{
	gchar *content;
	gchar *shadow_ini_file;
	gsize len;

	/* Check for shadow-copied assembly */
	if (mono_is_shadow_copy_enabled (domain, dirname)) {
		shadow_ini_file = g_build_filename (dirname, "__AssemblyInfo__.ini", NULL);
		content = NULL;
		if (!g_file_get_contents (shadow_ini_file, &content, &len, NULL) ||
			!g_file_test (content, G_FILE_TEST_IS_REGULAR)) {
			if (content) {
				g_free (content);
				content = NULL;
			}
		}
		g_free (shadow_ini_file);
		if (content != NULL) {
			if (*filename)
				g_free (*filename);
			*filename = content;
			return TRUE;
		}
	}
	return FALSE;
}