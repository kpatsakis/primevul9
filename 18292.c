ssize_t import_iovec(int type, const struct iovec __user *uvec,
		 unsigned nr_segs, unsigned fast_segs,
		 struct iovec **iovp, struct iov_iter *i)
{
	return __import_iovec(type, uvec, nr_segs, fast_segs, iovp, i,
			      in_compat_syscall());
}