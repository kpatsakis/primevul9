static ssize_t random_write(struct file *file, const char __user *buffer,
			    size_t count, loff_t *ppos)
{
	size_t ret;

	ret = write_pool(&input_pool, buffer, count);
	if (ret)
		return ret;

	return (ssize_t)count;
}