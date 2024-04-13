static int ksys_dup3(unsigned int oldfd, unsigned int newfd, int flags)
{
	int err = -EBADF;
	struct file *file;
	struct files_struct *files = current->files;

	if ((flags & ~O_CLOEXEC) != 0)
		return -EINVAL;

	if (unlikely(oldfd == newfd))
		return -EINVAL;

	if (newfd >= rlimit(RLIMIT_NOFILE))
		return -EBADF;

	spin_lock(&files->file_lock);
	err = expand_files(files, newfd);
	file = files_lookup_fd_locked(files, oldfd);
	if (unlikely(!file))
		goto Ebadf;
	if (unlikely(err < 0)) {
		if (err == -EMFILE)
			goto Ebadf;
		goto out_unlock;
	}
	return do_dup2(files, file, newfd, flags);

Ebadf:
	err = -EBADF;
out_unlock:
	spin_unlock(&files->file_lock);
	return err;
}