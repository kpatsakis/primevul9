static int bad_file_fasync(int fd, struct file *filp, int on)
{
	return -EIO;
}