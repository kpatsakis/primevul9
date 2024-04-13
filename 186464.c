static int php_libxml_streams_IO_write(void *context, const char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_write((php_stream*)context, buffer, len);
}