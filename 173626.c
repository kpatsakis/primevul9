static size_t php_zip_ops_write(php_stream *stream, const char *buf, size_t count TSRMLS_DC)
{
	if (!stream) {
		return 0;
	}

	return count;
}