static int php_stdiop_cast(php_stream *stream, int castas, void **ret)
{
	php_socket_t fd;
	php_stdio_stream_data *data = (php_stdio_stream_data*) stream->abstract;

	assert(data != NULL);

	/* as soon as someone touches the stdio layer, buffering may ensue,
	 * so we need to stop using the fd directly in that case */

	switch (castas)	{
		case PHP_STREAM_AS_STDIO:
			if (ret) {

				if (data->file == NULL) {
					/* we were opened as a plain file descriptor, so we
					 * need fdopen now */
					char fixed_mode[5];
					php_stream_mode_sanitize_fdopen_fopencookie(stream, fixed_mode);
					data->file = fdopen(data->fd, fixed_mode);
					if (data->file == NULL) {
						return FAILURE;
					}
				}

				*(FILE**)ret = data->file;
				data->fd = SOCK_ERR;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD_FOR_SELECT:
			PHP_STDIOP_GET_FD(fd, data);
			if (SOCK_ERR == fd) {
				return FAILURE;
			}
			if (ret) {
				*(php_socket_t *)ret = fd;
			}
			return SUCCESS;

		case PHP_STREAM_AS_FD:
			PHP_STDIOP_GET_FD(fd, data);

			if (SOCK_ERR == fd) {
				return FAILURE;
			}
			if (data->file) {
				fflush(data->file);
			}
			if (ret) {
				*(php_socket_t *)ret = fd;
			}
			return SUCCESS;
		default:
			return FAILURE;
	}
}