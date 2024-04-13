static int tw5864_s_input(struct file *file, void *priv, unsigned int i)
{
	if (i)
		return -EINVAL;
	return 0;
}