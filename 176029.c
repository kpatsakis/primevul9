SYSCALL_DEFINE1(userfaultfd, int, flags)
{
	int fd, error;
	struct file *file;

	error = get_unused_fd_flags(flags & UFFD_SHARED_FCNTL_FLAGS);
	if (error < 0)
		return error;
	fd = error;

	file = userfaultfd_file_create(flags);
	if (IS_ERR(file)) {
		error = PTR_ERR(file);
		goto err_put_unused_fd;
	}
	fd_install(fd, file);

	return fd;

err_put_unused_fd:
	put_unused_fd(fd);

	return error;
}