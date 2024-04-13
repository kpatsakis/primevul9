PHPAPI php_stream *_php_stream_fopen_temporary_file(const char *dir, const char *pfx, zend_string **opened_path_ptr STREAMS_DC)
{
	zend_string *opened_path = NULL;
	int fd;

	fd = php_open_temporary_fd(dir, pfx, &opened_path);
	if (fd != -1)	{
		php_stream *stream;

		if (opened_path_ptr) {
			*opened_path_ptr = opened_path;
		}

		stream = php_stream_fopen_from_fd_int_rel(fd, "r+b", NULL);
		if (stream) {
			php_stdio_stream_data *self = (php_stdio_stream_data*)stream->abstract;
			stream->wrapper = &php_plain_files_wrapper;
			stream->orig_path = estrndup(ZSTR_VAL(opened_path), ZSTR_LEN(opened_path));

			self->temp_name = opened_path;
			self->lock_flag = LOCK_UN;

			return stream;
		}
		close(fd);

		php_error_docref(NULL, E_WARNING, "unable to allocate stream");

		return NULL;
	}
	return NULL;
}