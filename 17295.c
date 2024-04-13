SYSCALL_DEFINE2(fchmod, unsigned int, fd, umode_t, mode)
{
	struct file * file;
	int err = -EBADF;

	file = fget(fd);
	if (file) {
		audit_inode(NULL, file->f_path.dentry, 0);
		err = chmod_common(&file->f_path, mode);
		fput(file);
	}
	return err;
}