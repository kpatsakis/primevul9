static int is_self_cloned(void)
{
	int fd, ret, is_cloned = 0;

	fd = open("/proc/self/exe", O_RDONLY|O_CLOEXEC);
	if (fd < 0)
		return -ENOTRECOVERABLE;

#ifdef HAVE_MEMFD_CREATE
	ret = fcntl(fd, F_GET_SEALS);
	is_cloned = (ret == RUNC_MEMFD_SEALS);
#else
	struct stat statbuf = {0};
	ret = fstat(fd, &statbuf);
	if (ret >= 0)
		is_cloned = (statbuf.st_nlink == 0);
#endif
	close(fd);
	return is_cloned;
}