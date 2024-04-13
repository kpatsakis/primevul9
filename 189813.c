static ssize_t proc_pid_attr_write(struct file * file, const char __user * buf,
				   size_t count, loff_t *ppos)
{
	struct inode * inode = file->f_path.dentry->d_inode;
	char *page;
	ssize_t length;
	struct task_struct *task = get_proc_task(inode);

	length = -ESRCH;
	if (!task)
		goto out_no_task;
	if (count > PAGE_SIZE)
		count = PAGE_SIZE;

	/* No partial writes. */
	length = -EINVAL;
	if (*ppos != 0)
		goto out;

	length = -ENOMEM;
	page = (char*)__get_free_page(GFP_TEMPORARY);
	if (!page)
		goto out;

	length = -EFAULT;
	if (copy_from_user(page, buf, count))
		goto out_free;

	length = security_setprocattr(task,
				      (char*)file->f_path.dentry->d_name.name,
				      (void*)page, count);
out_free:
	free_page((unsigned long) page);
out:
	put_task_struct(task);
out_no_task:
	return length;
}