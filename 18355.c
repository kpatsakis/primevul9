static int copy_compat_iovec_from_user(struct iovec *iov,
		const struct iovec __user *uvec, unsigned long nr_segs)
{
	const struct compat_iovec __user *uiov =
		(const struct compat_iovec __user *)uvec;
	int ret = -EFAULT, i;

	if (!user_access_begin(uiov, nr_segs * sizeof(*uiov)))
		return -EFAULT;

	for (i = 0; i < nr_segs; i++) {
		compat_uptr_t buf;
		compat_ssize_t len;

		unsafe_get_user(len, &uiov[i].iov_len, uaccess_end);
		unsafe_get_user(buf, &uiov[i].iov_base, uaccess_end);

		/* check for compat_size_t not fitting in compat_ssize_t .. */
		if (len < 0) {
			ret = -EINVAL;
			goto uaccess_end;
		}
		iov[i].iov_base = compat_ptr(buf);
		iov[i].iov_len = len;
	}

	ret = 0;
uaccess_end:
	user_access_end();
	return ret;
}