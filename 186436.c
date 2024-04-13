static int php_libxml_streams_IO_read(void *context, char *buffer, int len)
{
	TSRMLS_FETCH();
	return php_stream_read((php_stream*)context, buffer, len);
}