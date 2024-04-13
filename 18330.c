static int copy_iovec_from_user(struct iovec *iov,
		const struct iovec __user *uvec, unsigned long nr_segs)
{
	unsigned long seg;

	if (copy_from_user(iov, uvec, nr_segs * sizeof(*uvec)))
		return -EFAULT;
	for (seg = 0; seg < nr_segs; seg++) {
		if ((ssize_t)iov[seg].iov_len < 0)
			return -EINVAL;
	}

	return 0;
}