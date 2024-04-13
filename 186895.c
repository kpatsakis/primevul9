swrite(int fd, const void *buf, size_t n)
{
	ssize_t rv;
	size_t rn = n;

	do
		switch (rv = write(fd, buf, n)) {
		case -1:
			if (errno == EINTR)
				continue;
			return -1;
		default:
			n -= rv;
			buf = CAST(const char *, buf) + rv;
			break;
		}
	while (n > 0);
	return rn;
}