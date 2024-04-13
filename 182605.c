gxps_archive_input_stream_next_piece (GXPSArchiveInputStream *stream)
{
        gchar *dirname;
        gchar *prefix;

        if (!stream->is_interleaved)
                return;

        dirname = g_path_get_dirname (archive_entry_pathname (stream->entry));
        if (!dirname)
                return;

        stream->piece++;
        prefix = g_strdup_printf ("%s/[%u]", dirname, stream->piece);
        g_free (dirname);

        while (gxps_zip_archive_iter_next (stream->zip, &stream->entry)) {
                if (g_str_has_prefix (archive_entry_pathname (stream->entry), prefix)) {
                        const gchar *suffix = archive_entry_pathname (stream->entry) + strlen (prefix);

                        if (g_ascii_strcasecmp (suffix, ".piece") == 0 ||
                            g_ascii_strcasecmp (suffix, ".last.piece") == 0)
                                break;
                }
                archive_read_data_skip (stream->zip->archive);
        }

        g_free (prefix);
}