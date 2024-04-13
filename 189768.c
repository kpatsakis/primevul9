static ssize_t proc_loginuid_write(struct file * file, const char __user * buf,
				   size_t count, loff_t *ppos)
{
	struct inode * inode = file->f_path.dentry->d_inode;
	char *page, *tmp;
	ssize_t length;
	uid_t loginuid;

	if (!capable(CAP_AUDIT_CONTROL))
		return -EPERM;

	if (current != pid_task(proc_pid(inode), PIDTYPE_PID))
		return -EPERM;

	if (count >= PAGE_SIZE)
		count = PAGE_SIZE - 1;

	if (*ppos != 0) {
		/* No partial writes. */
		return -EINVAL;
	}
	page = (char*)__get_free_page(GFP_TEMPORARY);
	if (!page)
		return -ENOMEM;
	length = -EFAULT;
	if (copy_from_user(page, buf, count))
		goto out_free_page;

	page[count] = '\0';
	loginuid = simple_strtoul(page, &tmp, 10);
	if (tmp == page) {
		length = -EINVAL;
		goto out_free_page;

	}
	length = audit_set_loginuid(current, loginuid);
	if (likely(length == 0))
		length = count;

out_free_page:
	free_page((unsigned long) page);
	return length;
}