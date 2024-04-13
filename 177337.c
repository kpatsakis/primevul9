gchar *conv_filename_to_utf8(const gchar *fs_file)
{
	gchar *utf8_file = NULL;
	GError *error = NULL;

	utf8_file = g_filename_to_utf8(fs_file, -1, NULL, NULL, &error);
	if (error) {
		g_warning("failed to convert encoding of file name: %s",
			  error->message);
		g_error_free(error);
	}

	if (!utf8_file || !g_utf8_validate(utf8_file, -1, NULL)) {
		g_free(utf8_file);
		utf8_file = g_strdup(fs_file);
		conv_unreadable_8bit(utf8_file);
	}

	return utf8_file;
}