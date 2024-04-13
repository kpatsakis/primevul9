static int php_plain_files_dirstream_close(php_stream *stream, int close_handle)
{
	return closedir((DIR *)stream->abstract);
}