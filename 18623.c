struct file *task_lookup_fd_rcu(struct task_struct *task, unsigned int fd)
{
	/* Must be called with rcu_read_lock held */
	struct files_struct *files;
	struct file *file = NULL;

	task_lock(task);
	files = task->files;
	if (files)
		file = files_lookup_fd_rcu(files, fd);
	task_unlock(task);

	return file;
}