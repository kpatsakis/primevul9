struct iovec *iovec_from_user(const struct iovec __user *uvec,
		unsigned long nr_segs, unsigned long fast_segs,
		struct iovec *fast_iov, bool compat)
{
	struct iovec *iov = fast_iov;
	int ret;

	/*
	 * SuS says "The readv() function *may* fail if the iovcnt argument was
	 * less than or equal to 0, or greater than {IOV_MAX}.  Linux has
	 * traditionally returned zero for zero segments, so...
	 */
	if (nr_segs == 0)
		return iov;
	if (nr_segs > UIO_MAXIOV)
		return ERR_PTR(-EINVAL);
	if (nr_segs > fast_segs) {
		iov = kmalloc_array(nr_segs, sizeof(struct iovec), GFP_KERNEL);
		if (!iov)
			return ERR_PTR(-ENOMEM);
	}

	if (compat)
		ret = copy_compat_iovec_from_user(iov, uvec, nr_segs);
	else
		ret = copy_iovec_from_user(iov, uvec, nr_segs);
	if (ret) {
		if (iov != fast_iov)
			kfree(iov);
		return ERR_PTR(ret);
	}

	return iov;
}