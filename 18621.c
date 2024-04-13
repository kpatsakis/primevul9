static inline void __range_cloexec(struct files_struct *cur_fds,
				   unsigned int fd, unsigned int max_fd)
{
	struct fdtable *fdt;

	/* make sure we're using the correct maximum value */
	spin_lock(&cur_fds->file_lock);
	fdt = files_fdtable(cur_fds);
	max_fd = min(last_fd(fdt), max_fd);
	if (fd <= max_fd)
		bitmap_set(fdt->close_on_exec, fd, max_fd - fd + 1);
	spin_unlock(&cur_fds->file_lock);
}