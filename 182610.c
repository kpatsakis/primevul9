gxps_archive_input_stream_finalize (GObject *object)
{
	GXPSArchiveInputStream *stream = GXPS_ARCHIVE_INPUT_STREAM (object);

	g_clear_pointer (&stream->zip, gxps_zip_archive_destroy);

	G_OBJECT_CLASS (gxps_archive_input_stream_parent_class)->finalize (object);
}