_g_file_is_temp_work_dir (GFile *file)
{
	gboolean  result = FALSE;
	char     *path;
	int       i;

	path = g_file_get_path (file);
	if (path[0] != '/') {
		g_free (path);
		return FALSE;
	}

	for (i = 0; try_folder[i] != NULL; i++) {
		const char *folder;

		folder = get_nth_temp_folder_to_try (i);
		if (strncmp (path, folder, strlen (folder)) == 0) {
			if (strncmp (path + strlen (folder), "/.fr-", 5) == 0) {
				result = TRUE;
				break;
			}
		}
	}

	g_free (path);

	return result;
}