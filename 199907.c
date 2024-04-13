static ssize_t bad_file_splice_write(struct pipe_inode_info *pipe,
			struct file *out, loff_t *ppos, size_t len,
			unsigned int flags)
{
	return -EIO;
}