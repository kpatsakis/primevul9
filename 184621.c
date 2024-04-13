static size_t php_stdiop_write(php_stream *stream, const char *buf, size_t count)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;

	assert(data != NULL);

	if (data->fd >= 0) {
#ifdef PHP_WIN32
		int bytes_written;
		if (ZEND_SIZE_T_UINT_OVFL(count)) {
			count = UINT_MAX;
		}
		bytes_written = _write(data->fd, buf, (unsigned int)count);
#else
		int bytes_written = write(data->fd, buf, count);
#endif
		if (bytes_written < 0) return 0;
		return (size_t) bytes_written;
	} else {

#if HAVE_FLUSHIO
		if (!data->is_pipe && data->last_op == 'r') {
			zend_fseek(data->file, 0, SEEK_CUR);
		}
		data->last_op = 'w';
#endif

		return fwrite(buf, 1, count, data->file);
	}
}