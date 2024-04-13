static int clone_binary(void)
{
	int binfd, memfd;
	ssize_t sent = 0;

#ifdef HAVE_MEMFD_CREATE
	memfd = memfd_create(RUNC_MEMFD_COMMENT, MFD_CLOEXEC | MFD_ALLOW_SEALING);
#else
	memfd = open("/tmp", O_TMPFILE | O_EXCL | O_RDWR | O_CLOEXEC, 0711);
#endif
	if (memfd < 0)
		return -ENOTRECOVERABLE;

	binfd = open("/proc/self/exe", O_RDONLY | O_CLOEXEC);
	if (binfd < 0)
		goto error;

	sent = sendfile(memfd, binfd, NULL, RUNC_SENDFILE_MAX);
	close(binfd);
	if (sent < 0)
		goto error;

#ifdef HAVE_MEMFD_CREATE
	int err = fcntl(memfd, F_ADD_SEALS, RUNC_MEMFD_SEALS);
	if (err < 0)
		goto error;
#else
	/* Need to re-open "memfd" as read-only to avoid execve(2) giving -EXTBUSY. */
	int newfd;
	char *fdpath = NULL;

	if (asprintf(&fdpath, "/proc/self/fd/%d", memfd) < 0)
		goto error;
	newfd = open(fdpath, O_RDONLY | O_CLOEXEC);
	free(fdpath);
	if (newfd < 0)
		goto error;

	close(memfd);
	memfd = newfd;
#endif
	return memfd;

error:
	close(memfd);
	return -EIO;
}