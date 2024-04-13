PHPAPI php_stream *_php_stream_fopen_from_fd(int fd, const char *mode, const char *persistent_id STREAMS_DC)
{
	php_stream *stream = php_stream_fopen_from_fd_int_rel(fd, mode, persistent_id);

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
			stream->position = zend_lseek(self->fd, 0, SEEK_CUR);
#ifdef ESPIPE
			if (stream->position == (zend_off_t)-1 && errno == ESPIPE) {
				stream->position = 0;
				stream->flags |= PHP_STREAM_FLAG_NO_SEEK;
				self->is_pipe = 1;
			}
#endif
		}
	}

	return stream;
}