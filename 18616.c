struct file *fget_task(struct task_struct *task, unsigned int fd)
{
	struct file *file = NULL;

	task_lock(task);
	if (task->files)
		file = __fget_files(task->files, fd, 0, 1);
	task_unlock(task);

	return file;
}