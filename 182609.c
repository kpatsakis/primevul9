gxps_archive_input_stream_close (GInputStream  *stream,
				 GCancellable  *cancellable,
				 GError       **error)
{
	GXPSArchiveInputStream *istream = GXPS_ARCHIVE_INPUT_STREAM (stream);

	if (g_cancellable_set_error_if_cancelled (cancellable, error))
		return FALSE;

	g_clear_pointer (&istream->zip, gxps_zip_archive_destroy);

	return TRUE;
}