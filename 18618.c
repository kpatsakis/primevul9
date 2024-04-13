static struct file *pick_file(struct files_struct *files, unsigned fd)
{
	struct file *file;
	struct fdtable *fdt;

	spin_lock(&files->file_lock);
	fdt = files_fdtable(files);
	if (fd >= fdt->max_fds) {
		file = ERR_PTR(-EINVAL);
		goto out_unlock;
	}
	file = fdt->fd[fd];
	if (!file) {
		file = ERR_PTR(-EBADF);
		goto out_unlock;
	}
	rcu_assign_pointer(fdt->fd[fd], NULL);
	__put_unused_fd(files, fd);

out_unlock:
	spin_unlock(&files->file_lock);
	return file;
}