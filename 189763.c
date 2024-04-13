static ssize_t oom_adjust_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	struct task_struct *task;
	char buffer[PROC_NUMBUF], *end;
	int oom_adjust;

	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count))
		return -EFAULT;
	oom_adjust = simple_strtol(buffer, &end, 0);
	if ((oom_adjust < OOM_ADJUST_MIN || oom_adjust > OOM_ADJUST_MAX) &&
	     oom_adjust != OOM_DISABLE)
		return -EINVAL;
	if (*end == '\n')
		end++;
	task = get_proc_task(file->f_path.dentry->d_inode);
	if (!task)
		return -ESRCH;
	if (oom_adjust < task->oomkilladj && !capable(CAP_SYS_RESOURCE)) {
		put_task_struct(task);
		return -EACCES;
	}
	task->oomkilladj = oom_adjust;
	put_task_struct(task);
	if (end - buffer == 0)
		return -EIO;
	return end - buffer;
}