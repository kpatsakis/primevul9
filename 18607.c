int close_fd_get_file(unsigned int fd, struct file **res)
{
	struct files_struct *files = current->files;
	int ret;

	spin_lock(&files->file_lock);
	ret = __close_fd_get_file(fd, res);
	spin_unlock(&files->file_lock);

	return ret;
}