int import_single_range(int rw, void __user *buf, size_t len,
		 struct iovec *iov, struct iov_iter *i)
{
	if (len > MAX_RW_COUNT)
		len = MAX_RW_COUNT;
	if (unlikely(!access_ok(buf, len)))
		return -EFAULT;

	iov->iov_base = buf;
	iov->iov_len = len;
	iov_iter_init(i, rw, iov, 1, len);
	return 0;
}