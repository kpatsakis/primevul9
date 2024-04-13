gxps_archive_open (GXPSArchive *archive,
		   const gchar *path)
{
	GXPSArchiveInputStream *stream;
	gchar                  *first_piece_path = NULL;

	if (path == NULL)
		return NULL;

	if (path[0] == '/')
		path++;

	if (!g_hash_table_contains (archive->entries, path)) {
                first_piece_path = g_build_path ("/", path, "[0].piece", NULL);
                if (!g_hash_table_contains (archive->entries, first_piece_path)) {
                        g_free (first_piece_path);

                        return NULL;
                }
                path = first_piece_path;
        }

	stream = (GXPSArchiveInputStream *)g_object_new (GXPS_TYPE_ARCHIVE_INPUT_STREAM, NULL);
	stream->zip = gxps_zip_archive_create (archive->filename);
        stream->is_interleaved = first_piece_path != NULL;

        while (gxps_zip_archive_iter_next (stream->zip, &stream->entry)) {
                if (g_ascii_strcasecmp (path, archive_entry_pathname (stream->entry)) == 0)
                        break;
                archive_read_data_skip (stream->zip->archive);
        }

        g_free (first_piece_path);

	return G_INPUT_STREAM (stream);
}