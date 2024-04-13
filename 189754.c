static ssize_t oom_adjust_read(struct file *file, char __user *buf,
				size_t count, loff_t *ppos)
{
	struct task_struct *task = get_proc_task(file->f_path.dentry->d_inode);
	char buffer[PROC_NUMBUF];
	size_t len;
	int oom_adjust;

	if (!task)
		return -ESRCH;
	oom_adjust = task->oomkilladj;
	put_task_struct(task);

	len = snprintf(buffer, sizeof(buffer), "%i\n", oom_adjust);

	return simple_read_from_buffer(buf, count, ppos, buffer, len);
}