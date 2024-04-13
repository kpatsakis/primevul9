int receive_fd_replace(int new_fd, struct file *file, unsigned int o_flags)
{
	int error;

	error = security_file_receive(file);
	if (error)
		return error;
	error = replace_fd(new_fd, file, o_flags);
	if (error)
		return error;
	__receive_sock(file);
	return new_fd;
}