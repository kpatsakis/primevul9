gxps_zip_archive_create (GFile *filename)
{
	ZipArchive *zip;

	zip = g_slice_new0 (ZipArchive);
	zip->file = filename;
	zip->archive = archive_read_new ();
	archive_read_support_format_zip (zip->archive);
	archive_read_open2 (zip->archive,
			    zip,
			    _archive_open,
			    _archive_read,
			    _archive_skip,
			    _archive_close);
	return zip;
}