static ssize_t bad_file_sendfile(struct file *in_file, loff_t *ppos,
			size_t count, read_actor_t actor, void *target)
{
	return -EIO;
}