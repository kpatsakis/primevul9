const char *read_gitfile_gently(const char *path)
{
	char *buf;
	char *dir;
	const char *slash;
	struct stat st;
	int fd;
	size_t len;

	if (stat(path, &st))
		return NULL;
	if (!S_ISREG(st.st_mode))
		return NULL;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		die_errno("Error opening '%s'", path);
	buf = xmalloc(st.st_size + 1);
	len = read_in_full(fd, buf, st.st_size);
	close(fd);
	if (len != st.st_size)
		die("Error reading %s", path);
	buf[len] = '\0';
	if (prefixcmp(buf, "gitdir: "))
		die("Invalid gitfile format: %s", path);
	while (buf[len - 1] == '\n' || buf[len - 1] == '\r')
		len--;
	if (len < 9)
		die("No path in gitfile: %s", path);
	buf[len] = '\0';
	dir = buf + 8;

	if (!is_absolute_path(dir) && (slash = strrchr(path, '/'))) {
		size_t pathlen = slash+1 - path;
		size_t dirlen = pathlen + len - 8;
		dir = xmalloc(dirlen + 1);
		strncpy(dir, path, pathlen);
		strncpy(dir + pathlen, buf + 8, len - 8);
		dir[dirlen] = '\0';
		free(buf);
		buf = dir;
	}

	if (!is_git_directory(dir))
		die("Not a git repository: %s", dir);
	path = make_absolute_path(dir);

	free(buf);
	return path;
}