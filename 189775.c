static ssize_t proc_fdinfo_read(struct file *file, char __user *buf,
				      size_t len, loff_t *ppos)
{
	char tmp[PROC_FDINFO_MAX];
	int err = proc_fd_info(file->f_path.dentry->d_inode, NULL, NULL, tmp);
	if (!err)
		err = simple_read_from_buffer(buf, len, ppos, tmp, strlen(tmp));
	return err;
}