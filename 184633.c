PHPAPI php_stream *_php_stream_fopen_from_file(FILE *file, const char *mode STREAMS_DC)
{
	php_stream *stream = php_stream_fopen_from_file_int_rel(file, mode);

	if (stream) {
		php_stdio_stream_data *self = (php_stdio_stream_data*)stream->abstract;

#ifdef S_ISFIFO
		/* detect if this is a pipe */
		if (self->fd >= 0) {
			self->is_pipe = (do_fstat(self, 0) == 0 && S_ISFIFO(self->sb.st_mode)) ? 1 : 0;
		}
#elif defined(PHP_WIN32)
		{
			zend_uintptr_t handle = _get_osfhandle(self->fd);

			if (handle != (zend_uintptr_t)INVALID_HANDLE_VALUE) {
				self->is_pipe = GetFileType((HANDLE)handle) == FILE_TYPE_PIPE;
			}
		}
#endif

		if (self->is_pipe) {
			stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
		} else {
			stream->position = zend_ftell(file);
		}
	}

	return stream;
}