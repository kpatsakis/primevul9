gxps_zip_archive_iter_next (ZipArchive            *zip,
                            struct archive_entry **entry)
{
        int result;

        result = archive_read_next_header (zip->archive, entry);
        if (result >= ARCHIVE_WARN && result <= ARCHIVE_OK) {
                if (result < ARCHIVE_OK) {
                        g_warning ("Error: %s\n", archive_error_string (zip->archive));
                        archive_set_error (zip->archive, ARCHIVE_OK, "No error");
                        archive_clear_error (zip->archive);
                }

                return TRUE;
        }

        return result != ARCHIVE_FATAL && result != ARCHIVE_EOF;
}