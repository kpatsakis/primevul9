int f_dupfd(unsigned int from, struct file *file, unsigned flags)
{
	unsigned long nofile = rlimit(RLIMIT_NOFILE);
	int err;
	if (from >= nofile)
		return -EINVAL;
	err = alloc_fd(from, nofile, flags);
	if (err >= 0) {
		get_file(file);
		fd_install(err, file);
	}
	return err;
}