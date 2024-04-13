_archive_close (struct archive *archive,
		void *data)
{
	ZipArchive *zip = (ZipArchive *)data;

	g_clear_object (&zip->stream);

	return ARCHIVE_OK;
}