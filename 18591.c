struct file *fget(unsigned int fd)
{
	return __fget(fd, FMODE_PATH, 1);
}