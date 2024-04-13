asmlinkage long compat_sys_fstatfs(unsigned int fd, struct compat_statfs __user *buf)
{
	struct file * file;
	struct kstatfs tmp;
	int error;

	error = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;
	error = vfs_statfs(file->f_dentry, &tmp);
	if (!error)
		error = put_compat_statfs(buf, &tmp);
	fput(file);
out:
	return error;
}