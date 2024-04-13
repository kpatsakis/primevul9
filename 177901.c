PS_WRITE_FUNC(files)
{
	long n;
	PS_FILES_DATA;

	ps_files_open(data, key TSRMLS_CC);
	if (data->fd < 0) {
		return FAILURE;
	}

	/* Truncate file if the amount of new data is smaller than the existing data set. */

	if (vallen < (int)data->st_size) {
		php_ignore_value(ftruncate(data->fd, 0));
	}

#if defined(HAVE_PWRITE)
	n = pwrite(data->fd, val, vallen, 0);
#else
	lseek(data->fd, 0, SEEK_SET);
	n = write(data->fd, val, vallen);
#endif

	if (n != vallen) {
		if (n == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "write failed: %s (%d)", strerror(errno), errno);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "write wrote less bytes than requested");
		}
		return FAILURE;
	}

	return SUCCESS;
}