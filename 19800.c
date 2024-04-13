ssize_t seq_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	struct iovec iov = { .iov_base = buf, .iov_len = size};
	struct kiocb kiocb;
	struct iov_iter iter;
	ssize_t ret;

	init_sync_kiocb(&kiocb, file);
	iov_iter_init(&iter, READ, &iov, 1, size);

	kiocb.ki_pos = *ppos;
	ret = seq_read_iter(&kiocb, &iter);
	*ppos = kiocb.ki_pos;
	return ret;
}