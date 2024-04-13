gxps_archive_has_entry (GXPSArchive *archive,
			const gchar *path)
{
	if (path == NULL)
		return FALSE;

	if (path[0] == '/')
		path++;

	return g_hash_table_contains (archive->entries, path);
}