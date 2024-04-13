static ssize_t bad_file_sendpage(struct file *file, struct page *page,
			int off, size_t len, loff_t *pos, int more)
{
	return -EIO;
}