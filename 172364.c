imapx_copy_move_message_cache (CamelFolder *source_folder,
			       CamelFolder *destination_folder,
			       gboolean delete_originals,
			       const gchar *source_uid,
			       const gchar *destination_uid,
			       GCancellable *cancellable)
{
	CamelIMAPXFolder *imapx_source_folder, *imapx_destination_folder;
	gchar *source_filename, *destination_filename;

	g_return_if_fail (CAMEL_IS_IMAPX_FOLDER (source_folder));
	g_return_if_fail (CAMEL_IS_IMAPX_FOLDER (destination_folder));
	g_return_if_fail (source_uid != NULL);
	g_return_if_fail (destination_uid != NULL);

	imapx_source_folder = CAMEL_IMAPX_FOLDER (source_folder);
	imapx_destination_folder = CAMEL_IMAPX_FOLDER (destination_folder);

	source_filename = camel_data_cache_get_filename  (imapx_source_folder->cache, "cur", source_uid);
	if (!g_file_test (source_filename, G_FILE_TEST_EXISTS)) {
		g_free (source_filename);
		return;
	}

	destination_filename = camel_data_cache_get_filename  (imapx_destination_folder->cache, "cur", destination_uid);
	if (!g_file_test (destination_filename, G_FILE_TEST_EXISTS)) {
		GIOStream *stream;

		/* To create the cache folder structure for the message file */
		stream = camel_data_cache_add (imapx_destination_folder->cache, "cur", destination_uid, NULL);
		if (stream) {
			g_clear_object (&stream);

			/* Remove the empty file, it's gonna be replaced with actual message */
			g_unlink (destination_filename);

			if (delete_originals) {
				if (g_rename (source_filename, destination_filename) == -1 && errno != ENOENT) {
					g_warning ("%s: Failed to rename '%s' to '%s': %s", G_STRFUNC, source_filename, destination_filename, g_strerror (errno));
				}
			} else {
				GFile *source, *destination;
				GError *local_error = NULL;

				source = g_file_new_for_path (source_filename);
				destination = g_file_new_for_path (destination_filename);

				if (source && destination &&
				    !g_file_copy (source, destination, G_FILE_COPY_NONE, cancellable, NULL, NULL, &local_error)) {
					if (local_error) {
						g_warning ("%s: Failed to copy '%s' to '%s': %s", G_STRFUNC, source_filename, destination_filename, local_error->message);
					}
				}

				g_clear_object (&source);
				g_clear_object (&destination);
				g_clear_error (&local_error);
			}
		}
	}

	g_free (source_filename);
	g_free (destination_filename);
}