size_t _copy_from_iter(void *addr, size_t bytes, struct iov_iter *i)
{
	if (unlikely(iov_iter_is_pipe(i))) {
		WARN_ON(1);
		return 0;
	}
	if (iter_is_iovec(i))
		might_fault();
	iterate_and_advance(i, bytes, base, len, off,
		copyin(addr + off, base, len),
		memcpy(addr + off, base, len)
	)

	return bytes;
}