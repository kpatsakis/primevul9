int iterate_fd(struct files_struct *files, unsigned n,
		int (*f)(const void *, struct file *, unsigned),
		const void *p)
{
	struct fdtable *fdt;
	int res = 0;
	if (!files)
		return 0;
	spin_lock(&files->file_lock);
	for (fdt = files_fdtable(files); n < fdt->max_fds; n++) {
		struct file *file;
		file = rcu_dereference_check_fdtable(files, fdt->fd[n]);
		if (!file)
			continue;
		res = f(p, file, n);
		if (res)
			break;
	}
	spin_unlock(&files->file_lock);
	return res;
}