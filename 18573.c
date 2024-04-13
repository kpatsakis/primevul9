static inline void __range_close(struct files_struct *cur_fds, unsigned int fd,
				 unsigned int max_fd)
{
	while (fd <= max_fd) {
		struct file *file;

		file = pick_file(cur_fds, fd++);
		if (!IS_ERR(file)) {
			/* found a valid file to close */
			filp_close(file, cur_fds);
			cond_resched();
			continue;
		}

		/* beyond the last fd in that table */
		if (PTR_ERR(file) == -EINVAL)
			return;
	}
}