static ssize_t bad_file_splice_read(struct file *in, loff_t *ppos,
			struct pipe_inode_info *pipe, size_t len,
			unsigned int flags)
{
	return -EIO;
}