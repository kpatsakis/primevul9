int receive_fd(struct file *file, unsigned int o_flags)
{
	return __receive_fd(file, NULL, o_flags);
}