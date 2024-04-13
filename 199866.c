asmlinkage long compat_sys_getdents64(unsigned int fd,
		struct linux_dirent64 __user * dirent, unsigned int count)
{
	struct file * file;
	struct linux_dirent64 __user * lastdirent;
	struct compat_getdents_callback64 buf;
	int error;

	error = -EFAULT;
	if (!access_ok(VERIFY_WRITE, dirent, count))
		goto out;

	error = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;

	buf.current_dir = dirent;
	buf.previous = NULL;
	buf.count = count;
	buf.error = 0;

	error = vfs_readdir(file, compat_filldir64, &buf);
	if (error < 0)
		goto out_putf;
	error = buf.error;
	lastdirent = buf.previous;
	if (lastdirent) {
		typeof(lastdirent->d_off) d_off = file->f_pos;
		__put_user_unaligned(d_off, &lastdirent->d_off);
		error = count - buf.count;
	}

out_putf:
	fput(file);
out:
	return error;
}