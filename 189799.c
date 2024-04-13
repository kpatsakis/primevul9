static ssize_t environ_read(struct file *file, char __user *buf,
			size_t count, loff_t *ppos)
{
	struct task_struct *task = get_proc_task(file->f_dentry->d_inode);
	char *page;
	unsigned long src = *ppos;
	int ret = -ESRCH;
	struct mm_struct *mm;

	if (!task)
		goto out_no_task;

	if (!ptrace_may_attach(task))
		goto out;

	ret = -ENOMEM;
	page = (char *)__get_free_page(GFP_TEMPORARY);
	if (!page)
		goto out;

	ret = 0;

	mm = get_task_mm(task);
	if (!mm)
		goto out_free;

	while (count > 0) {
		int this_len, retval, max_len;

		this_len = mm->env_end - (mm->env_start + src);

		if (this_len <= 0)
			break;

		max_len = (count > PAGE_SIZE) ? PAGE_SIZE : count;
		this_len = (this_len > max_len) ? max_len : this_len;

		retval = access_process_vm(task, (mm->env_start + src),
			page, this_len, 0);

		if (retval <= 0) {
			ret = retval;
			break;
		}

		if (copy_to_user(buf, page, retval)) {
			ret = -EFAULT;
			break;
		}

		ret += retval;
		src += retval;
		buf += retval;
		count -= retval;
	}
	*ppos = src;

	mmput(mm);
out_free:
	free_page((unsigned long) page);
out:
	put_task_struct(task);
out_no_task:
	return ret;
}