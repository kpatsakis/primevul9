int close_fd(unsigned fd)
{
	struct files_struct *files = current->files;
	struct file *file;

	file = pick_file(files, fd);
	if (IS_ERR(file))
		return -EBADF;

	return filp_close(file, files);
}