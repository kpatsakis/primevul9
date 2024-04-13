int __close_fd_get_file(unsigned int fd, struct file **res)
{
	struct files_struct *files = current->files;
	struct file *file;
	struct fdtable *fdt;

	fdt = files_fdtable(files);
	if (fd >= fdt->max_fds)
		goto out_err;
	file = fdt->fd[fd];
	if (!file)
		goto out_err;
	rcu_assign_pointer(fdt->fd[fd], NULL);
	__put_unused_fd(files, fd);
	get_file(file);
	*res = file;
	return 0;
out_err:
	*res = NULL;
	return -ENOENT;
}