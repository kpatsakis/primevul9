static int do_fstat(php_stdio_stream_data *d, int force)
{
	if (!d->cached_fstat || force) {
		int fd;
		int r;

		PHP_STDIOP_GET_FD(fd, d);
		r = zend_fstat(fd, &d->sb);
		d->cached_fstat = r == 0;

		return r;
	}
	return 0;
}