gchar *conv_filename_from_utf8(const gchar *utf8_file)
{
	gchar *fs_file;
	GError *error = NULL;

	fs_file = g_filename_from_utf8(utf8_file, -1, NULL, NULL, &error);
	if (error) {
		debug_print("failed to convert encoding of file name: %s\n",
			  error->message);
		g_error_free(error);
	}
	if (!fs_file)
		fs_file = g_strdup(utf8_file);

	return fs_file;
}