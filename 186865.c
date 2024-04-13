file_pipe2file(struct magic_set *ms, int fd, const void *startbuf,
    size_t nbytes)
{
	char buf[4096];
	ssize_t r;
	int tfd;

	(void)strlcpy(buf, "/tmp/file.XXXXXX", sizeof buf);
#ifndef HAVE_MKSTEMP
	{
		char *ptr = mktemp(buf);
		tfd = open(ptr, O_RDWR|O_TRUNC|O_EXCL|O_CREAT, 0600);
		r = errno;
		(void)unlink(ptr);
		errno = r;
	}
#else
	{
		int te;
		tfd = mkstemp(buf);
		te = errno;
		(void)unlink(buf);
		errno = te;
	}
#endif
	if (tfd == -1) {
		file_error(ms, errno,
		    "cannot create temporary file for pipe copy");
		return -1;
	}

	if (swrite(tfd, startbuf, nbytes) != (ssize_t)nbytes)
		r = 1;
	else {
		while ((r = sread(fd, buf, sizeof(buf), 1)) > 0)
			if (swrite(tfd, buf, (size_t)r) != r)
				break;
	}

	switch (r) {
	case -1:
		file_error(ms, errno, "error copying from pipe to temp file");
		return -1;
	case 0:
		break;
	default:
		file_error(ms, errno, "error while writing to temp file");
		return -1;
	}

	/*
	 * We duplicate the file descriptor, because fclose on a
	 * tmpfile will delete the file, but any open descriptors
	 * can still access the phantom inode.
	 */
	if ((fd = dup2(tfd, fd)) == -1) {
		file_error(ms, errno, "could not dup descriptor for temp file");
		return -1;
	}
	(void)close(tfd);
	if (FINFO_LSEEK_FUNC(fd, (off_t)0, SEEK_SET) == (off_t)-1) {
		file_badseek(ms);
		return -1;
	}
	return fd;
}