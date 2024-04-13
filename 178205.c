static int write_file(char *data, size_t data_len, char *pathfmt, ...)
{
	int fd, len, ret = 0;
	char path[PATH_MAX];

	va_list ap;
	va_start(ap, pathfmt);
	len = vsnprintf(path, PATH_MAX, pathfmt, ap);
	va_end(ap);
	if (len < 0)
		return -1;

	fd = open(path, O_RDWR);
	if (fd < 0) {
		return -1;
	}

	len = write(fd, data, data_len);
	if (len != data_len) {
		ret = -1;
		goto out;
	}

 out:
	close(fd);
	return ret;
}