int __receive_fd(struct file *file, int __user *ufd, unsigned int o_flags)
{
	int new_fd;
	int error;

	error = security_file_receive(file);
	if (error)
		return error;

	new_fd = get_unused_fd_flags(o_flags);
	if (new_fd < 0)
		return new_fd;

	if (ufd) {
		error = put_user(new_fd, ufd);
		if (error) {
			put_unused_fd(new_fd);
			return error;
		}
	}

	fd_install(new_fd, get_file(file));
	__receive_sock(file);
	return new_fd;
}