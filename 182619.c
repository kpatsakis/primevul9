gxps_archive_input_stream_is_last_piece (GXPSArchiveInputStream *stream)
{
        return g_str_has_suffix (archive_entry_pathname (stream->entry), ".last.piece");
}