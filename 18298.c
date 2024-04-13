ssize_t __import_iovec(int type, const struct iovec __user *uvec,
		 unsigned nr_segs, unsigned fast_segs, struct iovec **iovp,
		 struct iov_iter *i, bool compat)
{
	ssize_t total_len = 0;
	unsigned long seg;
	struct iovec *iov;

	iov = iovec_from_user(uvec, nr_segs, fast_segs, *iovp, compat);
	if (IS_ERR(iov)) {
		*iovp = NULL;
		return PTR_ERR(iov);
	}

	/*
	 * According to the Single Unix Specification we should return EINVAL if
	 * an element length is < 0 when cast to ssize_t or if the total length
	 * would overflow the ssize_t return value of the system call.
	 *
	 * Linux caps all read/write calls to MAX_RW_COUNT, and avoids the
	 * overflow case.
	 */
	for (seg = 0; seg < nr_segs; seg++) {
		ssize_t len = (ssize_t)iov[seg].iov_len;

		if (!access_ok(iov[seg].iov_base, len)) {
			if (iov != *iovp)
				kfree(iov);
			*iovp = NULL;
			return -EFAULT;
		}

		if (len > MAX_RW_COUNT - total_len) {
			len = MAX_RW_COUNT - total_len;
			iov[seg].iov_len = len;
		}
		total_len += len;
	}

	iov_iter_init(i, type, iov, nr_segs, total_len);
	if (iov == *iovp)
		*iovp = NULL;
	else
		*iovp = iov;
	return total_len;
}