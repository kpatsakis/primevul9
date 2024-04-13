gxps_archive_initable_init (GInitable     *initable,
			    GCancellable  *cancellable,
			    GError       **error)
{
	GXPSArchive          *archive;
	ZipArchive           *zip;
	struct archive_entry *entry;
	const gchar          *pathname;

	archive = GXPS_ARCHIVE (initable);

	if (archive->initialized) {
		if (archive->init_error) {
			g_propagate_error (error, g_error_copy (archive->init_error));
			return FALSE;
		}

		return TRUE;
	}

	archive->initialized = TRUE;

	zip = gxps_zip_archive_create (archive->filename);
	if (zip->error) {
		g_propagate_error (&archive->init_error, zip->error);
		g_propagate_error (error, g_error_copy (archive->init_error));
		gxps_zip_archive_destroy (zip);
		return FALSE;
	}

        while (gxps_zip_archive_iter_next (zip, &entry)) {
                /* FIXME: We can ignore directories here */
                pathname = archive_entry_pathname (entry);
                if (pathname != NULL)
                        g_hash_table_add (archive->entries, g_strdup (pathname));
                archive_read_data_skip (zip->archive);
        }

	gxps_zip_archive_destroy (zip);

	return TRUE;
}