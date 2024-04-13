gxps_images_guess_content_type (GXPSArchive *zip,
				const gchar *image_uri)
{
	GInputStream *stream;
	guchar        buffer[1024];
	gssize        bytes_read;
	gchar        *mime_type;

	stream = gxps_archive_open (zip, image_uri);
	if (!stream)
		return NULL;

	bytes_read = g_input_stream_read (stream, buffer, 1024, NULL, NULL);
	mime_type = g_content_type_guess (NULL, buffer, bytes_read, NULL);
	g_object_unref (stream);

	return mime_type;
}