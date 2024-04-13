int replace_fd(unsigned fd, struct file *file, unsigned flags)
{
	int err;
	struct files_struct *files = current->files;

	if (!file)
		return close_fd(fd);

	if (fd >= rlimit(RLIMIT_NOFILE))
		return -EBADF;

	spin_lock(&files->file_lock);
	err = expand_files(files, fd);
	if (unlikely(err < 0))
		goto out_unlock;
	return do_dup2(files, file, fd, flags);

out_unlock:
	spin_unlock(&files->file_lock);
	return err;
}