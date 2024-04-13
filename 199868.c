asmlinkage long compat_sys_fstatfs64(unsigned int fd, compat_size_t sz, struct compat_statfs64 __user *buf)
{
	struct file * file;
	struct kstatfs tmp;
	int error;

	if (sz != sizeof(*buf))
		return -EINVAL;

	error = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;
	error = vfs_statfs(file->f_dentry, &tmp);
	if (!error)
		error = put_compat_statfs64(buf, &tmp);
	fput(file);
out:
	return error;
}