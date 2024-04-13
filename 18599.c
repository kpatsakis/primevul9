void do_close_on_exec(struct files_struct *files)
{
	unsigned i;
	struct fdtable *fdt;

	/* exec unshares first */
	spin_lock(&files->file_lock);
	for (i = 0; ; i++) {
		unsigned long set;
		unsigned fd = i * BITS_PER_LONG;
		fdt = files_fdtable(files);
		if (fd >= fdt->max_fds)
			break;
		set = fdt->close_on_exec[i];
		if (!set)
			continue;
		fdt->close_on_exec[i] = 0;
		for ( ; set ; fd++, set >>= 1) {
			struct file *file;
			if (!(set & 1))
				continue;
			file = fdt->fd[fd];
			if (!file)
				continue;
			rcu_assign_pointer(fdt->fd[fd], NULL);
			__put_unused_fd(files, fd);
			spin_unlock(&files->file_lock);
			filp_close(file, files);
			cond_resched();
			spin_lock(&files->file_lock);
		}

	}
	spin_unlock(&files->file_lock);
}