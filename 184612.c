static int php_plain_files_dirstream_rewind(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffs)
{
	rewinddir((DIR *)stream->abstract);
	return 0;
}