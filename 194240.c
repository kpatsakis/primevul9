static int cpia2_s_input(struct file *file, void *fh, unsigned int i)
{
	return i ? -EINVAL : 0;
}