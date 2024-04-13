static ssize_t mem_write(struct file * file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	int copied;
	char *page;
	struct task_struct *task = get_proc_task(file->f_path.dentry->d_inode);
	unsigned long dst = *ppos;

	copied = -ESRCH;
	if (!task)
		goto out_no_task;

	if (!MAY_PTRACE(task) || !ptrace_may_attach(task))
		goto out;

	copied = -ENOMEM;
	page = (char *)__get_free_page(GFP_TEMPORARY);
	if (!page)
		goto out;

	copied = 0;
	while (count > 0) {
		int this_len, retval;

		this_len = (count > PAGE_SIZE) ? PAGE_SIZE : count;
		if (copy_from_user(page, buf, this_len)) {
			copied = -EFAULT;
			break;
		}
		retval = access_process_vm(task, dst, page, this_len, 1);
		if (!retval) {
			if (!copied)
				copied = -EIO;
			break;
		}
		copied += retval;
		buf += retval;
		dst += retval;
		count -= retval;			
	}
	*ppos = dst;
	free_page((unsigned long) page);
out:
	put_task_struct(task);
out_no_task:
	return copied;
}