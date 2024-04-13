static int php_stdiop_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;
	int ret;

	assert(data != NULL);

	if (data->is_pipe) {
		php_error_docref(NULL, E_WARNING, "cannot seek on a pipe");
		return -1;
	}

	if (data->fd >= 0) {
		zend_off_t result;

		result = zend_lseek(data->fd, offset, whence);
		if (result == (zend_off_t)-1)
			return -1;

		*newoffset = result;
		return 0;

	} else {
		ret = zend_fseek(data->file, offset, whence);
		*newoffset = zend_ftell(data->file);
		return ret;
	}
}