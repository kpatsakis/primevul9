static char *read_file(char *path, size_t *length)
{
	int fd;
	char buf[4096], *copy = NULL;

	if (!length)
		return NULL;

	fd = open(path, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		return NULL;

	*length = 0;
	for (;;) {
		int n;

		n = read(fd, buf, sizeof(buf));
		if (n < 0)
			goto error;
		if (!n)
			break;

		copy = must_realloc(copy, (*length + n) * sizeof(*copy));
		memcpy(copy + *length, buf, n);
		*length += n;
	}
	close(fd);
	return copy;

error:
	close(fd);
	free(copy);
	return NULL;
}