gxps_zip_archive_destroy (ZipArchive *zip)
{
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	/* This is a deprecated synonym for archive_read_free() in libarchive
	 * 3.0; but is not deprecated in libarchive 2.0, which we continue to
	 * support. */
	archive_read_finish (zip->archive);
G_GNUC_END_IGNORE_DEPRECATIONS
	g_slice_free (ZipArchive, zip);
}