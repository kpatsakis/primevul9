static int php_libxml_streams_IO_close(void *context)
{
	TSRMLS_FETCH();
	return php_stream_close((php_stream*)context);
}