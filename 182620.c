_archive_skip (struct archive *archive,
	       void           *data,
	       __LA_INT64_T    request)
{
	ZipArchive *zip = (ZipArchive *)data;

	if (!g_seekable_can_seek (G_SEEKABLE (zip->stream)))
		return 0;

	g_seekable_seek (G_SEEKABLE (zip->stream),
			 request,
			 G_SEEK_CUR,
			 NULL,
			 &zip->error);

	if (zip->error) {
		g_clear_error (&zip->error);
		request = 0;
	}

	return request;
}