struct file *task_lookup_next_fd_rcu(struct task_struct *task, unsigned int *ret_fd)
{
	/* Must be called with rcu_read_lock held */
	struct files_struct *files;
	unsigned int fd = *ret_fd;
	struct file *file = NULL;

	task_lock(task);
	files = task->files;
	if (files) {
		for (; fd < files_fdtable(files)->max_fds; fd++) {
			file = files_lookup_fd_rcu(files, fd);
			if (file)
				break;
		}
	}
	task_unlock(task);
	*ret_fd = fd;
	return file;
}