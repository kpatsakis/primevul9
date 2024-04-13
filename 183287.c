int psr_read(const char *filename)
{
	struct stat st;
	char *str, *map, *off, *end;
	int fd, err = 0;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return fd;

	if (fstat(fd, &st) < 0) {
		err = -errno;
		goto close;
	}

	map = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (!map || map == MAP_FAILED) {
		err = -errno;
		goto close;
	}

	off = map;

	while (1) {
		if (*off == '\r' || *off == '\n') {
			off++;
			continue;
		}

		end = strpbrk(off, "\r\n");
		if (!end)
			break;

		str = malloc(end - off + 1);
		if (!str)
			break;

		memset(str, 0, end - off + 1);
		strncpy(str, off, end - off);
		if (*str == '&')
			parse_line(str);

		free(str);
		off = end + 1;
	}

	munmap(map, st.st_size);

close:
	close(fd);

	return err;
}