_archive_open (struct archive *archive,
	       void           *data)
{
	ZipArchive *zip = (ZipArchive *)data;

	zip->stream = g_file_read (zip->file, NULL, &zip->error);

	return (zip->error) ? ARCHIVE_FATAL : ARCHIVE_OK;
}