urandom_read_nowarn(struct file *file, char __user *buf, size_t nbytes,
		    loff_t *ppos)
{
	int ret;

	nbytes = min_t(size_t, nbytes, INT_MAX >> (ENTROPY_SHIFT + 3));
	ret = extract_crng_user(buf, nbytes);
	trace_urandom_read(8 * nbytes, 0, ENTROPY_BITS(&input_pool));
	return ret;
}