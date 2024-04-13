static size_t php_stdiop_read(php_stream *stream, char *buf, size_t count)
{
	php_stdio_stream_data *data = (php_stdio_stream_data*)stream->abstract;
	size_t ret;

	assert(data != NULL);

	if (data->fd >= 0) {
#ifdef PHP_WIN32
		php_stdio_stream_data *self = (php_stdio_stream_data*)stream->abstract;

		if ((self->is_pipe || self->is_process_pipe) && !self->is_pipe_blocking) {
			HANDLE ph = (HANDLE)_get_osfhandle(data->fd);
			int retry = 0;
			DWORD avail_read = 0;

			do {
				/* Look ahead to get the available data amount to read. Do the same
					as read() does, however not blocking forever. In case it failed,
					no data will be read (better than block). */
				if (!PeekNamedPipe(ph, NULL, 0, NULL, &avail_read, NULL)) {
					break;
				}
				/* If there's nothing to read, wait in 10ms periods. */
				if (0 == avail_read) {
					usleep(10);
				}
			} while (0 == avail_read && retry++ < 3200000);

			/* Reduce the required data amount to what is available, otherwise read()
				will block.*/
			if (avail_read < count) {
				count = avail_read;
			}
		}
#endif
		ret = read(data->fd, buf,  PLAIN_WRAP_BUF_SIZE(count));

		if (ret == (size_t)-1 && errno == EINTR) {
			/* Read was interrupted, retry once,
			   If read still fails, giveup with feof==0
			   so script can retry if desired */
			ret = read(data->fd, buf,  PLAIN_WRAP_BUF_SIZE(count));
		}

		stream->eof = (ret == 0 || (ret == (size_t)-1 && errno != EWOULDBLOCK && errno != EINTR && errno != EBADF));

	} else {
#if HAVE_FLUSHIO
		if (!data->is_pipe && data->last_op == 'w')
			zend_fseek(data->file, 0, SEEK_CUR);
		data->last_op = 'r';
#endif

		ret = fread(buf, 1, count, data->file);

		stream->eof = feof(data->file);
	}
	return ret;
}