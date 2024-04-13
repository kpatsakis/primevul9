int single_open_size(struct file *file, int (*show)(struct seq_file *, void *),
		void *data, size_t size)
{
	char *buf = seq_buf_alloc(size);
	int ret;
	if (!buf)
		return -ENOMEM;
	ret = single_open(file, show, data);
	if (ret) {
		kvfree(buf);
		return ret;
	}
	((struct seq_file *)file->private_data)->buf = buf;
	((struct seq_file *)file->private_data)->size = size;
	return 0;
}