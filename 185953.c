static ssize_t do_iter_readv_writev(struct file *filp, struct iov_iter *iter,
		loff_t *ppos, iter_fn_t fn)
{
	struct kiocb kiocb;
	ssize_t ret;

	init_sync_kiocb(&kiocb, filp);
	kiocb.ki_pos = *ppos;

	ret = fn(&kiocb, iter);
	BUG_ON(ret == -EIOCBQUEUED);
	*ppos = kiocb.ki_pos;
	return ret;
}