int validate_headref(const char *path)
{
	struct stat st;
	char *buf, buffer[256];
	unsigned char sha1[20];
	int fd;
	ssize_t len;

	if (lstat(path, &st) < 0)
		return -1;

	/* Make sure it is a "refs/.." symlink */
	if (S_ISLNK(st.st_mode)) {
		len = readlink(path, buffer, sizeof(buffer)-1);
		if (len >= 5 && !memcmp("refs/", buffer, 5))
			return 0;
		return -1;
	}

	/*
	 * Anything else, just open it and try to see if it is a symbolic ref.
	 */
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return -1;
	len = read_in_full(fd, buffer, sizeof(buffer)-1);
	close(fd);

	/*
	 * Is it a symbolic ref?
	 */
	if (len < 4)
		return -1;
	if (!memcmp("ref:", buffer, 4)) {
		buf = buffer + 4;
		len -= 4;
		while (len && isspace(*buf))
			buf++, len--;
		if (len >= 5 && !memcmp("refs/", buf, 5))
			return 0;
	}

	/*
	 * Is this a detached HEAD?
	 */
	if (!get_sha1_hex(buffer, sha1))
		return 0;

	return -1;
}