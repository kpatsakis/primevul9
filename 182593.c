gxps_archive_input_stream_class_init (GXPSArchiveInputStreamClass *klass)
{
	GObjectClass      *object_class = G_OBJECT_CLASS (klass);
	GInputStreamClass *istream_class = G_INPUT_STREAM_CLASS (klass);

	object_class->finalize = gxps_archive_input_stream_finalize;

	istream_class->read_fn = gxps_archive_input_stream_read;
	istream_class->skip = gxps_archive_input_stream_skip;
	istream_class->close_fn = gxps_archive_input_stream_close;
}