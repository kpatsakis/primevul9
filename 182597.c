gxps_archive_finalize (GObject *object)
{
	GXPSArchive *archive = GXPS_ARCHIVE (object);

	g_clear_pointer (&archive->entries, g_hash_table_unref);
	g_clear_object (&archive->filename);
	g_clear_error (&archive->init_error);
	g_clear_object (&archive->resources);

	G_OBJECT_CLASS (gxps_archive_parent_class)->finalize (object);
}