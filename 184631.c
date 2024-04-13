PHPAPI php_stream *_php_stream_fopen_tmpfile(int dummy STREAMS_DC)
{
	return php_stream_fopen_temporary_file(NULL, "php", NULL);
}