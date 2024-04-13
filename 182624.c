gxps_archive_init (GXPSArchive *archive)
{
	archive->entries = g_hash_table_new_full (caseless_hash, caseless_equal, g_free, NULL);
}