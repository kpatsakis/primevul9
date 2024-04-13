static ssize_t do_loop_readv_writev(struct file *filp, struct iov_iter *iter,
		loff_t *ppos, io_fn_t fn)
{
	ssize_t ret = 0;

	while (iov_iter_count(iter)) {
		struct iovec iovec = iov_iter_iovec(iter);
		ssize_t nr;

		nr = fn(filp, iovec.iov_base, iovec.iov_len, ppos);

		if (nr < 0) {
			if (!ret)
				ret = nr;
			break;
		}
		ret += nr;
		if (nr != iovec.iov_len)
			break;
		iov_iter_advance(iter, nr);
	}

	return ret;
}