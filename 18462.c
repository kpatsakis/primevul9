random_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	int ret;

	ret = wait_for_random_bytes();
	if (ret != 0)
		return ret;
	return urandom_read_nowarn(file, buf, nbytes, ppos);
}