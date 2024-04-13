_archive_read (struct archive *archive,
	       void           *data,
	       const void    **buffer)
{
	ZipArchive *zip = (ZipArchive *)data;
	gssize read_bytes;

	*buffer = zip->buffer;
	read_bytes = g_input_stream_read (G_INPUT_STREAM (zip->stream),
					  zip->buffer,
					  sizeof (zip->buffer),
					  NULL,
					  &zip->error);
	return read_bytes;
}