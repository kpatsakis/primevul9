list_archive_thread (GSimpleAsyncResult *result,
		     GObject            *object,
		     GCancellable       *cancellable)
{
	LoadData             *load_data;
	struct archive       *a;
	struct archive_entry *entry;
	int                   r;

	load_data = g_simple_async_result_get_op_res_gpointer (result);

	fr_archive_progress_set_total_bytes (load_data->archive,
					     _g_file_get_size (fr_archive_get_file (load_data->archive), cancellable));

	r = create_read_object (load_data, &a);
	if (r != ARCHIVE_OK) {
		archive_read_free(a);
		return;
	}

	while ((r = archive_read_next_header (a, &entry)) == ARCHIVE_OK) {
		FileData   *file_data;
		const char *pathname;

		if (g_cancellable_is_cancelled (cancellable))
			break;

		file_data = file_data_new ();

		if (archive_entry_size_is_set (entry)) {
			file_data->size = archive_entry_size (entry);
			FR_ARCHIVE_LIBARCHIVE (load_data->archive)->priv->uncompressed_size += file_data->size;
		}

		if (archive_entry_mtime_is_set (entry))
			file_data->modified =  archive_entry_mtime (entry);

		if (archive_entry_filetype (entry) == AE_IFLNK)
			file_data->link = g_strdup (archive_entry_symlink (entry));

		pathname = archive_entry_pathname (entry);
		if (*pathname == '/') {
			file_data->full_path = g_strdup (pathname);
			file_data->original_path = file_data->full_path;
		}
		else {
			file_data->full_path = g_strconcat ("/", pathname, NULL);
			file_data->original_path = file_data->full_path + 1;
		}

		file_data->dir = (archive_entry_filetype (entry) == AE_IFDIR);
		if (file_data->dir)
			file_data->name = _g_path_get_dir_name (file_data->full_path);
		else
			file_data->name = g_strdup (_g_path_get_basename (file_data->full_path));
		file_data->path = _g_path_remove_level (file_data->full_path);

		/*
		g_print ("%s\n", archive_entry_pathname (entry));
		g_print ("\tfull_path: %s\n", file_data->full_path);
		g_print ("\toriginal_path: %s\n", file_data->original_path);
		g_print ("\tname: %s\n", file_data->name);
		g_print ("\tpath: %s\n", file_data->path);
		g_print ("\tlink: %s\n", file_data->link);
		*/

		fr_archive_add_file (load_data->archive, file_data);

		archive_read_data_skip (a);
	}
	archive_read_free (a);

	if ((load_data->error == NULL) && (r != ARCHIVE_EOF) && (archive_error_string (a) != NULL))
		load_data->error = _g_error_new_from_archive_error (archive_error_string (a));
	if (load_data->error == NULL)
		g_cancellable_set_error_if_cancelled (cancellable, &load_data->error);
	if (load_data->error != NULL)
		g_simple_async_result_set_from_error (result, load_data->error);

	load_data_free (load_data);
}