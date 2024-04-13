static int random_fasync(int fd, struct file *filp, int on)
{
	return fasync_helper(fd, filp, on, &fasync);
}