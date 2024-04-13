ssize_t __kernel_write(struct file *file, const char *buf, size_t count, loff_t *pos)
{
	mm_segment_t old_fs;
	const char __user *p;
	ssize_t ret;

	if (!(file->f_mode & FMODE_CAN_WRITE))
		return -EINVAL;

	old_fs = get_fs();
	set_fs(get_ds());
	p = (__force const char __user *)buf;
	if (count > MAX_RW_COUNT)
		count =  MAX_RW_COUNT;
	ret = __vfs_write(file, p, count, pos);
	set_fs(old_fs);
	if (ret > 0) {
		fsnotify_modify(file);
		add_wchar(current, ret);
	}
	inc_syscw(current);
	return ret;
}