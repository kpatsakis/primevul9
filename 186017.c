ssize_t vfs_iter_write(struct file *file, struct iov_iter *iter, loff_t *ppos)
{
	struct kiocb kiocb;
	ssize_t ret;

	if (!file->f_op->write_iter)
		return -EINVAL;

	init_sync_kiocb(&kiocb, file);
	kiocb.ki_pos = *ppos;

	iter->type |= WRITE;
	ret = file->f_op->write_iter(&kiocb, iter);
	BUG_ON(ret == -EIOCBQUEUED);
	if (ret > 0)
		*ppos = kiocb.ki_pos;
	return ret;
}