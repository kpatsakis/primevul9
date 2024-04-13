static int sg_build_iovec(sg_io_hdr_t __user *sgio, void __user *dxferp, u16 iovec_count)
{
	sg_iovec_t __user *iov = (sg_iovec_t __user *) (sgio + 1);
	sg_iovec32_t __user *iov32 = dxferp;
	int i;

	for (i = 0; i < iovec_count; i++) {
		u32 base, len;

		if (get_user(base, &iov32[i].iov_base) ||
		    get_user(len, &iov32[i].iov_len) ||
		    put_user(compat_ptr(base), &iov[i].iov_base) ||
		    put_user(len, &iov[i].iov_len))
			return -EFAULT;
	}

	if (put_user(iov, &sgio->dxferp))
		return -EFAULT;
	return 0;
}